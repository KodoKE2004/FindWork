#include	"Cube.h"
#include	"../Game.h"
#include	<SimpleMath.h>

using namespace DirectX::SimpleMath;

Cube::Cube(Camera* cam) : Object(cam)
{
	
}

//=======================================
//初期化処理
//=======================================
void Cube::Initialize()
{
	m_Position = NVector3(0.0f, 0.0f, 0.0f);
	m_Rotation = NVector3(0.0f, 0.0f, 0.0f);
	m_Scale    = NVector3(1.0f, 1.0f, 1.0f);

	// 頂点データ
	std::vector<VERTEX_3D>	vertices;

	vertices.resize(24);

	//上面
	vertices[0].position = NVector3(-10.0f,10.0f, 10.0f);
	vertices[1].position = NVector3( 10.0f,10.0f, 10.0f);
	vertices[2].position = NVector3(-10.0f,10.0f,-10.0f);
	vertices[3].position = NVector3( 10.0f,10.0f,-10.0f);

	vertices[0].color = DirectX::SimpleMath::Color( 1.0f, 1.0f, 0.0f, 1.0f);
	vertices[1].color = DirectX::SimpleMath::Color( 1.0f, 1.0f, 0.0f, 1.0f);
	vertices[2].color = DirectX::SimpleMath::Color( 1.0f, 1.0f, 0.0f, 1.0f);
	vertices[3].color = DirectX::SimpleMath::Color( 1.0f, 1.0f, 0.0f, 1.0f);

	vertices[0].uv = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
	vertices[1].uv = DirectX::SimpleMath::Vector2(0.33f, 0.0f);
	vertices[2].uv = DirectX::SimpleMath::Vector2(0.0f, 0.5f);
	vertices[3].uv = DirectX::SimpleMath::Vector2(0.33f, 0.5f);
	
	vertices[0].normal = NVector3( 0.0f, 1.0f, 0.0f);
	vertices[1].normal = NVector3( 0.0f, 1.0f, 0.0f);
	vertices[2].normal = NVector3( 0.0f, 1.0f, 0.0f);
	vertices[3].normal = NVector3( 0.0f, 1.0f, 0.0f);
	
	//正面
	vertices[4].position = NVector3(-10.0f, 10.0f, -10.0f);
	vertices[5].position = NVector3( 10.0f, 10.0f, -10.0f);
	vertices[6].position = NVector3(-10.0f,-10.0f, -10.0f);
	vertices[7].position = NVector3( 10.0f,-10.0f, -10.0f);

	vertices[4].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[5].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[6].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[7].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);

	vertices[4].uv = DirectX::SimpleMath::Vector2(0.33f, 0.5f);
	vertices[5].uv = DirectX::SimpleMath::Vector2(0.66f, 0.5f);
	vertices[6].uv = DirectX::SimpleMath::Vector2(0.33f, 1.0f);
	vertices[7].uv = DirectX::SimpleMath::Vector2(0.66f, 1.0f);

	vertices[4].normal = NVector3( 0.0f, 0.0f,-1.0f);
	vertices[5].normal = NVector3( 0.0f, 0.0f,-1.0f);
	vertices[6].normal = NVector3( 0.0f, 0.0f,-1.0f);
	vertices[7].normal = NVector3( 0.0f, 0.0f,-1.0f);

	//右面
	vertices[8].position  = NVector3( 10.0f, 10.0f, -10.0f);
	vertices[9].position  = NVector3( 10.0f, 10.0f,  10.0f);
	vertices[10].position = NVector3( 10.0f,-10.0f, -10.0f);
	vertices[11].position = NVector3( 10.0f,-10.0f,  10.0f);

	vertices[8]. color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[9].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[10].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[11].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);

	vertices[8].uv  = DirectX::SimpleMath::Vector2(0.66f, 0.0f);
	vertices[9].uv = DirectX::SimpleMath::Vector2(1.0f, 0.0f);
	vertices[10].uv = DirectX::SimpleMath::Vector2(0.66f, 0.5f);
	vertices[11].uv = DirectX::SimpleMath::Vector2(1.0f , 0.5f);

	vertices[8].normal  = NVector3( 1.0f, 0.0f, 0.0f);
	vertices[9].normal  = NVector3( 1.0f, 0.0f, 0.0f);
	vertices[10].normal = NVector3( 1.0f, 0.0f, 0.0f);
	vertices[11].normal = NVector3( 1.0f, 0.0f, 0.0f);

	//背面
	vertices[12].position  = NVector3(-10.0f, 10.0f,  10.0f);
	vertices[13].position  = NVector3( 10.0f, 10.0f,  10.0f);
	vertices[14].position  = NVector3(-10.0f,-10.0f,  10.0f);
	vertices[15].position  = NVector3( 10.0f,-10.0f,  10.0f);

	vertices[12].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[13].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[14].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[15].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);

	vertices[12].uv = DirectX::SimpleMath::Vector2(0.33f, 0.0f);
	vertices[13].uv = DirectX::SimpleMath::Vector2(0.66f, 0.0f);
	vertices[14].uv = DirectX::SimpleMath::Vector2(0.33f, 0.5f);
	vertices[15].uv = DirectX::SimpleMath::Vector2(0.66f, 0.5f);

	vertices[12].normal  = NVector3( 0.0f, 0.0f, 1.0f);
	vertices[13].normal  = NVector3( 0.0f, 0.0f, 1.0f);
	vertices[14].normal  = NVector3( 0.0f, 0.0f, 1.0f);
	vertices[15].normal  = NVector3( 0.0f, 0.0f, 1.0f);

	//左面
	vertices[16].position  = NVector3(- 10.0f,  10.0f, - 10.0f);
	vertices[17].position  = NVector3(- 10.0f,  10.0f,   10.0f);
	vertices[18].position  = NVector3(- 10.0f,- 10.0f, - 10.0f);
	vertices[19].position  = NVector3(- 10.0f,- 10.0f,   10.0f);

	vertices[16]. color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[17].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[18].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[19].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);

	vertices[16].uv  = DirectX::SimpleMath::Vector2(0.0f, 0.5f);
	vertices[17].uv = DirectX::SimpleMath::Vector2(0.33f, 0.5f);
	vertices[18].uv = DirectX::SimpleMath::Vector2(0.0f, 1.0f);
	vertices[19].uv = DirectX::SimpleMath::Vector2(0.33f, 1.0f);

	vertices[16].normal  = NVector3(-1.0f, 0.0f, 0.0f);
	vertices[17].normal  = NVector3(-1.0f, 0.0f, 0.0f);
	vertices[18].normal  = NVector3(-1.0f, 0.0f, 0.0f);
	vertices[19].normal  = NVector3(-1.0f, 0.0f, 0.0f);

	//底面
	vertices[20].position  = NVector3( 10.0f,-10.0f,  10.0f);
	vertices[21].position  = NVector3(-10.0f,-10.0f,  10.0f);
	vertices[22].position  = NVector3( 10.0f,-10.0f, -10.0f);
	vertices[23].position  = NVector3(-10.0f,-10.0f, -10.0f);

	vertices[20].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[21].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[22].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);
	vertices[23].color = DirectX::SimpleMath::Color(1.0f, 1.0f, 0.0f, 1.0f);

	vertices[20].uv = DirectX::SimpleMath::Vector2(0.66f, 0.5f);
	vertices[21].uv = DirectX::SimpleMath::Vector2(1.0f, 0.5f);
	vertices[22].uv = DirectX::SimpleMath::Vector2(0.66f, 1.0f);
	vertices[23].uv = DirectX::SimpleMath::Vector2(1.0f , 1.0f);

	vertices[20].normal  = NVector3( 0.0f,-1.0f, 0.0f);
	vertices[21].normal  = NVector3( 0.0f,-1.0f, 0.0f);
	vertices[22].normal  = NVector3( 0.0f,-1.0f, 0.0f);
	vertices[23].normal  = NVector3( 0.0f,-1.0f, 0.0f);

	// 頂点バッファ生成
	m_VertexBuffer.Create(vertices);

	// インデックスバッファ生成
	std::vector<unsigned int> indices;
	indices.resize(36);



	indices = { 
		0,1,2,
		1,3,2,

		4,5,6,
		5,7,6,
		
		8,9,10,
		9,11,10,

		14,15,12,
		15,13,12,
		
		17,16,18,
		18,19,17,

		23,22,21,
		22,20,21
	};
	
	// インデックスバッファ生成
	m_IndexBuffer.Create(indices);

	// シェーダオブジェクト生成
	m_Shader.Create("02_ResourceFile/ShaderFile/VS_Default.hlsl", "02_ResourceFile/ShaderFile/PS_Default.hlsl");

	//テクスチャロード
	m_Texture = *Game::GetInstance().GetTextureManager()->GetTexture("dice.png");

	m_Camera->SetCamera(CAMERA_3D);
}


//=======================================
//更新処理
//=======================================
void Cube::Update()
{

	/*if (Input::GetKeyPress(VK_D))
	{
		m_Position.x += 0.1f;
	}
	if (Input::GetKeyPress(VK_A))
	{
		m_Position.x -= 0.1f;
	}*/

}

//=======================================
//描画処理
//=======================================
void Cube::Draw()
{
	// SRT情報作成
	DirectX::SimpleMath::Matrix r = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	DirectX::SimpleMath::Matrix t = DirectX::SimpleMath::Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	DirectX::SimpleMath::Matrix s = DirectX::SimpleMath::Matrix::CreateScale			 ( m_Scale.x   , m_Scale.y   , m_Scale.z);

	DirectX::SimpleMath::Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	// 描画の処理
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// トポロジーをセット（プリミティブタイプ）
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_Shader.SetGPU();
	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();
	m_Texture.SetGPU();
	m_Camera->SetCamera(CAMERA_3D);

	devicecontext->DrawIndexed(
		36,							// 描画するインデックス数（四角形なんで４）
		0,							// 最初のインデックスバッファの位置
		0);
}

//=======================================
//終了処理
//=======================================
void Cube::Finalize()
{

}