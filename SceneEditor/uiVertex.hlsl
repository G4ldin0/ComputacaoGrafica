struct VertexIn
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VertexOut
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VertexOut main( VertexIn v )
{
    VertexOut vout;
    vout.pos = float4(v.pos, 1.0f);
    vout.color = v.color;
	return vout;
}