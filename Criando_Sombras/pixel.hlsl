cbuffer cbPerObject : register(b0)
{
    float4x4 worldViewProj;
    float3 cameraPos;
    float3 ambientLight;
}


struct pixelIn
{
    float4 posH : SV_POSITION;
    float4 Color : COLOR;
    float3 normal : NORMAL;
};

float4 main(pixelIn pIn) : SV_TARGET
{
    //return mul(pIn.Color, float4(ambientLight, 1.0f));
    //float3 d = cos(dot(pIn.normal, cross(cameraPos - pIn.posH.xyz, ambientLight)));
    float3 teste = float3(.01f, .01f, .01f);
    float3 d = 1.2f * cos(dot(pIn.normal, cross(pIn.posH.xyz - cameraPos, ambientLight)));
    //float3 d = teste + (1 - teste) * (1 - dot(pIn.normal, ambientLight));
    return float4(d * pIn.Color.xyz, pIn.Color.w);
    //return pIn.Color;
}