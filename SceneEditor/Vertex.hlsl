
cbuffer Object: register(b0)
{
    float4x4 w;
    //float4x4 v;
    //float4x4 p;
    bool selected;
};

cbuffer proj : register(b1)
{
    float4x4 proj;
    float4x4 space1;
    float4x4 space2;
    float4x4 space3;
};

float4x4 view : register(b2);



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
    //vout.posH = mul(vout.posH, v);
    vout.posH = mul(vout.posH, view);
    vout.posH = mul(vout.posH, proj);
    //vout.posH = mul(vout.posH, transpose(p));
    
    if (selected){
        vout.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else{
        vout.color = pIn.color;
    }
    return vout;
}