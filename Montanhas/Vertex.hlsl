cbuffer cbPerObject : register(b0)
{
    float4x4 worldViewProj;
}

struct vertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct vertexOut
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
};

vertexOut main( vertexIn pIn )
{
    vertexOut vout;
    vout.posH = mul(float4(pIn.pos, 1.0f), worldViewProj);
    vout.color = pIn.color;
    return vout;
}