// #pragma hlsl profile ps_6_6
// #pragma hlsl entry main

Texture2D<float4> virtualDisplayTarget : register(t0, space2);
cbuffer UniformData : register(b0, space3) {
    int2 displaySize;
    int2 windowSize;
    float2 cameraSubpixel;
}

[shader("pixel")]
float4 main(float2 uv : TEXCOORD0) : SV_Target {
    // Calculate the aspect ratio of the display and window
    float displayAspectRatio = float(displaySize.x) / float(displaySize.y);
    float windowAspectRatio = float(windowSize.x) / float(windowSize.y);

    // Transform the UV
    float2 transformedUV = uv - 0.5f;
    if (windowAspectRatio > displayAspectRatio) {
        transformedUV.x *= (windowAspectRatio / displayAspectRatio);
    } else {
        transformedUV.y *= (displayAspectRatio / windowAspectRatio);
    }
    transformedUV += 0.5f;

    // Return black for the regions that should be letterboxed
    if (transformedUV.x < 0.0f || transformedUV.x > 1.0f ||
        transformedUV.y < 0.0f || transformedUV.y > 1.0f) {
        return float4(0.1f, 0.0f, 0.0f, 1.0f);
    }

    // Add the camera subpixel to the UV
    transformedUV = transformedUV + cameraSubpixel / displaySize;

    // Return the pixel on the texture to use
    int2 pixelCoords = transformedUV * displaySize + int2(0, 1);
    return virtualDisplayTarget.Load(int3(pixelCoords, 0));
}

