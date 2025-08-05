#include "TransScene.h"
#include "../main.h"
#include "../Game.h"

void TransScene::Initialize()
{
	m_Timer		= 0.0f;
	m_Alpha		= 0.0f;
	m_isChange	= false;
	m_Step		= STEP::DOING;

	switch (m_TransMode)
	{
	case TRANS_MODE::FADE:
	{
		m_AlphaValue = 1.0f / m_Duration;
		m_Fade = Game::GetInstance().AddObject<Fade>();
		m_Fade->Initialize();
		m_MySceneObjects.emplace_back(m_Fade);
	}
	break;
	}

}

void TransScene::Update()
{
	switch (m_Step)
	{
	case START:
	{
		Initialize();
	}
	break;
	case DOING:
	{
		m_Timer += m_Counter;

		// OUT‚Ìˆ—
		if (!m_isChange) {
			switch (m_TransMode) 
			{
			case FADE:	FADE_OUT();		break;

			}
			if (m_isChange) {
				m_SceneOld->Finalize();
				m_SceneNext->Initialize();
			}
		}
		// IN‚Ìˆ—
		else
		{
			switch (m_TransMode)
			{
			case FADE:	FADE_IN();		break;

			}
		}


	}
	break;
	case FINISH:
	{
		Finalize();
		Game::GetInstance().SetSceneCurrent(m_SceneNext);
	}
	break;

	}


}

void TransScene::Finalize()
{
	m_Step = OFF;
	for (auto obj : m_MySceneObjects)
	{
		Game::GetInstance().DeleteObject(obj);
	}
	m_MySceneObjects.clear();
}

bool TransScene::isOverClock()
{
	if (m_Timer >= m_Duration)
	{
		m_Timer = 0.0f;
		return true;
	}

	return false;
}

void TransScene::FADE_IN()
{
	m_Alpha -= m_AlphaValue * m_Counter;

	if (isOverClock())
	{
		m_Alpha = 0.0f;
		m_Step = FINISH;
	}
	auto fade = Game::GetInstance().GetObjects<Fade>();
	fade[0]->SetColor(0.0f, 0.0f, 0.0f, m_Alpha);
	
}

void TransScene::FADE_OUT()
{
	m_Alpha += m_AlphaValue * m_Counter;

	if (isOverClock())
	{
		m_Alpha = 1.0f;
		m_isChange = true;
	}

	auto fade = Game::GetInstance().GetObjects<Fade>();
	fade[0]->SetColor(0.0f, 0.0f, 0.0f, m_Alpha);
}