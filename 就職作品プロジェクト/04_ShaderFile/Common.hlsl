struct VS_IN
{
    float4 pos : POSITION0;
    float4 nrm : NORMAL0;
    float4 col : COLOR0;
    float2 tex : TEXCOORD0;
#ifdef USE_INSTANCING
    float4 world0 : INSTANCEWORLD0;
    float4 world1 : INSTANCEWORLD1;
    float4 world2 : INSTANCEWORLD2;
    float4 world3 : INSTANCEWORLD3;
    float4 normal0 : INSTANCENORMAL0;
    float4 normal1 : INSTANCENORMAL1;
    float4 normal2 : INSTANCENORMAL2;
    float4 normal3 : INSTANCENORMAL3;
    float4 tint : INSTANCECOLOR0;
    float4 uvRect : INSTANCEUV0;
#endif
};

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 tex : TEXCOORD0;
	float3 nrm : TEXCOORD1;
};

struct MATERIAL
{
	float4 Ambient;     // 環境光
	float4 Diffuse;     // 拡散反射光
	float4 Specular;    // 鏡面反射光
	float4 Emission;    // 自己発光
	float Shininess;    // 鏡面反射の鋭さ
	bool TextureEnable; // テクスチャが有効の確認
	bool2 Dummy;        
};

struct LightBuffer
{
	float4 gLightDirection;
	float4 gLightColor;
	float  gAmbientI;
	float  gDiffuseI;
	float  gSpecularI;
	float _pad;
};


cbuffer WorldBuffer : register(b0)
{
    matrix World;
};
cbuffer ViewBuffer : register(b1)
{
    matrix View;
};
cbuffer ProjectionBuffer : register(b2)
{
    matrix Projection;
};
cbuffer LightBuffer : register(b3)
{
	LightBuffer gLight;
};
cbuffer MeterialBuffer : register(b4)
{
    MATERIAL Material;
};
cbuffer TextureBuffer : register(b5)
{
    float4x4 matrixTex;
};


float4x4 GetWorld(VS_IN input)
{
#ifdef USE_INSTANCING
    return float4x4(input.world0, input.world1, input.world2, input.world3);
#else
    return World;
#endif
}
float4 GetColor(VS_IN input)
{
#ifdef USE_INSTANCING
    return input.col * input.tint;
#else
    return input.col;
#endif
}
float2 GetUV(VS_IN input)
{
#ifdef USE_INSTANCING
    return input.tex * input.uvRect.zw + input.uvRect.xy;
#else
    return input.tex;
#endif
}
float3 GetNormal(VS_IN input)
{
#ifdef USE_INSTANCING
    float4x4 normalMatrix = float4x4(input.normal0, input.normal1, input.normal2, input.normal3);
#else
    float4x4 normalMatrix = World;
#endif
    float3 n = mul(float4(input.nrm.xyz, 0), normalMatrix).xyz;
    return n * rsqrt(max(dot(n, n), 1e-20f));
}
