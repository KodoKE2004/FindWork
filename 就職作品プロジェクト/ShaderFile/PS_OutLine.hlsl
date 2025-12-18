#include "common.hlsl"

cbuffer Outline : register(b6)
{
	float4 gOutlineColor;	
	float2 gTexelSize;		
	float  gAlphaThreshold;	
	int    gRadius;			
};

Texture2D    tex : register(t0);
SamplerState samp : register(s0);

float4 main(PS_IN input) : SV_TARGET
{
	float4 color = tex.Sample(samp, input.tex);

    // 1) 元が塗られているなら、そのまま（アウトラインは外側のみ）
	if (color.a >= gAlphaThreshold)
	{
		return color * input.col;
	}

    // 2) 現在ピクセルは透明。近傍に塗られたピクセルがあればアウトライン
	bool nearOpaque = false;

    // gRadius=1なら3x3、2なら5x5…の探索
    [loop]
	for (int y = -gRadius; y <= gRadius; ++y)
	{
        [loop]
		for (int x = -gRadius; x <= gRadius; ++x)
		{
            // 自分自身は見なくてOK
			if (x == 0 && y == 0)
				continue;

			float2 uv2 = input. + float2(x, y) * gTexelSize;

            // Clampサンプラなら不要だが、念のため範囲外を避けたい場合は saturate
            // uv2 = saturate(uv2);

			float a = tex.Sample(samp, uv2).a;
			if (a >= gAlphaThreshold)
			{
				nearOpaque = true;
				break;
			}
		}
		if (nearOpaque)
			break;
	}

	if (nearOpaque)
	{
        // アウトライン色だけ描く（元は透明なので base は使わない）
        // 透過は gOutlineColor.a で制御
		return float4(gOutlineColor.rgb, gOutlineColor.a) * input.col;
	}

    // 3) 近傍にも何も無いなら透明のまま
	return float4(0, 0, 0, 0);
	
	return color;
	
}