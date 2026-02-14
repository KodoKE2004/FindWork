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
    TextureManager* textureMgr = instance;
    SetTexture(textureMgr->GetTexture("GameScene/Stone.png"));
    SpawnPos(*this);
    SetScale(100.0f, 100.0f, 1.0f);

    m_isActive = true;

    m_VerticalMotion.velocity = 0.0f;
    m_VerticalMotion.gravity = 1200.0f;
    m_VerticalMotion.weight = 0.5f;
    m_VerticalMotion.terminalVelocity = -9000.0f;
    m_VerticalMotion.groundY = -10000.0f;

    SetShader("VS_Instansing2D","PS_Alpha");
}

void Stone::Update()
{
    if (!IsActive()) {
        return;
    }

    const float deltaTime = Application::GetDeltaTime();
    m_Position.y = Calculator::Physics::UpdateVerticalPosition(m_VerticalMotion, m_Position.y, deltaTime);

    if (GetPos().y <= -750.0f) {
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
