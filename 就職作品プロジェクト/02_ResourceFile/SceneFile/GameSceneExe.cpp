#include "GameSceneExe.h"
#include "SceneList.h"
#include "input.h"
#include "DebugUI.h"
#include "Game.h"
#include "ObjectFile/Model.h"
#include "ObjectFile/Skydome.h"

void GameSceneExe::Initialize()
{
    // スピード依存でチェンジタイムの変更
    m_ChangeSceneTime *= (1.0f / m_GameSpeedMass);


}

void GameSceneExe::Update(float tick)
{
    TickCount(tick);
    if (IsTimeUp()) {
        m_isChange = true;
    }
    if (IsChange()) {
        ChangeScenePop(TRANS_MODE::FADE, 0.2f);
    }
}

void GameSceneExe::Finalize()
{
}
