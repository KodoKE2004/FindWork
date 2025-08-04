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


// シーン遷移の状態を表す変数
Scene*		SceneTrans::m_NextScene		= nullptr;	// 次のシーン
bool		SceneTrans::m_isChange		= false  ;	//
float		SceneTrans::m_Timer			= 0.0f	 ;	// タイマー
float		SceneTrans::m_Duration		= 1.0f	 ;	// 遷移時間
SWITCH		SceneTrans::m_isTransition  = OFF	 ;	// 遷移処理が完了したかどうか
TRANS_MODE	SceneTrans::m_TransMode		= FADE	 ;	// 遷移のパターン
Texture2D	SceneTrans::m_Texture(Game::GetInstance().GetCamera().get());

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

void SceneTrans::Initialize(TRANS_MODE mode)
{
	m_Timer = 0.0f;
	m_TransMode = mode;
	m_Texture.Initialize();
	m_Texture.SetTexture("01_AssetFile/Texture/Plane.png");
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
			m_Texture.SetColor(0.0f,0.0f,0.0f,m_Alpha);
		}
		
		
	}
	break;


	}
	if (m_TransMode == FINISH)
	{
		m_isTransition = OFF;
		Game::SetSceneCurrent(m_NextScene);
	}

}

void SceneTrans::Draw()
{
	if (m_isTransition == OFF)  { return; }
	m_Texture.Draw();

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

void SceneTrans::StartTransition(TRANS_MODE mode, Scene* nextScene, float duration)
{
	m_TransMode = mode;
	m_NextScene = nextScene;
	m_Duration = duration;
	m_Timer = 0.0f;
	m_isTransition = START;
}

