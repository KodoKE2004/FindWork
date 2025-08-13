#include	"TestPlane.h"
#include	"../Game.h"
#include	"../input.h"
using namespace DirectX::SimpleMath;

//=======================================
//初期化処理
//=======================================
void TestPlane::Init()
{
	// 頂点データ
	std::vector<VERTEX_3D>	vertices;

	vertices.resize(4);

	vertices[0].position = NVector3(-10.0f,  10.0f, -0.0f);
	vertices[1].position = NVector3( 10.0f,  10.0f, -0.0f);
	vertices[2].position = NVector3(-10.0f, -10.0f, -0.0f);
	vertices[3].position = NVector3( 10.0f, -10.0f, -0.0f);

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
	auto shaderMgr = Game::GetInstance().GetShaderManager();
	m_Shaders.emplace_back(shaderMgr->GetShader("VS_Default"));
	m_Shaders.emplace_back(shaderMgr->GetShader("PS_Default"));
	//テクスチャロード
	bool sts = m_Texture.LoadFromFile("01_AssetFile/Texture/background1.png");
	assert(sts == true);

}


//=======================================
//更新処理
//=======================================
void TestPlane::Update()
{
	if (Input::GetKeyPress(VK_W))
	{
		m_Scale.z += 0.1f; // 上に移動
	}
	if (Input::GetKeyPress(VK_S))
	{
		m_Scale.z -= 0.1f; // 下に移動
	}
}

//=======================================
//描画処理
//=======================================
void TestPlane::Draw()
{
	// SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll(
			m_Rotation.y, 
			m_Rotation.x, 
			m_Rotation.z);

	Matrix t = Matrix::CreateTranslation(
		m_Position.x, 
		m_Position.y, 
		m_Position.z);

	Matrix s = Matrix::CreateScale(
		m_Scale.x, 
		m_Scale.y, 
		m_Scale.z);

	Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	// 描画の処理
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// トポロジーをセット（プリミティブタイプ）
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(auto shader :m_Shaders) shader->SetGPU();
	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();

	m_Texture.SetGPU();

	devicecontext->DrawIndexed(
		4,							// 描画するインデックス数（四角形なんで４）
		0,							// 最初のインデックスバッファの位置
		0);
}

//=======================================
//終了処理
//=======================================
void TestPlane::Uninit()
{

}