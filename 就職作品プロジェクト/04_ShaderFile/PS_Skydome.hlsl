Texture2D gSkyTex : register(t0); // SRGBで作られたSRVをバインド
SamplerState gSamp : register(s0);

struct PSIn
{
	float4 svpos : SV_POSITION;
	float2 uv : TEXCOORD0;
};

float4 main(PSIn i) : SV_TARGET
{
    // そのまま出す。ガンマはSRV/RTV設定に任せる。
	return gSkyTex.Sample(gSamp, i.uv);
}
