#include "common.hlsl"

PS_IN main(in VS_IN input)
{
    PS_IN output;
	
	// 座標変換
    matrix wvp;
    wvp = mul(World, View); // ワールド ⇒ ビュー
    wvp = mul(wvp, Projection); // ビュー　 ⇒ プロジェクション
    output.pos = mul(input.pos, wvp); // 最終的にoutput.posに代入しラスタライザに渡される

    float4 uv;
    uv.xy = input.tex;
    uv.z = 0.0f;
    uv.w = input.tex;
    uv = mul(uv, matrixTex);

    output.tex = uv.xy;
    output.col = input.col;
	
    return output;
}
