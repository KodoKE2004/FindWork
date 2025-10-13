#include "Fade.h"
#include "../../Game.h"
#include "../../main.h"
#include <vector>

Fade::Fade(Camera* cam) : TransitionBase(cam)
{

}

void Fade::Initialize()
{
	// フェード用の画像追加
    auto textureMgr = Game::GetInstance().GetTextureManager();
	m_Texture = textureMgr->GetTexture("Black.png");
	SetScale(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
	SetColor(0.0f, 0.0f, 0.0f, 0.0f);

	// 頂点データ
	std::vector<VERTEX_3D> vertices;

	vertices.resize(4);

	vertices[0].position = NVector3(-0.5f, 0.5f, 0.0f);
	vertices[1].position = NVector3(0.5f, 0.5f, 0.0f);
	vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
	vertices[3].position = NVector3(0.5f, -0.5f, 0.0f);

	vertices[0].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[1].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[2].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[3].color = Color(1.0f, 1.0f, 1.0f, 1.0f);

	vertices[0].uv = Vector2(0.0f, 0.0f);
	vertices[1].uv = Vector2(1.0f, 0.0f);
	vertices[2].uv = Vector2(0.0f, 1.0f);
	vertices[3].uv = Vector2(1.0f, 1.0f);

	// 頂点バッファ生成
	m_VertexBuffer.Create(vertices);

	// インデックスバッファ生成
	std::vector<unsigned int> indices;
	indices.resize(4);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	// インデックスバッファ生成
	m_IndexBuffer.Create(indices);

	// シェーダオブジェクト生成
	SetShader("VS_Alpha", "PS_Alpha");

	// マテリアル情報取得
	m_Materiale = std::make_unique<Material>();
	MATERIAL mtrl;
	mtrl.Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Shiness = 1;
	mtrl.TextureEnable = true; // テクスチャを使うか否かのフラグ
	m_Materiale->Create(mtrl);

}
void Fade::Update()
{
	
}
void Fade::Draw()
{
	Renderer ::SetDepthEnable(false);
	Renderer ::SetBlendState(BS_ALPHABLEND);
	Renderer::SetBlendState(BS_ALPHABLEND);

	// SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	// 描画の処理
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();


	// トポロジーをセット（プリミティブタイプ）
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	SetGPU();

	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();

	m_Texture->SetGPU();

	m_Materiale->SetDiffuse(DirectX::XMFLOAT4(m_Color.x, m_Color.y, m_Color.z, m_Color.w));
	m_Materiale->Update();
	m_Materiale->SetGPU();

	// UVの設定を指定
	float u = m_NumU - 1;
	float v = m_NumV - 1;
	float uw = 1 / m_SplitX;
	float vh = 1 / m_SplitY;

	Renderer::SetUV(u, v, uw, vh);
	Camera::ScopedMode scepedMode(m_Camera, CAMERA_2D);

	devicecontext->DrawIndexed(
		4, // 描画するインデックス数（四角形なんで４）
		0, // 最初のインデックスバッファの位置
		0);

	Renderer ::SetDepthEnable(true);  // 描画後に元へ戻す
}
void Fade::Finalize()
{
	
}