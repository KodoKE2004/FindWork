#include "common.hlsl"

PS_IN main(in VS_IN input)
{	
	PS_IN output = (PS_IN) 0;
	float4 worldPos = mul(input.pos, World);
	
	output.pos = mul(worldPos, mul(View, Projection));
	output.tex = input.tex;
	output.col = input.col;
    float4 n = input.nrm;
    n.w = 0.0f;
    output.nrm = normalize( mul( n, World).xyz); // ƒ[ƒ‹ƒh‹óŠÔ‚É•ÏŠ·‚µ‚Ä“n‚·
	return output;
	
}