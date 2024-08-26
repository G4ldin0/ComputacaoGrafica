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
    vout.posH = float4(pIn.pos, 1.0f);
    vout.color = pIn.color;
    return vout;
}