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

    NVector3 kSmashStartScale  = NVector3(300.0f,  300.0f, 1.0f);
    NVector3 kSmashTargetPos   = NVector3(  0.0f,  300.0f, 0.0f);
    NVector3 kSmashTargetScale = NVector3( 10.0f,   10.0f, 1.0f);

    Transform kSmashTransform = []()
    {
        Transform transform;
        transform.SetPos   (NVector3(0.0f, 0.0f, 0.0f));
        transform.SetRotate(NVector3(0.0f, 0.0f, 0.0f));
        transform.SetScale (kSmashStartScale);
        return transform;
    }();

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

void GameSceneGunman::TargetMovePosition()
{
    float moveValueY = 0.0f;
    float moveValueX = 0.0f;
    float duration    = 0.0f;
    // 上から下
    if (m_isShot)
    {
        // Y座標は同じなので、X座標の移動量だけ計算
        duration = Game::GetRhythmBeat().GetOneBeat();

        const float progress = std::clamp(m_MoveUpElapsed / duration, 0.0f, 1.0f);

        const float easedProgress = Calculator::Easing::EvaluateEasing(EASING_TYPE::OUT_QUAD, progress);

        // 開始時の位置とサイズを設定
        auto startSetting = [](pShared<MouseObject> mouseObject)
        {
            kSmashTransform.SetScale(mouseObject->GetScale());
            return mouseObject ? mouseObject->GetPos() : NVector3(0.0f, 0.0f, 1.0f);
        };

        NVector3 startPos{};
        switch (m_ShotIndex)
        {
            case 0: startPos = startSetting(m_RedyList[0]); break;
            case 1: startPos = startSetting(m_RedyList[1]); break;
            case 2: startPos = startSetting(m_Oldman     ); break;
        }

        moveValueX = kSmashTargetPos.x - startPos.x;
        moveValueY = kSmashTargetPos.y - startPos.y;

        const float currentX = startPos.x + moveValueX * easedProgress;
        const float currentY = startPos.y + moveValueY * easedProgress;
        const float currentScaleX = kSmashStartScale.x + (kSmashTargetScale.x - kSmashStartScale.x) * easedProgress;
        const float currentScaleY = kSmashStartScale.y + (kSmashTargetScale.y - kSmashStartScale.y) * easedProgress;

        kSmashTransform.SetPos(NVector3(currentX, currentY, 0.0f));
        kSmashTransform.SetScale(NVector3(currentScaleX, currentScaleY, 1.0f));
        

        bool isMax = progress >= 1.0f;
        
        auto transform = [](std::shared_ptr<MouseObject> mouseObject, bool isTimer)
        {
            if (mouseObject) 
            {
                float rotateZ = mouseObject->GetRotate().z + 3.0f; // 360度回転
                mouseObject->SetPos  (kSmashTransform.GetPos()  );
                mouseObject->SetScale(kSmashTransform.GetScale());
                mouseObject->SetRotate(NVector3(0.0f, 0.0f, rotateZ));
                if (isTimer)
                {
                    TextureManager* textureMar = Game::GetInstance();
                    mouseObject->SetTexture(textureMar->GetTexture("GameScene/SmashStar.png"));
                    mouseObject->SetScale (NVector3(100.0f, 100.0f, 1.0f));
                }
            }
        };

        switch (m_ShotIndex)
        {
        case 0: transform(m_RedyList[0], isMax); break;
        case 1: transform(m_RedyList[1], isMax); break;
        case 2: transform(m_Oldman     , isMax); break;
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
   
    if(m_isShot) return;

    AudioManager* audioMgr = Game::GetInstance();
    if (m_Oldman->IsDrag())
    {
        m_ReactionActive = audioMgr->Create(m_AudioList.at("clear"));
        m_ReactionActive->Play(m_AudioList.at("clear").params);
        m_ShotIndex = 2;
        m_isCreateReactionSE   = true;
        m_RelationData.isClear = true;
        m_isShot               = true;
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
            m_isShot             = true;
            m_ShotIndex = i;
            m_MoveUpElapsed = 0.0f;
            break;
        }
    }
    

}

void GameSceneGunman::UpdateSubjectScale(float tick)
{
    if (!m_Subject) return;

    const float duration = max(Game::GetRhythmBeat().GetOneBeat(), 0.01f);
    m_SubjectScaleElapsed += tick;

    if (m_SubjectScaleElapsed >= duration)
    {
        m_SubjectScaleElapsed -= duration;
        m_isSubjectScaleUp = !m_isSubjectScaleUp;
    }

    const float progress = std::clamp(m_SubjectScaleElapsed / duration, 0.0f, 1.0f);
    const float easedProgress = Calculator::Easing::EvaluateEasing(EASING_TYPE::IN_OUT_SINE, progress);

    const float minScaleRatio = 1.0f;
    const float maxScaleRatio = 1.3f;
    const float scaleRatio = m_isSubjectScaleUp
        ? (minScaleRatio + (maxScaleRatio - minScaleRatio) * easedProgress)
        : (maxScaleRatio - (maxScaleRatio - minScaleRatio) * easedProgress);

    m_Subject->SetScale(m_SubjectBaseScale * scaleRatio);
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
    m_isShot        = false;
    m_SubjectScaleElapsed = 0.0f;
    m_isSubjectScaleUp    = true;

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm());
    rhythmBeat.Initialize(beatConfig, false, ONE_MEASURE * 2);

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
    m_Wanted->SetPos  (-400.0f, 150.0f, 0.0f);
    m_Wanted->SetScale( 270.0f, 360.0f, 1.0f);
    m_Wanted->SetTexture(textureMar->GetTexture("GameScene/Wanted.png"));
    
    m_Subject = AddObject<Square>(instance.GetCamera());
    m_Subject->SetName("m_Subject");
    m_Subject->SetPos  (-400.0f, 120.0f, 0.0f);
    m_Subject->SetScale( 200.0f, 200.0f, 1.0f);
    m_SubjectBaseScale = m_Subject->GetScale();
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

    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    PlayParams bgmParams;
    m_AudioList.emplace("bgmGunman", AudioConfig(L"BGM/GameSceneMelody/Gunman.wav", bgmParams, true, true));

    PlayParams clearParams;
    m_AudioList.emplace("clear", AudioConfig(L"SE/GameReaction/True1.wav", clearParams, false, false));

    PlayParams missParams;
    m_AudioList.emplace("miss", AudioConfig(L"SE/GameReaction/False2.wav", missParams, false, false));
    
    PlayParams shotParams;
    m_AudioList.emplace("shot", AudioConfig(L"SE/GameReaction/HandGun.wav", missParams, false, false));

    RegisterAudio();
    PlaySE("bgmGunman", 0.3f);

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
        TargetMovePosition();
    }

    if (Input::GetMouseTrigger(vkLEFT) || Input::GetKeyTrigger(VK_RETURN)) {
        PlaySE("shot", 0.3f);
    }
    ShotReaction();

    // リズムをとって指名手配の方の画像をスケールさせる
    UpdateSubjectScale(tick);
    
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



