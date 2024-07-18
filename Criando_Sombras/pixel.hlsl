cbuffer cbPerObject : register(b0)
{
    float4x4 worldViewProj;
    float3 ambientLight;
}


struct pixelIn
{
    float4 posH : SV_POSITION;
    float4 Color : COLOR;
};

float4 main(pixelIn pIn) : SV_TARGET
{
    //return mul(pIn.Color, float4(ambientLight, 1.0f));
    return pIn.Color;
}