#include "Common.hlsl"

cbuffer GridParams : register(b6)
{
	float GridStart;
	float GridSpacing;
	float GridAxis;
	float GridPadding;
};

PS_IN main(VS_IN input, uint instanceId : SV_InstanceID)
{
	PS_IN output = (PS_IN) 0;

	float offset = GridStart + GridSpacing * instanceId;

	float4 vertex = input.pos;
	if (GridAxis < 0.5f)
	{
		vertex.z += offset;
	}
	else
	{
		vertex.x += offset;
	}

	float4 worldPos = mul(vertex, World);
	float4 viewPos = mul(worldPos, View);
	output.pos = mul(viewPos, Projection);
	output.tex = input.tex;

	float3 baseColor = input.col.rgb;
	if (abs(offset) < 1.0e-4f)
	{
		baseColor = (GridAxis < 0.5f) ? float3(0.25f, 0.45f, 1.0f) : float3(1.0f, 0.25f, 0.25f);
	}
	else
	{
		const float majorInterval = 5.0f;
		float normalized = offset / majorInterval;
		float deviation = abs(normalized - round(normalized));
		if (deviation < 1.0e-3f)
		{
			baseColor *= 1.4f;
		}
	}

	output.col = float4(saturate(baseColor), input.col.a);

	float4 n = input.nrm;
	n.w = 0.0f;
	output.nrm = normalize(mul(n, World).xyz);

	return output;
}