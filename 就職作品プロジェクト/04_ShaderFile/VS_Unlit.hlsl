#include "common.hlsl"

PS_IN main(in VS_IN input)
{	
	PS_IN output = (PS_IN) 0;
	float4 worldPos = mul(input.pos, GetWorld(input));
	
	output.pos = mul(worldPos, mul(View, Projection));
	output.tex = GetUV(input);
	output.col = GetColor(input);
    float4 n = input.nrm;
    n.w = 0.0f;
    output.nrm = GetNormal(input);
	return output;
	
}