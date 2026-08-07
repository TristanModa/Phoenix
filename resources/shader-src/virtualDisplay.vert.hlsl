// #pragma hlsl profile vs_6_6
// #pragma hlsl entry main

struct VSOutput {
    float4 position : SV_Position;
    float2 uv       : TEXCOORD0;
};

const static float2 vertexPos[4] = {
	float2(-1.0f,  1.0f),
	float2( 1.0f,  1.0f),
	float2(-1.0f, -1.0f),
	float2( 1.0f, -1.0f),
};
const static float2 vertexUV[4] = {
    float2(0.0f, 0.0f),
    float2(1.0f, 0.0f),
    float2(0.0f, 1.0f),
    float2(1.0f, 1.0f),
};

[shader("vertex")]
VSOutput main(uint vertexID : SV_VertexID) {
    VSOutput output;
    output.position = float4(vertexPos[vertexID], 0.0f, 1.0f);
    output.uv = vertexUV[vertexID];
    return output;
}
