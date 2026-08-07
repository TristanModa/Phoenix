// #pragma hlsl profile ps_6_6
// #pragma hlsl entry main

[shader("pixel")]
float4 main(float4 color : TEXCOORD0) : SV_Target0 {
    return color;
}
