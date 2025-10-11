#include"SceneList.h"

#include "../Game.h"
#include "../DebugUI.h"
#include "../Texture2D.h"
#include "../ObjectFile/Model.h"
#include "../ObjectFile/Bullet.h"
#include "../Camera.h"

void SelectScene::Initialize()
{
	DebugUI::TEXT_CurrentScene = "SelectScene";

#ifdef _DEBUG
	GAME_INSTANCE.m_Grid.SetEnabled(true);
#endif

    auto* m_SkyDome = GAME_INSTANCE.AddObject<Sphere>();
	m_SkyDome->SetSkyDomeMode(true);
    m_SkyDome->SetTexture(GAME_MANAGER_TEXTURE->GetTexture("Plane.png"));
	m_SkyDome->SetRadius(500.0f);
	m_MySceneObjects.emplace_back(m_SkyDome);

}

void SelectScene::Update(float tick)
{
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		Game::GetInstance().GetAudioManager()->Play("enter");
		ChangeScene<GameScene>(TRANS_MODE::FADE, 1.0f);
	}

	if (isEnd)
	{
		ChangeScene<GameScene>(TRANS_MODE::FADE, 0.5f);
		return;
    }
	if (isStart)
	{
		const float move = 5.0f;

        //m_Time += tick;
		if (Input::GetKeyPress(VK_W)) GAME_INSTANCE.GetCamera()->Dolly(move);
		if (Input::GetKeyPress(VK_S)) GAME_INSTANCE.GetCamera()->Dolly(-move);
		if (Input::GetKeyPress(VK_A)) GAME_INSTANCE.GetCamera()->Track(-move, 0.0f);
		if (Input::GetKeyPress(VK_D)) GAME_INSTANCE.GetCamera()->Track(move, 0.0f);

		if (Input::GetKeyTrigger(VK_L) || Input::GetKeyTrigger(VK_J))
		{
			auto pos = GAME_INSTANCE.GetCamera()->GetPosition();
			auto dir = GAME_INSTANCE.GetCamera()->GetForward();
            auto* m_Bullet = GAME_INSTANCE.AddObject<Bullet>();
			m_Bullet->Shoot(pos, dir);
            m_MySceneObjects.emplace_back(m_Bullet);
            m_ShotCount++;
		}
		if (m_Time >= m_ChangeTime)
		{
            isEnd = true;
		}
	}
	else
	{
        m_ReadyTime -= tick;
		if (m_ReadyTime <= 0.0f)
		{
            isStart = true;
		}
	}
	

}

void SelectScene::Finalize()
{
#ifdef _DEBUG
	GAME_INSTANCE.m_Grid.SetEnabled(false);
#endif
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		Game::GetInstance().DeleteObject(o);
	}
	m_MySceneObjects.clear();
}
