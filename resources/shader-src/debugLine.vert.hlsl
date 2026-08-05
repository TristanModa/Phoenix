struct VSInput {
    float2 position : POSITION;
	float4 color    : COLOR0;
};

struct VSOutput {
    float4 position : SV_Position;
    float4 color    : TEXCOORD0;
};

cbuffer UniformBlock : register(b0, space1) {
    float4x4 matrixTransform : packoffset(c0);
}

[shader("vertex")]
VSOutput main(VSInput input, uint vertexID : SV_VertexID) {
    VSOutput output;

    float z = vertexID / 1e-6f;
    output.position = mul(matrixTransform, float4(input.position, z, 1.0f));
    output.color = input.color;

    return output;
}
