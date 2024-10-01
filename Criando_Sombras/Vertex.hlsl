cbuffer cbPerObject : register(b0)
{
    float4x4 worldViewProj;
    float3 cameraPos;
    float3 ambientLight;
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
    float4 color : COLOR;
    float3 normal : NORMAL;
};

vertexOut main( vertexIn pIn )
{
    vertexOut vout;
    vout.posH = mul(float4(pIn.pos, 1.0f), worldViewProj);
    float lightAmount = ((dot(pIn.normal, ambientLight) + 1.0f) / 1.5f);
    //float lightAmount = 1;
    vout.color = pIn.color * lightAmount;
    vout.normal = pIn.normal;
    //vout.color = float4(ambientLight, 1.0f);
    //vout.normal = pIn.normal;
    return vout;
}