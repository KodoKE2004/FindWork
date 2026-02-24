#include "GameSceneGunman.h"
#include "Game.h"
#include "Calculator.h"
#include "Cube.h"
#include "Transform.h"

#include <algorithm>
#include <random>

namespace
{
    NVector3 kStartPos[3] = {
        NVector3(-300.0f, - 900.0f, 0.0f),
        NVector3(   0.0f, - 900.0f, 0.0f),
        NVector3( 300.0f, - 900.0f, 0.0f),
    };

    NVector3 kTargetPos[3] = {
        NVector3(-300.0f, - 100.0f, 0.0f),
        NVector3(   0.0f, - 100.0f, 0.0f),
        NVector3( 300.0f, - 100.0f, 0.0f),
    };

    Transform kSmashTransform;
    kSmashTransform.SetPos(NVector3(0.0f, 300.0f, 0.0f));
    kSmashTransform.SetRotate(NVector3(0.0f, 0.0f, 0.0f));
    kSmashTransform.SetScale(NVector3(300.0f, 300.0f, 1.0f));

    NVector3 kBaseScaleRedy = {
        NVector3(150.0f, 300.0f, 1.0f)
    };

    NVector3 kBaseScaleWanted = {
        NVector3(300.0f, 300.0f, 1.0f)
    };
    
    std::array<size_t, 3> ShuffleButtonIndices()
    {
        static std::mt19937 engine{ std::random_device{}() };
        std::array<size_t, 3> indices{ 0, 1, 2 };
        std::shuffle(indices.begin(), indices.end(), engine);
        return indices;
    }

    std::array<float, 2> ShuffleFloatIndices(std::array<float, 2> indices)
    {
        static std::mt19937 engine{ std::random_device{}() };
        std::shuffle(indices.begin(), indices.end(), engine);
        return indices;
    }


};

void GameSceneGunman::TargetMovePosition(bool isShot)
{
    float moveValueY = 0.0f;
    float moveValueX = 0.0f;
    float duration    = 0.0f;
    // 上から下
    if (isShot)
    {
        moveValueX = kSmashTransform.GetPos().x - kTargetPos[0].x;
        moveValueY = kSmashTransform.GetPos().y - kTargetPos[0].y;
        duration = Game::GetRhythmBeat().GetOneBeat();

        const float progress = std::clamp(m_MoveUpElapsed / duration, 0.0f, 1.0f);

        const float easedProgress = Calculator::Easing::EvaluateEasing(EASING_TYPE::OUT_QUAD, progress);
        float currentX = kSmashTransform.GetPos().x + moveValueX * easedProgress;
        float currentY = kSmashTransform.GetPos().y + moveValueY * easedProgress;

        switch(m_ShotIndex)
        {
        case 0: m_RedyList[0]->SetPos(currentX, currentY, 0.0f); break;
        case 1: m_RedyList[1]->SetPos(currentX, currentY, 0.0f); break;
        case 2: m_Oldman     ->SetPos(currentX, currentY, 0.0f); break;
        }
    }
    // 下から上
    else
    {
        moveValueY = kTargetPos[0].y - kStartPos[0].y;
        duration = Game::GetRhythmBeat().GetOneBeat() / 2;

        float progress   = std::clamp(m_MoveUpElapsed / duration,0.0f, 1.0f);

        const auto& easedProgress = Calculator::Easing::EvaluateEasing(EASING_TYPE::IN_QUAD, progress);
        float currentY = kStartPos[0].y + moveValueY * easedProgress;
        for (int i = 0; i < 2; ++i)
        {
            m_RedyList[i]->SetPos(m_RedyList[i]->GetPos().x, currentY, 0.0f);
        }
        m_Oldman->SetPos(m_Oldman->GetPos().x, currentY, 0.0f);
    }
    
}

void GameSceneGunman::ShotReaction()
{
   
    AudioManager* audioMgr = Game::GetInstance();

    if (m_Oldman->IsDrag())
    {
        m_ReactionActive = audioMgr->Create(m_AudioList.at("clear"));
        m_ReactionActive->Play(m_AudioList.at("clear").params);
        m_ShotIndex = 2;
        m_isCreateReactionSE   = true;
        m_RelationData.isClear = true;
        m_MoveUpElapsed = 0.0f;
        return;
    }

    for (size_t i = 0; i < m_RedyList.size(); ++i)
    {
        if (m_RedyList[i]->IsDrag())
        {
            
            m_ReactionActive = audioMgr->Create(m_AudioList.at("miss"));
            m_ReactionActive->Play(m_AudioList.at("miss").params);
            m_isCreateReactionSE = true;
            m_ShotIndex = i;
            m_MoveUpElapsed = 0.0f;
            break;
        }
    }
    

}

GameSceneGunman::GameSceneGunman(Camera& cam) : GameSceneExe(cam)
{

}

void GameSceneGunman::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneGunman";
#endif

    m_RelationData.isClear = false;

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    StageFail();

    // メンバ変数初期化
    m_MoveUpElapsed = 0.0f;
    m_ShotIndex     = 0;

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm());
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS - 8);

    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    auto& instance = Game::GetInstance();
    TextureManager* textureMar = instance;

    // オブジェクトの登録
    m_Background = AddObject<Square>(instance.GetCamera());
    m_Background->SetName("m_Background");
    m_Background->SetScale(1280.0f, 720.0f, 1.0f);
    m_Background->SetTexture(textureMar->GetTexture("BackGround/Gunman.png"));

    // 指名手配役の登録
    m_Wanted = AddObject<Square>(instance.GetCamera());
    m_Wanted->SetName("m_Wanted");
    m_Wanted->SetPos  (  0.0f, 150.0f, 0.0f);
    m_Wanted->SetScale(180.0f, 240.0f, 1.0f);
    m_Wanted->SetTexture(textureMar->GetTexture("GameScene/Wanted.png"));
    
    m_Subject = AddObject<Square>(instance.GetCamera());
    m_Subject->SetName("m_Subject");
    m_Subject->SetPos(0.0f, 150.0f, 0.0f);
    m_Subject->SetScale(120.0f, 150.0f, 1.0f);
    m_Subject->SetTexture(textureMar->GetTexture("GameScene/GunmanOldman.png"));

    // 位置をシャッフルして配置
    std::array<size_t, 3> buttonIndices = ShuffleButtonIndices();
    for (int i = 0; i < 3; ++i)
    {
        // 女性の追加登録
        if (i != 2)
        {
            m_RedyList[i] = AddObject<MouseObject>(instance.GetCamera());
            m_RedyList[i]->SetName("m_Redy" + std::to_string(i));
            m_RedyList[i]->SetTexture(textureMar->GetTexture("GameScene/GunmanRedy.png"));
            m_RedyList[i]->SetScale(kBaseScaleRedy);
            m_RedyList[i]->SetPos(kStartPos[buttonIndices[i]]);
        }
        // エネミーの登録
        else
        {
            m_Oldman = AddObject<MouseObject>(instance.GetCamera());
            m_Oldman->SetName("m_Oldman");
            m_Oldman->SetTexture(textureMar->GetTexture("GameScene/GunmanOldman.png"));
            m_Oldman->SetScale(kBaseScaleWanted);
            m_Oldman->SetPos(kStartPos[buttonIndices[i]]);
        }
    }


    m_Bomber = AddObject<Bomber>(instance.GetCamera());
    m_Bomber->SetName("m_TimeGauge");

    m_MySceneObjects.emplace_back(m_Bomber->GetRope());
    m_MySceneObjects.emplace_back(m_Bomber->GetNumber());

    PlayParams bgmParams;
    m_AudioList.emplace("bgmRocket", AudioConfig(L"BGM/GameSceneMelody/Rocket.wav", bgmParams, true, true));

    PlayParams clearParams;
    m_AudioList.emplace("clear", AudioConfig(L"SE/GameReaction/True1.wav", clearParams, false, false));

    PlayParams missParams;
    m_AudioList.emplace("miss", AudioConfig(L"SE/GameReaction/False2.wav", missParams, false, false));
    
    PlayParams shotParams;
    m_AudioList.emplace("shot", AudioConfig(L"SE/GameReaction/HandGun.wav", missParams, false, false));

    RegisterAudio();

}

void GameSceneGunman::Update(float tick)
{
    GameSceneExe::Update(tick);
    auto& rhythmBeat  = Game::GetRhythmBeat();
    // 経過拍数を取得(4拍基準)
    int   elapsedBeat = rhythmBeat.GetBeatElapsed() / 2;

    if (elapsedBeat > 1) 
    {
        m_MoveUpElapsed += tick;
        TargetMovePosition(m_isCreateReactionSE);
    }

    ShotReaction();
    // リズムをとって指名手配の方の画像をスケールさせる

}

void GameSceneGunman::Draw()
{
    for (auto& object : m_MySceneObjects)
    {
        object->Draw();
    }
}

void GameSceneGunman::Finalize()
{
    // このシーンのオブジェクトを削除する
    for (auto& o : m_MySceneObjects) {
        DeleteObject(o);
    }
    m_MySceneObjects.clear();
    GameSceneExe::Finalize();
}


