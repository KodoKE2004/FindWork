cbuffer CBScene : register(b0)
{
	float4x4 gWorld;
	float4x4 gView;
	float4x4 gProj;
};

struct VSIn
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD0;
};

struct VSOut
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

VSOut main(VSIn v)
{
	VSOut o;
	float4 wp = mul(float4(v.pos, 1), gWorld);
	float4 vp = mul(wp, gView);
	o.svpos = mul(vp, gProj);
	o.uv = v.uv;
	return o;
}
