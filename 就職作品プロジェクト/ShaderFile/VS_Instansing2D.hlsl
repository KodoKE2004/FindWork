#include "common.hlsl"

cbuffer InstanceBuffer : register(b6)
{
	float4 gPositionScale[1024];
}

PS_IN main(in VS_IN input)
{
	PS_IN output = (PS_IN) 0;

	uint indexBase = input.instanceID * 2;
	float4 instancePos = gPositionScale[indexBase + 0];
	float4 instanceScale = gPositionScale[indexBase + 1];

	float4 localPos = input.pos;
	localPos.xyz *= instanceScale.xyz;
	localPos.xyz += instancePos.xyz;

	matrix wvp;
	wvp = mul(World, View);
	wvp = mul(wvp, Projection);
	output.pos = mul(localPos, wvp);

	float4 uv = float4(input.tex.xy, 0.0f, 1.0f);
	uv = mul(uv, matrixTex);
	output.tex = uv.xy;

	output.col = input.col;

	return output;
}