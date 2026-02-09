#include "GameSceneDodge.h"
#include "Game.h"
#include "Calculator.h"
#include "DebugUI.h"
#include <random>
#include <array>
namespace
{
    constexpr float kStoneSpawnInterval = 0.5f;

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

void GameSceneDodge::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneJump";

    // 基底クラスの初期化
    GameSceneExe::SetBaseBeatCount(BASE_BEATS);
    GameSceneExe::Initialize();

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.previousScene = SCENE_NO::GAME_DODGE;
    m_RelationData.oldScene = SCENE_NO::GAME_WAIT;
    m_RelationData.isClear = true;

    auto& instance = Game::GetInstance();

    m_Bomber = instance.AddObject<Bomber>();
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber);

    m_Bird = instance.AddObject<Bird>();
    m_Bird->SetScale(50.0f,50.0f,1.0f);
    m_MySceneObjects.emplace_back(m_Bird);

    std::shared_ptr<Circle> circle = instance.AddObject<Circle>();
    circle->SetRadius(30.0f);
    circle->SetPos(0.0f, 0.0f, 0.0f);
    m_MySceneObjects.emplace_back(circle);

}

void GameSceneDodge::Update(float tick)
{
    GameSceneExe::Update(tick);
    auto& instance = Game::GetInstance();

    // 拍が更新された場合
    m_StoneSpawnElapsed += tick;
    while (m_StoneSpawnElapsed >= kStoneSpawnInterval)
    {
        m_StoneSpawnElapsed -= kStoneSpawnInterval;
        int createNum = 8 * (m_RelationData.stageCount / 6 + 1);
        for (int i = 0; i < createNum; ++i)
        {
            std::shared_ptr<Stone> stone = instance.AddObject<Stone>();
            m_StoneList.emplace_back(stone);
            m_MySceneObjects.emplace_back(stone);
        }
    }

    // 非アクティブな石を削除
    auto stoneList = instance.GetObjects<Stone>();
    for (auto stone : stoneList)
    {
        //Circle stoneCircle;
        //
        //stoneCircle.center = {stone->GetPos().x, stone->GetPos().y},
        //stoneCircle.radius = stone->GetScale().x * 0.5f,
        //
        //if (Calculator::Collider2D::isHitCircleCircle)

        if (stone->IsActive())
        {
            instance.DeleteObject(stone);
        }
    }




    if (IsChange()) 
    {
        ApplyBeatDuration(GameToWait, m_RelationData);
        ChangeScenePop(GameToWait);
    }



}

void GameSceneDodge::Finalize()
{
    GameSceneExe::Finalize();
}

