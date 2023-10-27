
struct PushConstants{
    float4 data;
    float4x4 renderMatrix;
};
[[vk::push_constant]]
PushConstants push;

[[vk::binding(0)]] 
struct Vertex{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float3 color : COLOR;
};

// Vertex  Shader
struct VertexStageOutput
{
    float4 sv_position : SV_Position;
    float3 normal: FragmentNormal;
    float3 color : FragmentColor;
};

[shader("vertex")]
VertexStageOutput vertexMain(Vertex inputVertex)
{
    VertexStageOutput output;

    output.sv_position = mul(push.renderMatrix, float4(inputVertex.position, 1));
    output.normal = inputVertex.normal;
    output.color = inputVertex.color;

    return output;
}

// Fragment Shader

[shader("fragment")]
float4 fragmentMain(float3 color: FragmentColor)
    : SV_Target
{
    return float4(color, 1.0);
}