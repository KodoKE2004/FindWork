#include "Common.hlsl"
struct VSOut
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD0;
};
VSOut main(VS_IN input)
{
    VSOut output;
    output.svpos = mul(mul(mul(input.pos, GetWorld(input)), View), Projection);
    output.uv = GetUV(input);
    return output;
}
