#include "common.hlsl"

cbuffer FadeBuffer : register(b6)
{
	float2 CenterUV;
	float FadeProgress;
	float MaxInnerRadius;
	float edgeWidth;
};

Texture2D    tex : register(t0);
SamplerState samp : register(s0);

float4 main(PS_IN input) : SV_TARGET
{
	float4 color = tex.Sample(samp, input.tex);
	
	// ’†S‚©‚ç‚Ì‹——£
	float2 d = input.tex - CenterUV;
	float dist = length(d);
	
	// ¡‚Ç‚±‚Ü‚Å“§–¾‚ªL‚ª‚Á‚Ä‚¢‚é‚©(’†S‚©‚ç‚Ì”¼Œa)
	float innerRadius = MaxInnerRadius * FadeProgress;

	float edge = edgeWidth;
	float alphaMask = saturate((dist - innerRadius) / edge);
	
	color.a *= alphaMask;
	
	return color;
}