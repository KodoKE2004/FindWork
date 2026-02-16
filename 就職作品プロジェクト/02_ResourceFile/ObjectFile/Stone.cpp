#include "Stone.h"
#include "Game.h"
#include "Calculator.h"
#include <random>

Stone::Stone(Camera& cam) : Square(cam)
{
}

namespace 
{
    static std::mt19937 engine{ std::random_device{}() };

    void SpawnPos(Stone& stone)
    {
        float screenWidthHalf = static_cast<float>(Application::GetWidth()) * 0.5f;
        float kMinX = - screenWidthHalf;
        float kMaxX =   screenWidthHalf;
        constexpr float kCenterSafeHalfWidth = 150.0f;       
        std::uniform_real_distribution<float> distY(600.0f, 1200.0f);
        std::uniform_int_distribution<int> distSide(0, 1);
        float posX = 0.0f;
        if (distSide(engine) == 0)
        {
            std::uniform_real_distribution<float> distX(kMinX, - kCenterSafeHalfWidth);
            posX = distX(engine);
        }
        else
        {
            std::uniform_real_distribution<float> distX(kCenterSafeHalfWidth, kMaxX);
            posX = distX(engine);
        }        
        
        float posY = distY(engine);
        stone.SetPos(posX, posY, 0.0f);
    }
}

void Stone::Initialize()
{
    Square::Initialize();
    auto& instance = Game::GetInstance();
    auto& rhythmBeat = Game::GetRhythmBeat();
    TextureManager* textureMgr = instance;
    SetTexture(textureMgr->GetTexture("GameScene/Stone.png"));
    SpawnPos(*this);
    SetScale(100.0f, 100.0f, 1.0f);

    m_isActive = true;

    m_Motion.velocity = Vector2::Zero;
    m_Motion.mass = 1.0f ;
    m_Motion.mag  = 10.0f + (rhythmBeat.GetBpm() * 0.01f);
    m_Motion.enableGravity = true;
    m_Motion.integrateX = false;
    m_Motion.integrateY = true;
    m_Motion.groundY = -700.0f;
    m_Motion.clampToGround = false;

    SetShader("VS_Alpha","PS_Alpha");
}

void Stone::Update()
{
    if (!IsActive()) {
        return;
    }

    const float dt = Application::GetDeltaTime();
    if (dt <= 0.0f)
    {
        return;
    }

    const NVector3 currentPos = GetPos();
    m_Motion.velocity.x = 0.0f;
    const Vector2 nextPos = Calculator::Physics::StepRigidBody(m_Motion, Vector2(currentPos.x, currentPos.y), dt);
    SetPos(currentPos.x, nextPos.y, currentPos.z);

    if (GetPos().y <= m_Motion.groundY) {
        DeActive();
    }
}

void Stone::Draw()
{
    if (!IsActive()) {
        return;
    }
    Square::Draw();
}

void Stone::DrawInstanced(const vector<pShared<Stone>>& stones)
{
    vector<InstanceTransform2D> transforms;
    transforms.reserve(stones.size());

    for (const auto& stone : stones)
    {
        if (!stone || !stone->IsActive()) {
            continue;
        }

        const auto stonePos = stone->GetPos();
        const auto stoneScale = stone->GetScale();
        transforms.push_back({
            DirectX::SimpleMath::Vector3(stonePos.x,   stonePos.y,   stonePos.z),
            DirectX::SimpleMath::Vector3(stoneScale.x, stoneScale.y, stoneScale.z)
            });
    }

    if (transforms.empty())
    {
        SetInstancingEnabled(false);
        return;
    }

    SetInstancingEnabled(true);
    SetInstanceTransforms(transforms);
    Square::Draw();
}

void Stone::Finalize()
{
    Square::Finalize();
}
