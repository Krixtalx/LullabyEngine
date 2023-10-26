struct Vertex{
    [[vk::binding(0)]] float3 position : POSITION;
    [[vk::binding(0)]] float3 normal : NORMAL;
    [[vk::binding(0)]] float3 color : COLOR;
}

// Vertex  Shader
struct VertexStageOutput
{
    float4 sv_position : SV_Position;
    float3 normal: FragmentNormal;
    float3 color : FragmentColor;
};

struct PushConstants{
    float4 data;
    float4x4 renderMatrix;
}
[[vk::push_constant]]
ConstantBuffer<PushConstants> pushConstantsBuffer;


/*[[vk::push_constant]]
cbuffer pushConstantsBuffer{
    PushConstants data;
} */

[shader("vertex")]
VertexStageOutput vertexMain(Vertex inputVertex, PushConstants pushConstants )
{
    VertexStageOutput output;

    output.sv_position = mul(pushConstants.renderMatrix, float4(inputVertex.position, 1));
    output.color = inputVertex.color;
    output.normal = inputVertex.normal;

    return output;
}

// Fragment Shader

[shader("fragment")]
float4 fragmentMain(float3 color: FragmentColor)
    : SV_Target
{
    return float4(color, 1.0);
}
