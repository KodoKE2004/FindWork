#include "Fade.h"
#include "../../Game.h"
#include "../../main.h"

Fade::Fade(Camera* cam) : Texture2D(cam)
{

}

void Fade::Initialize()
{
	// フェード用の画像追加
	m_Texture = Game::GetInstance().GetTextureManager()->GetTexture("Black.png");
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
	auto shaderMgr = Game::GetInstance().GetShaderManager();
	m_Shaders.emplace_back(shaderMgr->GetShader("VS_Default"));
	m_Shaders.emplace_back(shaderMgr->GetShader("PS_Default"));

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
void Fade::Finalize()
{

}