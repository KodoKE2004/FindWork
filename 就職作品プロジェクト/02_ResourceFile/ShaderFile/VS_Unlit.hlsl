cbuffer PerObjectBuffer : register(b0)
{
	float4x4 world;
	float4x4 view;
};

struct VSIn
{
	float3 pos : POSITION;
};

struct VSOut
{
	float4 svpos : SV_POSITION;
};

VSOut main(VSIn i)
{
	VSOut o;
	float4 wpos = mul(float4(i.pos, 1.0), world);
	o.svpos = mul(wpos, view);
	return o;
}