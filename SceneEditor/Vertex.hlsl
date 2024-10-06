
cbuffer Object: register(b0)
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
    vout.posH = float4(pIn.pos, 1.0f);
    vout.posH = mul(vout.posH, w);
    vout.posH = mul(vout.posH, v);
    vout.posH = mul(vout.posH, p);
    //vout.posH = mul(vout.posH, transpose(v));
    //vout.posH = mul(vout.posH, transpose(p));
    
    if (selected){
        vout.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else{
        vout.color = pIn.color;
    }
    return vout;
}