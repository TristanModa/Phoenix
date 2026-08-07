// #pragma hlsl profile vs_6_6
// #pragma hlsl entry main

struct VSInput {
    int2 position : POSITION;
	float4 color  : COLOR0;
};

struct VSOutput {
    float4 position : SV_Position;
    float4 color    : TEXCOORD0;
};

cbuffer UniformData : register(b0, space1) {
    int2 displaySize;
    int2 cameraPos;
}

[shader("vertex")]
VSOutput main(VSInput input, uint vertexID : SV_VertexID) {
    VSOutput output;

    int2 pixelPos = input.position - cameraPos;
    float2 normalizedPos = float2(pixelPos) / float2(displaySize);
    output.position.xy = (normalizedPos * 2) - 1;
    output.position.zw = float2(vertexID / 1e-6f, 1.0f);

    output.color = input.color;
    return output;
}
