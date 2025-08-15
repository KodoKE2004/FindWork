#include "Light.h"

Light::Light(Camera* cam) : Cube(cam)
{
}

void Light::Initialize()
{
	Cube::Initialize();
	SetScale(0.5f, 0.5f, 0.5f);
	SetColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Light::Update()
{
	
	// ライトの位置をオブジェクトの位置に設定
	SetLightPosition(DirectX::SimpleMath::Vector3(GetPos().x,GetPos().y,GetPos().z));
	
	// ライトの方向を下向きに設定
	SetLightDirection(DirectX::SimpleMath::Vector4(0.0f, -1.0f, 0.0f, 0.0f));
	
	// ライトの色を白色に設定
	SetLightColor(DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
	
	// 環境光、拡散光、鏡面反射光の強度を設定
	SetAmbient(0.1f);
	SetDiffuse(1.0f);
	SetSpecular(0.5f);
	
	Cube::Update(); // 親クラスの更新処理を呼び出す
}

void Light::Draw()
{
	Renderer::SetLightBuffer(&m_LightBuffer); // ライトバッファをセット
	Cube::Draw(); // 親クラスの描画処理を呼び出す
}