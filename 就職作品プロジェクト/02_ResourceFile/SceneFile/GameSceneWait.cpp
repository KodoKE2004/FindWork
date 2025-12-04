#include "GameSceneWait.h"
#include "Game.h"
#include "DebugUI.h"
#include "SceneList.h"

#include <array>
#include <random>

bool GameSceneWait::s_HasFirstGameSceneWaitInitialized = false;
GAME_PHASE GameSceneWait::s_CurrentGamePhase = GAME_PHASE::START;

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
        SCENE_NO::GAME_JUMP,
        SCENE_NO::GAME_CRUSH
    };

    constexpr float kStageTransitionDelay = 1.0f;
}

void GameSceneWait::Initialize()
{
    DebugUI::TEXT_CurrentScene = "GameSceneWait";

    // 引き渡しデータのシーンの整理
    m_RelationData.oldScene      = m_RelationData.previousScene;
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

    RhythmBeatConst beatConfig{};
    m_TimerList.clear();
    SetTimer(&m_Tick);
    SetTimer(&m_ChangeStage.timer);
    SetTimer(&m_DecrementLife.timer);

    beatConfig.Setup(120.0f, 4, 1); // 120 BPM, 4/4 拍子
    m_RhythmBeat.Initialize(beatConfig);
    m_IsFirstInitialized = true;

    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance.GetInstance();

    // スカイドーム初期化
    // Skydome初期化 
    m_Skydome = instance.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);

    // ライフの数だけハートの生成
    const float lifePosX = - 200.0f;
    const float lifePosY = - 100.0f;

    m_ShouldTransitionToStage = false;
    m_wasDecrementLife        = false;

    // ライフオブジェクトの生成
    for(uint32_t i = 0; i < m_RelationData.gameLife; ++i)
    {
        const float distance = 130.0f;

        Square* life = instance.AddObject<Square>(instance.GetCamera());
        life->SetTexture(textureMgr->GetTexture("DestroyBullet.png"));
        life->SetPos(lifePosX + ( i * distance), lifePosY, 1.0f);
        life->SetScale(100.0f, 100.0f, 1.0f);
        life->SetShader("VS_Alpha","PS_Alpha");
        life->SetColor(1.0f, 1.0f, 1.0f, 1.0f);

        m_MySceneObjects.emplace_back(life);
        m_LifeGame.emplace_back(life);
    }

    const float initialTilt = m_IsLifeTiltPositive ? 30.0f : -30.0f;
    for (auto* life : m_LifeGame)
    {
        if (life)
        {
            life->SetRotate(life->GetRotate().x, life->GetRotate().y, initialTilt);
        }
    }

    // ステージ乱数選択処理   
    PrepareNextStage();
}

void GameSceneWait::Update(float tick)
{   
    auto& instance = Game::GetInstance();

    // 一定時間経過後に次のステージ選択処理へ
    if (m_ChangeStage.IsTimeUp())
    {
        m_ShouldTransitionToStage = true;
    }
    if (m_ShouldTransitionToStage)
    {
        StartNextStageTransition();
    }


    // リズムを取る
    // ライフをリズムに合わせて回転させる
    int advancedTicks = m_RhythmBeat.Update(tick);
    if (advancedTicks > 0)
    {
        for (int i = 0; i < advancedTicks; ++i)
        {
            m_IsLifeTiltPositive = !m_IsLifeTiltPositive;
        }

        const float tiltAngle = m_IsLifeTiltPositive ? 30.0f : -30.0f;
        for (auto* life : m_LifeGame)
        {
            if (life)
            {
                life->SetRotate(0.0f, 0.0f, tiltAngle);
            }
        }
    }

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

    // タイマー更新処理
    CountTimer(tick);

    // デバッグ用　終わったら消す予定のreturn
    // return ;
    #pragma region リザルトシーン遷移処理
    if (m_RelationData.gameLife == 0u)
    {
        // ライフが0になったらリザルトシーンへ
        ChangeScenePush<ResultScene>(WaitToResult);
    }
    #pragma endregion
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

// 次のステージ選択とシーン遷移処理
void GameSceneWait::StartNextStageTransition()
{
    // シーン遷移処理
    switch (m_RelationData.nextScene)
    {
    case SCENE_NO::GAME_SLICE: {
        ChangeScenePush<GameSceneSlice>(WaitToGame); 
    }  
    break;
    case SCENE_NO::GAME_JUMP:  {
        ChangeScenePush<GameSceneJump>(WaitToGame);
    }  
    break;
    case SCENE_NO::GAME_CRUSH: {
        ChangeScenePush<GameSceneCrush>(WaitToGame);
    }  
    break;
    default: return;
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
            _NextScene = RandomChoose<SCENE_NO>(SCENE_NO::GAME_JUMP,
                SCENE_NO::GAME_CRUSH);
            break;
        case SCENE_NO::GAME_JUMP:
            _NextScene = RandomChoose<SCENE_NO>(SCENE_NO::GAME_SLICE,
                SCENE_NO::GAME_CRUSH);
            break;
        case SCENE_NO::GAME_CRUSH:
            _NextScene = RandomChoose<SCENE_NO>(SCENE_NO::GAME_JUMP,
                SCENE_NO::GAME_SLICE);
            break;
        default: _NextScene = StageSelectAllRandom();
            break;
        }
    }

    // 次のシーンRelationDataに格納
    m_RelationData.nextScene = _NextScene;
}

// 全ステージから選ぶ時の乱数を取得する関数

SCENE_NO GameSceneWait::StageSelectAllRandom()
{
    SCENE_NO stageKinds[3] = {
            SCENE_NO::GAME_SLICE,
            SCENE_NO::GAME_CRUSH,
            SCENE_NO::GAME_JUMP
    };

    // 最初の一回だけ作成する乱数は３パターンの中から選択
    std::uniform_int_distribution<int> dist(0, 2);
    int selectedIdx = dist(m_RandomEngine);
    return stageKinds[selectedIdx];
}