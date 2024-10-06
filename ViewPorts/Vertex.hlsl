cbuffer Object
{
    float4x4 World;
    float4x4 View;
    float4x4 Proj;
};

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
    //float4 vecTarget = Target - Pos;
    //float4 w = vecTarget / length(vecTarget);
    //float4 crossProduct = float4(cross(Up.xyz, w.xyz), 0.0f);
    //float4 u = crossProduct / length(crossProduct);
    //float4 v = float4(cross(w.xyz, u.xyz), 0.0f);
    //float4x4 V = float4x4(u, v, w, Pos);
    //float4x4 w = World * View * Proj;
    
    vertexOut vout;
    vout.posH = mul(float4(pIn.pos, 1.0f), transpose(World));
    //vout.posH = mul(vout.posH, transpose(View));
    //vout.posH = mul(vout.posH, transpose(Proj));
    
    vout.color = pIn.color;
    return vout;
}