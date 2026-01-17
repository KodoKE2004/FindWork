#include "GameSceneJump.h"
#include "Game.h"
#include "Calculator.h"
#include "DebugUI.h"
#include <random>
#include <array>
namespace
{
    float StartPosX[10] = {
          0.0f,
        200.0f,
        400.0f,
        };
    std::array<size_t, 3> ShuffleButtonIndices()
    {
        static std::mt19937 engine{ std::random_device{}() };
        std::array<size_t, 3> indices{ 0, 1, 2 };
        std::shuffle(indices.begin(), indices.end(), engine);
        return indices;
    }
}

void GameSceneJump::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneJump";

    // 基底クラスの初期化
    GameSceneExe::SetBaseBeatCount(BASE_BEATS);
    GameSceneExe::Initialize();

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.previousScene = SCENE_NO::GAME_JUMP;
    m_RelationData.oldScene = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear = true;

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    m_Bomber = instance.AddObject<Bomber>();
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber);

    m_Bird = instance.AddObject<Bird>();
    m_Bird->SetScale(90.0f,90.0f,1.0f);
    m_MySceneObjects.emplace_back(m_Bird);

    m_Beat = m_BeatTimer.GetRestBeats();
}

void GameSceneJump::Update(float tick)
{
    GameSceneExe::Update(tick);
    // 拍が更新された場合
    int restBeat = m_BeatTimer.GetRestBeats();
    if (m_Beat - restBeat > 2)
    {
        auto& instance = Game::GetInstance();
        m_Beat = restBeat;
        int createNum = 2 * (m_RelationData.stageCount / 8 + 1);
        for (int i = 0; i <= createNum; ++i)
        {
            std::shared_ptr<Stone> stone = instance.AddObject<Stone>();
            m_StoneList.emplace_back(stone);
            m_MySceneObjects.emplace_back(stone);
        }
    }

    for (auto stone = m_StoneList.begin(); stone != m_StoneList.end(); )
    {
        if (stone->get()->IsActive())
        {
            stone = m_StoneList.erase(stone);
        }
        else 
        {
            ++stone;
        }

    }


    if (IsChange()) 
    {
        ApplyBeatDuration(GameToWait, m_RelationData);
        ChangeScenePop(GameToWait);
    }



}

void GameSceneJump::Finalize()
{
    GameSceneExe::Finalize();
}

