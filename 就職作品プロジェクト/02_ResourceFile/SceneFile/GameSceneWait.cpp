#include "GameSceneWait.h"
#include "Game.h"
#include "DebugUI.h"
#include "SceneList.h"

#include <array>
#include <random>

// a か b のどちらかをランダムに返すテンプレート関数
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
    // 次のステージ選択フラグをflase
    m_ShouldTransitionToStage = false;

    m_TimerList.clear();
    SetTimer(&m_Tick);
    SetTimer(&m_ChangeStage.timer);
    SetTimer(&m_DecrementLife.timer);

    auto& instance = Game::GetInstance();

    // ライフの作成
    TextureManager* textureMgr = instance.GetInstance();

    // ライフの数だけハートの生成
    const float lifePosX = - 200.0f;
    const float lifePosY = - 100.0f;

    m_ShouldTransitionToStage = false;
    m_isStageCleared          = false;
    m_wasDecrementLife        = false;

    for(uint32_t i = 0; i < m_RelationData.gameLife; ++i)
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
    auto& instance = Game::GetInstance();
    #pragma region ゲームステージ遷移処理
    // 一定時間経過後に次のステージ選択処理へ
    if (m_ChangeStage.IsTimeUp())
    {
        m_ShouldTransitionToStage = true;
    }
    if (m_ShouldTransitionToStage)
    {
        // ステージ乱数選択処理   
        PrepareNextStage();
        // シーン遷移処理
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
    }
    #pragma endregion
    #pragma region ライフ減少処理
    // ライフ減少処理
    if ( m_DecrementLife.IsTimeUp() &&
        !m_wasDecrementLife         &&
        !m_RelationData.isClear)
    {
        // ライフを減らす
        m_RelationData.gameLife -= 1u;
        DecrementLife();
        m_wasDecrementLife = true;
    }
    #pragma endregion
    #pragma region リザルトシーン遷移処理
    if (m_RelationData.gameLife == 0u)
    {
        // ライフが0になったらリザルトシーンへ
        ChangeScenePush<ResultScene>(TRANS_MODE::FADE,0.5f);
    }
    #pragma endregion
    // タイマー更新処理
    CountTimer(tick);
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

void GameSceneWait::DecrementLife()
{
    auto& instance = Game::GetInstance();
    if (!m_LifeGame.empty())
    instance.DeleteObject(m_LifeGame.back());
    m_LifeGame.pop_back();
    --m_LifeCount;
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

// 全ステージから選ぶ時の乱数を取得する関数

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