#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

float4 main(PS_IN input) : SV_Target
{
   // テクスチャと頂点カラーを掛け合わせる
	float4 texColor = g_Texture.Sample(g_SamplerState, input.tex) * input.col;

    // マテリアルの Diffuse を掛け合わせる (ここに α が入る)
	float4 finalColor = texColor * Material.Diffuse;

	return finalColor;
}