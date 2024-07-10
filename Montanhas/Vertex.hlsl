cbuffer cbPerObject : register(b0)
{
    float4x4 worldViewProj;
    float3 CameraPos;
    float3 LightDirection;
    float3 LightColor;
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
};

vertexOut main( vertexIn pIn )
{
    
    
    float3 cameraDir = CameraPos - pIn.pos;
    float3 v = cameraDir / abs(cameraDir);
    
    
    float3 r = LightDirection - 2 * dot(pIn.normal, LightDirection) * pIn.normal;
    
    vertexOut vout;
    vout.posH = mul(float4(pIn.pos, 1.0f), worldViewProj);
    //vout.color = pIn.color * float4(LightColor, 1.0f);
    //vout.color = pIn.color * 0.9f;
    //vout.color = float4(pIn.color.x * 0.9f, pIn.color.y * 0.9f, pIn.color.z * 0.9f, pIn.color.w);
    //vout.color = pIn.color;
    vout.color = float4(v, 1.0f);
    return vout;
}