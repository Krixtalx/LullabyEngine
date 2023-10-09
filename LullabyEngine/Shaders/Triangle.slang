// Output of the fragment shader
struct Fragment
{
    float4 color;
};

// Vertex  Shader

struct VertexStageOutput
{
    float3          color           : float3;
    float4          sv_position     : SV_Position;
};

[shader("vertex")]
VertexStageOutput vertexMain(uint vertexID : SV_VertexID,){
    VertexStageOutput output;
    const float3 positions[3] = {
		float3(1.f,1.f, 0.0f),
		float3(-1.f,1.f, 0.0f),
		float3(0.f,-1.f, 0.0f)
	};
    output.sv_position = (positions[vertexID], 1.0f);
    return output;
}

[shader("fragment")]
float4 fragmentMain(float3 color) : SV_Target{
    return float4(color, 1.0);
}