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

void SceneTrans::Initialize(TRANS_MODE mode)
{
	m_Timer = 0.0f;
	m_TransMode = mode;
	m_Texture = new Texture2D(Game::GetInstance().GetCamera());
	m_Texture->Initialize();
	m_Texture->SetTexture("01_AssetFile/Texture/Plane.png");
}

void SceneTrans::Update()
{
	if(m_isTransition == OFF)  { return; }

	// ’lXV
	switch (m_TransMode)
	{
	case FADE:
	{
		if (IsTransition() == START)
		{
			FadeInit();
		}
		else if (IsTransition() == DOING)
		{
			if (m_isChange) 
			{
				FadeIN();
			}
			else
			{
				FadeOUT();
			}
			m_Texture->SetColor(0.0f,0.0f,0.0f,m_Alpha);
		}
		
		
	}
	break;


	}
	if (IsTransition() == FINISH)
	{
		SetTransition(OFF);
		Game::SetSceneCurrent(m_NextScene);
	}

}

void SceneTrans::Draw()
{
	if (IsTransition() == OFF) { return; }
	m_Texture->Draw();

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
	m_isChange = false;
	m_isTransition = START;
}

void SceneTrans::FadeInit()
{
	m_Alpha = 0.0f;
	m_isTransition = DOING;
}

void SceneTrans::FadeIN()
{
	m_Timer += Application::GetDeltaTime();
	m_Alpha = std::clamp(m_Timer / m_Duration, 0.0f, 1.0f);

	if (m_Alpha >= 1.0f)
	{
		m_Alpha = 1.0f;
		m_isChange = true;
		m_Timer = 0.0f;
	}
}

void SceneTrans::FadeOUT()
{
	m_Timer += Application::GetDeltaTime();
	m_Alpha = std::clamp(1.0f - (m_Timer / m_Duration), 0.0f, 1.0f);

	if (m_Alpha <= 0.0f)
	{
		m_Alpha = 0.0f;
		m_isTransition = FINISH;
	}
}

