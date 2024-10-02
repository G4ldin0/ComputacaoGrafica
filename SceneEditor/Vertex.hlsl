cbuffer Object
{
    float4x4 w;
    float4x4 v;
    float4x4 p;
    
    bool selected;
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
    vertexOut vout;
    vout.posH = mul(float4(pIn.pos, 1.0f), w);
    vout.posH = mul(vout.posH, v);
    vout.posH = mul(vout.posH, p);
    //vout.posH = float4(pIn.pos, 1.0f);
    vout.color = pIn.color;
    return vout;
}