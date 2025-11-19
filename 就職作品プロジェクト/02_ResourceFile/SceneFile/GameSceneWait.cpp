#include "GameSceneWait.h"
#include "Game.h"
#include "DebugUI.h"
#include "SceneList.h"

#include <array>
#include <random>

template<class T>
T RandomChoose(const T& a, const T& b)
{
    static std::mt19937 mt{std::random_device{}() };
    unsigned int bit = mt() & 1u;

    // bit が 0 のとき a を、1 のとき b を返す
    return (bit == 0) ? a : b;

}


namespace
{
    constexpr std::array<SCENE_NO, 3> kStaegeCandidates = {
        SCENE_NO::GAME_SLICE,
        SCENE_NO::GAME_PUSH,
        SCENE_NO::GAME_HIT
    };

    constexpr float kStageTransitionDelay = 1.0f;
}

bool GameSceneWait::s_HasFirstGameSceneWaitInitialized = false;

void GameSceneWait::PrepareNextStage()
{
    // ステージのインデックスを格納
    SCENE_NO _NextScene = SCENE_NO::NONE;

    // 乱数の作成
    // 初回初期化時は前回の連続するので要素の削除
    // 全3ステージからランダム選択
    if (m_IsFirstInitialized)
    {
        _NextScene = StageSelectAllRandom();
    }
    // oldを見てから2パターンの選択
    else
    {   
        switch (m_RelationData.oldScene)
        {
        case SCENE_NO::GAME_SLICE:
            _NextScene = RandomChoose<SCENE_NO>(SCENE_NO::GAME_PUSH,
                                               SCENE_NO::GAME_HIT);
        break;
        case SCENE_NO::GAME_PUSH:
            _NextScene = RandomChoose<SCENE_NO>(SCENE_NO::GAME_SLICE,
                                               SCENE_NO::GAME_HIT);
        break;
        case SCENE_NO::GAME_HIT:
            _NextScene = RandomChoose<SCENE_NO>(SCENE_NO::GAME_PUSH,
                                               SCENE_NO::GAME_SLICE);
        break;
        default:
            _NextScene = StageSelectAllRandom();
        break;
        }
    }

    // 次のシーンRelationDataに格納
    m_RelationData.nextScene = _NextScene;
    // 次シーンの選択完了
    m_ShouldTransitionToStage = true;
}

SCENE_NO GameSceneWait::StageSelectAllRandom()
{
    SCENE_NO stageKinds[3] = {
            SCENE_NO::GAME_SLICE,
            SCENE_NO::GAME_HIT,
            SCENE_NO::GAME_PUSH
    };

    // 最初の一回だけ作成する乱数は３パターンの中から選択
    std::uniform_int_distribution<int> dist(0, 2);
    int selectedIdx = dist(m_RandomEngine);
    return stageKinds[selectedIdx];
}

void GameSceneWait::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneWait";

    // 引き渡しデータのシーンの整理
    m_RelationData.oldScene = m_RelationData.previousScene;
    m_RelationData.previousScene = SCENE_NO::GAME_WAIT;

    if (m_RelationData.isClear) {
        MyDebugLog(std::cout << "=====  成功  =====" << std::endl;)
    }
    else {
        MyDebugLog(std::cout << "=====  失敗  =====" << std::endl;)
    }

    // 最初の一度だけ or 指定したタイミングのみフラグを立てる
    m_IsFirstInitialized = !s_HasFirstGameSceneWaitInitialized;
    s_HasFirstGameSceneWaitInitialized = true;

    m_ShouldTransitionToStage = false;
    m_Tick = 0.0f;

    auto& instance = Game::GetInstance();

    // ライフの作成
    TextureManager* textureMgr = instance.GetInstance();

    // ライフの数だけハートの生成
    const float lifePosX = - 200.0f;
    const float lifePosY = - 100.0f;

    m_LifeCount = 4;
    for(uint32_t i = 0; i < m_LifeCount; ++i)
    {
        Square* life = instance.AddObject<Square>(instance.GetCamera());
        life->SetTexture(textureMgr->GetTexture("DestroyBullet.png"));
        life->SetPos(lifePosX + ( i * 130.0f ), lifePosY, 1.0f);
        life->SetScale(100.0f, 100.0f, 1.0f);
        life->SetShader("VS_Alpha","PS_Alpha");
        life->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

        m_MySceneObjects.emplace_back(life);
        m_LifeGame.emplace_back(life);
    }



}

void GameSceneWait::Update(float tick)
{
    // 乱数生成、ステージを厳選・二回連続で同じステージは踏まない
    m_Tick += tick;
    if (!m_ShouldTransitionToStage)
    {
        PrepareNextStage();
        m_Tick = 0.0f;
    }

    switch (m_RelationData.nextScene)
    {
    case SCENE_NO::GAME_SLICE:
        ChangeScenePush<GameSceneSlice>(TRANS_MODE::FADE, 0.3f);
        break;
    case SCENE_NO::GAME_PUSH:
        ChangeScenePush<GameScenePush>(TRANS_MODE::FADE, 0.3f);
        break;
    case SCENE_NO::GAME_HIT:
        ChangeScenePush<GameSceneHit>(TRANS_MODE::FADE, 0.3f);
        break;
    default:
        return;
    }

    m_ShouldTransitionToStage = false;
}

void GameSceneWait::Finalize()
{
    auto& instance = Game::GetInstance();
#ifdef _DEBUG
    instance.m_Grid.SetEnabled(false);
#endif

    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        instance.DeleteObject(o);
    }
    m_MySceneObjects.clear();
    // オーディオの停止
    if (AudioManager* audioManager = instance)
    {
        for (const auto& [key, config] : m_AudioList)
        {
            audioManager->StopAllByName(key);
        }
    }
}
