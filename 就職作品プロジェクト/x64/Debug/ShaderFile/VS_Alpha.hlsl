#include "common.hlsl"

PS_IN main(in VS_IN input)
{
	PS_IN output = (PS_IN) 0;
	
	// 座標変換
    matrix wvp;
    wvp = mul(World, View); // ワールド ⇒ ビュー
    wvp = mul(wvp, Projection); // ビュー　 ⇒ プロジェクション
    output.pos = mul(input.pos, wvp); // 最終的にoutput.posに代入しラスタライザに渡される

	float4 uv = (float4) 0;
    uv.xy = input.tex;
	uv.w  = input.tex.y; // テクスチャ座標のyをwに代入
    uv = mul(uv, matrixTex);

    output.tex = uv.xy;
    output.col = input.col;
	
    return output;
}
