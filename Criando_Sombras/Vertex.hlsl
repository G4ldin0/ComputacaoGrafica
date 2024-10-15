cbuffer cbPerObject : register(b0)
{
    float4x4 world;
    float4x4 ViewProj;
    float3 cameraPos;
    float _blank;
    float3 lightDirection;
}


struct vertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
};

struct vertexOut
{
    float4 posH : SV_POSITION;
    float4 posW : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
};

vertexOut main( vertexIn pIn )
{
    vertexOut vout;
    vout.posH = mul(float4(pIn.pos, 1.0f), world);
    vout.posH = mul(vout.posH, ViewProj);
    vout.posW = mul(float4(pIn.pos, 1.0f), world);
    vout.color = pIn.color;
    vout.normal = mul(pIn.normal, (float3x3) world);
    return vout;
}