#include "GameSceneShot.h"
#include "Game.h"
#include "Calculator.h"
#include "Enemy.h"

using namespace Calculator::Collider2D;

GameSceneShot::GameSceneShot(Camera& cam) : GameSceneExe(cam)
{
}

void GameSceneShot::Initialize()
{
#ifdef _DEBUG
    DebugUI::TEXT_CurrentScene = "GameSceneShot";
#endif 
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    // シーンに繋ぐ情報は基底初期化後の一番最初に設定
    m_RelationData.isClear = false;

    m_CreateBulletTime = 0.1f;

    // リズムの定義
    RhythmBeatConst beatConfig{};
    auto& rhythmBeat = Game::GetRhythmBeat();
    beatConfig.Setup(Game::GetBgmBpm());
    rhythmBeat.Initialize(beatConfig, false, BASE_BEATS);
    
    // ゲーム内の総拍数を参照するためリズム定義より後
    GameSceneExe::Initialize();

    m_Skydome = AddObject<Skydome>(instance.GetCamera());
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(5.0f);
    
    m_Player = AddObject<Player>(instance.GetCamera());
    m_Player->SetName("m_Player");
    m_Player->SetTexture(textureMgr->GetTexture("GameScene/GamePlane.png"));
    m_Player->SetPos  (  0.0f,   0.0f, 0.0f);
    m_Player->SetScale(300.0f, 200.0f, 0.0f);

    m_OperatorBar = AddObject<Square>(instance.GetCamera());
    m_OperatorBar->SetName("m_OperatorBar");
    m_OperatorBar->SetTexture(textureMgr->GetTexture("GameScene/OperationBar.png"));
    m_OperatorBar->SetPos  (  0.0f, - 270.0f, 0.0f);
    m_OperatorBar->SetScale(900.0f,   100.0f, 1.0f);

    m_PlaneHandle = AddObject<DragController>(instance.GetCamera());
    m_PlaneHandle->SetName("m_PlaneHandle");
    m_PlaneHandle->SetDirection(MOVE_RIGHT);
    m_PlaneHandle->SetTexture(textureMgr->GetTexture("GameScene/PlaneHandle.png"));
    m_PlaneHandle->SetPos  ( 0.0f, -270.0f, 1.0f);
    m_PlaneHandle->SetScale(50.0f,   50.0f, 1.0f);


    ////m_Player->SetScale();
    //int difficult = m_RelationData.stageCount / 4;
    //if (difficult >= 4){ difficult = 3; }
    //for (int i = 0; i <= difficult; ++i)
    //{
    //    auto enemy = AddObject<Enemy>(instance.GetCamera());
    //    enemy->SetName("m_Enemy");
    //    enemy->SetPos  ( 0.0f, - 180.0f, 1.0f);
    //}

    m_Bomber = AddObject<Bomber>(instance.GetCamera());
    m_Bomber->SetName("m_TimeGauge");
    m_MySceneObjects.emplace_back(m_Bomber->GetRope());
    m_MySceneObjects.emplace_back(m_Bomber->GetNumber());
}

void GameSceneShot::Update(float tick)
{
    return;

    // Skydomeの回転
    m_Skydome->Spin(0.0f, -4.0f, 0.0f);

    auto enemys = GetObjects<Enemy>();
    if (IsAllDeathEnemy(enemys)) {
        // SceneExeで早めにクリアをした場合も想定
        m_isFastChange = true;
        m_RelationData.isClear = true;
    }
    GameSceneExe::Update(tick);
    
    // ハンドルの位置に合わせて、プレイヤーも移動させる
    NVector3 planePos = {
        m_PlaneHandle->GetPos().x,
        m_Plane->GetPos().y,
        m_Plane->GetPos().z,
    };
    m_Plane->SetPos(planePos);

    m_CreateBulletElapsed += tick;
    if (m_CreateBulletTime <= m_CreateBulletElapsed)
    {
        CreateBullet();
        m_CreateBulletElapsed = 0.0f;
    }

    auto bullets = GetObjects<Bullet>();
    for (auto bullet : bullets) {
        if (!bullet->IsAlive()) {
            DeleteObject(bullet);
        }
    }


    if (!m_RelationData.isClear) 
    {

    }

    if (IsChange())
    {
        ChangeScene();
    }
}

void GameSceneShot::Draw()
{
    Scene::Draw();
}

void GameSceneShot::Finalize()
{
    GameSceneExe::Finalize();
}

void GameSceneShot::CreateBullet()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    pShared<Bullet> bullet = AddObject<Bullet>(instance.GetCamera());
    bullet->SetTexture(textureMgr->GetTexture("GameScene/Bullet.png"));
    Vector3 pos = {
        m_Player->GetPos().x,
        m_Player->GetPos().y + m_Player->GetScale().y * 0.5f,
        m_Player->GetPos().z,
    };
    bullet->SetName("m_Bullet");
    bullet->Alive();
    bullet->SetPos(pos);
    bullet->SetScale(25.0f,50.0f,0.0f);
    bullet->Shoot (pos, Vector3(0.0f,1.0f,0.0f));
}
