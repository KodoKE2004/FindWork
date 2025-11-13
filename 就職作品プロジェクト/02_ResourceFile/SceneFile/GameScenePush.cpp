#include "GameScenePush.h"
#include "Game.h"

void GameScenePush::Initialize()
{
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;

    m_Skydome = instance.AddObject<Skydome>();
    m_Skydome->SetName("m_Skydome");
    m_Skydome->SetSkyDomeMode(true);
    m_Skydome->SetTexture(textureMgr->GetTexture("SkydomeSpace.png"));
    m_Skydome->SetRadius(500.0f);
    m_MySceneObjects.emplace_back(m_Skydome);

    m_Player = instance.AddObject<Player>(instance.GetCamera());
    m_Player->SetTexture(textureMgr->GetTexture("Plane.png"));
    m_MySceneObjects.emplace_back(m_Player);

    m_Cart = instance.AddObject<Cart>(instance.GetCamera());
    m_Cart->SetTexture(textureMgr->GetTexture("Plane.png"));
    m_MySceneObjects.emplace_back(m_Cart);

    if (m_Player)
    {
        m_Player->SetPos(0.0f, m_PlayerGroundHeight, 0.0f);
        const float jumpOffset = m_Player->GetScale().y * PlayerJumpHeightScale;
        m_PlayerJumpApexHeight = m_PlayerGroundHeight + jumpOffset;
    }

    m_CartStartPatterns = {
        CarStartPattern::GroundLeftToRight,
        CarStartPattern::GroundRightToLeft,
        CarStartPattern::JumpLeftToRight,
        CarStartPattern::JumpRightToLeft,
    };

    ApplyCartPattern(m_CurrentCartPatternIndex);
}

void GameScenePush::Update(float tick)
{
    if (m_Cart && !m_Cart->IsActive())
    {
        AdvanceCartPattern();
    }

    GameSceneExe::Update(tick);
}

void GameScenePush::Finalize()
{
    GameSceneExe::Finalize();
}

void GameScenePush::ApplyCartPattern(size_t index)
{
    if (!m_Cart || m_CartStartPatterns.empty())
    {
        return;
    }

    const size_t patternIndex = index % m_CartStartPatterns.size();
    m_CurrentCartPatternIndex = patternIndex;
    m_Cart->ConfigureStartPattern(m_CartStartPatterns[patternIndex], m_PlayerGroundHeight, m_PlayerJumpApexHeight);
}

void GameScenePush::AdvanceCartPattern()
{
    if (m_CartStartPatterns.empty())
    {
        return;
    }

    const size_t nextIndex = (m_CurrentCartPatternIndex + 1) % m_CartStartPatterns.size();
    ApplyCartPattern(nextIndex);
}
