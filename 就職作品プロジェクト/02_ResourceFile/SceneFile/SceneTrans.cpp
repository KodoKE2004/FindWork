#include "SceneTrans.h"
#include "../Renderer.h"
#include "../Application.h"
#include "../Shader.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Material.h"
#include "../Game.h"
#include <vector>
#include <algorithm>

using namespace DirectX::SimpleMath;

namespace {
	bool s_IsInit = false;
	VertexBuffer s_Vertex;
	IndexBuffer  s_Index;
	Shader       s_Shader;
	std::unique_ptr<Material> s_Material;
}

// シーン遷移の状態を表す変数
Scene*		SceneTrans::m_NextScene		= nullptr;	// 次のシーン
bool		SceneTrans::m_isChange		= false  ;	//
float		SceneTrans::m_Timer			= 0.0f	 ;	// タイマー
float		SceneTrans::m_Duration		= 1.0f	 ;	// 遷移時間
SWITCH		SceneTrans::m_isTransition  = OFF	 ;	// 遷移処理が完了したかどうか
TRANS_MODE	SceneTrans::m_TransMode		= FADE	 ;	// 遷移のパターン

// シーン遷移演出の値を表す変数
float SceneTrans::m_Alpha = 0.0f;
float SceneTrans::m_Delta = 0.0f;

namespace TRANS
{
	float m_Timer;
	float m_Duration;
	float m_Alpha = 0.0f;
	std::vector<VERTEX_3D> vertices(4);

	// フェード処理初期化
	void FADE_INIT(float duration)
	{
		m_Timer = 0.0f;
		m_Duration = duration;
		m_Alpha = 0.0f;
		SceneTrans::SetAlpha(m_Alpha);
	}

	// フェードイン処理
	void FADE_IN()
	{
		m_Timer += Application::GetDeltaTime();
		float alpha = std::clamp(m_Timer / m_Duration, 0.0f, 1.0f);

		SceneTrans::SetAlpha(alpha);

		if (SceneTrans::GetAlpha() >= 1.0f)
		{
			SceneTrans::SetAlpha(1.0f);
			SceneTrans::SetChange(true);
			m_Timer = 0.0f;
		}
	}

	// フェードアウト処理
	void FADE_OUT()
	{
		m_Timer += Application::GetDeltaTime();
		float alpha = std::clamp(1.0f - (m_Timer / m_Duration), 0.0f, 1.0f);
		SceneTrans::SetAlpha(alpha);

		if (SceneTrans::GetAlpha() <= 0.0f)
		{
			SceneTrans::SetAlpha(0.0f);
			SceneTrans::SetTransition(FINISH);
		}
	}
}

static void CreateResources()
{
	if (s_IsInit) return;

	std::vector<VERTEX_3D> vertices(4);
	vertices[0].position = NVector3(-0.5f, 0.5f, 0.0f);
	vertices[1].position = NVector3(0.5f, 0.5f, 0.0f);
	vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
	vertices[3].position = NVector3(0.5f, -0.5f, 0.0f);
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = Color(1, 1, 1, 1);
	vertices[0].uv = Vector2(0, 0);
	vertices[1].uv = Vector2(1, 0);
	vertices[2].uv = Vector2(0, 1);
	vertices[3].uv = Vector2(1, 1);
	s_Vertex.Create(vertices);

	std::vector<unsigned int> indices = { 0,1,2,3 };
	s_Index.Create(indices);

	s_Shader.Create("02_ResourceFile/ShaderFile/VS_Default.hlsl",
					"02_ResourceFile/ShaderFile/PS_Default.hlsl");

	MATERIAL mtrl{};
	mtrl.Diffuse = Color(0, 0, 0, 1);
	mtrl.Ambient = Color(0, 0, 0, 1);
	mtrl.Specular = Color(0, 0, 0, 1);
	mtrl.Emission = Color(0, 0, 0, 1);
	mtrl.Shiness = 1.0f;
	mtrl.TextureEnable = FALSE;
	s_Material = std::make_unique<Material>();
	s_Material->Create(mtrl);

	s_IsInit = true;
}


void SceneTrans::Initialize(TRANS_MODE mode)
{
	m_Timer = 0.0f;

}

void SceneTrans::Update()
{
	if(m_isTransition == OFF)  { return; }

	// 値更新
	switch (m_TransMode)
	{
	case FADE:
	{
		if (IsTransition() == START)
		{
			TRANS::FADE_INIT(m_Duration);
		}
		else if (IsTransition() == DOING)
		{
			if (!m_isChange) 
			{
				TRANS::FADE_IN();
			}
			else
			{
				TRANS::FADE_OUT();
			}
		}
		
	}
	break;


	}

	if (IsTransition() == FINISH)
	{
		Game::SetSceneCurrent(m_NextScene);
	}
}

void SceneTrans::Draw()
{
	if (m_isTransition == OFF)  { return; }

	// 描画命令
	std::vector<VERTEX_3D> vertices(4);
	vertices[0].position = NVector3(-0.5f, 0.5f, 0.0f);
	vertices[1].position = NVector3(0.5f, 0.5f, 0.0f);
	vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
	vertices[3].position = NVector3(0.5f, -0.5f, 0.0f);
	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = Color(1, 1, 1, 1);
	vertices[0].uv = Vector2(0, 0);
	vertices[1].uv = Vector2(1, 0);
	vertices[2].uv = Vector2(0, 1);
	vertices[3].uv = Vector2(1, 1);
	s_Vertex.Create(vertices);

	std::vector<unsigned int> indices = { 0,1,2,3 };
	s_Index.Create(indices);

	s_Shader.Create("02_ResourceFile/ShaderFile/VS_Default.hlsl",
					"02_ResourceFile/ShaderFile/PS_Default.hlsl");

	MATERIAL mtrl{};
	mtrl.Diffuse  = Color(0, 0, 0, m_Alpha);
	mtrl.Ambient  = Color(0, 0, 0, m_Alpha);
	mtrl.Specular = Color(0, 0, 0, m_Alpha);
	mtrl.Emission = Color(0, 0, 0, m_Alpha);
	mtrl.Shiness  = 1.0f;
	mtrl.TextureEnable = FALSE;
	s_Material->SetMaterial(mtrl);

	Matrix s = Matrix::CreateScale((float)Application::GetWidth(), (float)Application::GetHeight(), 1.0f);
	Matrix t = Matrix::CreateTranslation((float)Application::GetWidth() * 0.5f, (float)Application::GetHeight() * 0.5f, 0.0f);
	Matrix world = s * t;
	Renderer::SetWorldMatrix(&world);

	ID3D11DeviceContext* dc = Renderer::GetDeviceContext();
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	s_Shader.SetGPU();
	s_Vertex.SetGPU();
	s_Index.SetGPU();
	s_Material->SetGPU();
	Renderer::SetUV(0.0f, 0.0f, 1.0f, 1.0f);
	dc->DrawIndexed(4, 0, 0);


}
void SceneTrans::SetAlpha(float alpha)
{
	m_Alpha = alpha;
}
void SceneTrans::SetChange(bool isChange)
{
	m_isChange = isChange;
}
void SceneTrans::SetTransition(SWITCH setSwitch)
{
	m_isTransition = setSwitch;
}

SWITCH SceneTrans::IsTransition()
{
	return m_isTransition;
}

