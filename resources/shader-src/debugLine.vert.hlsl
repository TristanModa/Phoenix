struct VSOutput {
    float4 position : SV_Position;
    float4 color    : TEXCOORD0;
};

[shader("vertex")]
VSOutput main(uint vertexIndex : SV_VertexID) {
    VSOutput output;
    if (vertexIndex == 0) {
        output.position = float4(-1, -1, 0, 1);
        output.color    = float4(1, 0, 0, 1);
    } else if (vertexIndex == 1) {
        output.position = float4(1, -1, 0, 1);
        output.color    = float4(0, 1, 0, 1);
    } else {
        output.position = float4(0, 1, 0, 1);
        output.color    = float4(0, 0, 1, 1);
    }
    return output;
}
