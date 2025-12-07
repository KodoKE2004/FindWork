#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

// ピクセルシェーダーのエントリポイント
float4 main(in PS_IN input) : SV_Target
{
	//float4 color;
    // Sample関数→テクスチャから該当のUV位置のピクセル色を取って来る
	//color = g_Texture.Sample(g_SamplerState, input.tex);
	//color *= input.col;
	//color = gLight.gLightColor; // 光の色を掛ける
	//color = input.col;
	//return color;
	
	 // 正規化
	float3 N = normalize(input.nrm);
	float3 L = normalize(-gLight.gLightDirection.xyz);

    // 拡散反射
	float NdotL = max(dot(N, L), 0.0f);
	float3 diffuse = gLight.gLightColor.rgb * gLight.gDiffuseI * NdotL;

    // 環境光
	float3 ambient = gLight.gLightColor.rgb * gLight.gAmbientI;

    // 鏡面反射（簡易 Blinn-Phong）
	float3 V = normalize(float3(0, 0, 1)); // 仮:カメラ前方固定（テスト用）
	float3 H = normalize(L + V);
	float spec = pow(max(dot(N, H), 0.0f), 16.0f);
	float3 specular = gLight.gLightColor.rgb * gLight.gSpecularI * spec;

	float3 lighting = ambient + diffuse + specular;

    // テクスチャサンプル
	float4 texColor = g_Texture.Sample(g_SamplerState, input.tex);

	return float4(texColor.rgb * lighting, texColor.a);
}
