#include "Light.h"

Light::Light(Camera* cam) : Cube(cam)
{
}

void Light::Initialize()
{
	Cube::Initialize();
	SetPos(10.0f,0.0f,0.0f);
	SetScale(0.5f, 0.5f, 0.5f);
	SetColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Texture = nullptr;
}

void Light::Update()
{
	
	// ライトの位置をオブジェクトの位置に設定
	
	// ライトの方向を下向きに設定
	SetDirection(DirectX::SimpleMath::Vector4(0.0f, -1.0f, 0.0f, 0.0f));
	
	// ライトの色を白色に設定
	SetLightColor(DirectX::SimpleMath::Color(1.0f, 1.0f, 1.0f, 1.0f));
	
	// 環境光、拡散光、鏡面反射光の強度を設定
	SetAmbient(0.1f);
	SetDiffuse(1.0f);
	SetSpecular(0.5f);
	
	NVector3 pos = GetPos();

	if (Input::GetKeyPress(VK_W)) {
		pos.x += 1.0f;
	}
	if (Input::GetKeyPress(VK_S)) {
		pos.x -= 1.0f;
	}
	if (Input::GetKeyPress(VK_A)) {
		pos.z += 1.0f;
	}
	if (Input::GetKeyPress(VK_D)) {
		pos.z -= 1.0f;
	}

	SetPos(pos); // 新しい位置を設定


	Cube::Update(); // 親クラスの更新処理を呼び出す
}

void Light::Draw()
{
	// SRT情報作成
	DirectX::SimpleMath::Matrix r = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	DirectX::SimpleMath::Matrix t = DirectX::SimpleMath::Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	DirectX::SimpleMath::Matrix s = DirectX::SimpleMath::Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	DirectX::SimpleMath::Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	// 描画の処理
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// トポロジーをセット（プリミティブタイプ）
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (auto shader : m_Shaders)
	{
		shader->SetGPU();
	}

	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();
	if (m_Texture != nullptr) { m_Texture->SetGPU(); }
	m_Camera->SetCamera(CAMERA_3D);

	Renderer::SetLightBuffer(&m_LightBuffer); // ライトバッファをセット
	devicecontext->DrawIndexed(
		36,							// 描画するインデックス数（四角形なんで４）
		0,							// 最初のインデックスバッファの位置
		0);
}