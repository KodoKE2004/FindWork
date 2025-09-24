struct VS_IN
{
    float4 pos : POSITION0;
    float4 nrm : NORMAL0;
    float4 col : COLOR0;
    float2 tex : TEXCOORD0;
	uint instanceID : TEXCOORD1;
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
