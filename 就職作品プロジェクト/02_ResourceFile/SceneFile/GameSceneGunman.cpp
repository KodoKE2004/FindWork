#include "GameSceneGunman.h"
#include "Game.h"
#include <algorithm>
#include <random>

namespace
{
    NVector3 kButtonPos[3] = {
        NVector3(-375.0f, - 0.0f, 0.0f),
        NVector3(   0.0f, - 0.0f, 0.0f),
        NVector3( 375.0f, - 0.0f, 0.0f),
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

GameSceneGunman::GameSceneGunman(Camera& cam) : GameSceneExe(cam)
{
}

void GameSceneGunman::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneGunman";
#endif
    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    StageFail();

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm());
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS - 8);

    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    auto& instance = Game::GetInstance();
    TextureManager* textureMar = instance;

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
            m_RedyList[i]->SetScale(100.0f, 100.0f, 1.0f);
            m_RedyList[i]->SetPos(kButtonPos[buttonIndices[i]]);
        }
        // エネミーの登録
        else
        {
            m_Oldman = AddObject<MouseObject>(instance.GetCamera());
            m_Oldman->SetName("m_Oldman");
            m_Oldman->SetTexture(textureMar->GetTexture("GameScene/GunmanOldman.png"));
            m_Oldman->SetScale(100.0f, 100.0f, 1.0f);
            m_Oldman->SetPos(kButtonPos[buttonIndices[i]]);
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

    RegisterAudio();

}

void GameSceneGunman::Update(float tick)
{
    // デバッグ中
    return;
    GameSceneExe::Update(tick);
}

void GameSceneGunman::Draw()
{
    for (auto object : m_MySceneObjects)
    {
        object->Draw();
    }
}

void GameSceneGunman::Finalize()
{
    // このシーンのオブジェクトを削除する
    for (auto o : m_MySceneObjects) {
        DeleteObject(o);
    }
    m_MySceneObjects.clear();
    GameSceneExe::Finalize();
}


