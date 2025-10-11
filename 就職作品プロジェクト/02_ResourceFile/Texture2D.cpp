#include "Texture2D.h"
#include "Game.h"

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
Texture2D::Texture2D(Camera* cam) :Object(cam)
{

}

// デストラクタ
Texture2D::~Texture2D()
{

}

//=======================================
// 初期化処理
//=======================================
void Texture2D::Initialize()
{
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

	m_VertexBuffer.Create(vertices);

	std::vector<unsigned int> indices;
	indices.resize(4);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	m_IndexBuffer.Create(indices);

	SetShader("VS_Default", "PS_Default");

	m_Materiale = std::make_unique<Material>();
	MATERIAL mtrl;
	mtrl.Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Shiness = 1;
	mtrl.TextureEnable = true;
	m_Materiale->Create(mtrl);
}

//=======================================
// 更新処理
//=======================================
void Texture2D::Update()
{

}

//=======================================
// 描画処理
//=======================================
void Texture2D::Draw()
{
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
}

//=======================================
// 終了処理
//=======================================
void Texture2D::Finalize()
{

}

// テクスチャを指定
void Texture2D::SetTexture(const char* imgname)
{
	// テクスチャロード
	bool sts = m_Texture->LoadFromFile(imgname);
	assert(sts != true);
}

void Texture2D::SetTexture(std::shared_ptr<Texture> texture)
{
	m_Texture = texture;
}

// UV座標を指定
void Texture2D::SetUV(const float& nu, const float& nv, const float& sx, const float& sy)
{
	m_NumU = nu;
	m_NumV = nv;
	m_SplitX = sx;
	m_SplitY = sy;
}