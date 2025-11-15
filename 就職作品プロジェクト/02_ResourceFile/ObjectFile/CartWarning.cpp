#include "CartWarning.h"
#include "Application.h"
CartWarning::CartWarning(Camera* cam) : Square(cam)
{
}

void CartWarning::Initialize()
{
    Square::Initialize();
    m_isVisible = false;
    m_PulseTimer = 0.0f;
    m_BasePosition = NVector3(0.0f, 0.0f, 0.0f);
    SetName("CartWarning");
    SetBaseScale(m_DefaultScale);
    SetColor(Color(1.0f, 0.25f, 0.25f, 0.85f));
}

void CartWarning::Update()
{
    Square::Update();

    if(!m_isVisible) { return; }

    m_PulseTimer += Application::GetDeltaTime();
    const float oscillation = 1.0f + std::sin(m_PulseTimer * m_PulseSpeed) * m_PulseAmplitude;
    SetScale(m_DefaultScale.x * oscillation, m_DefaultScale.y * oscillation, m_DefaultScale.z);
    SetPos(m_BasePosition);
}

void CartWarning::Draw()
{
    if (!m_isVisible){ return; }

    Square::Draw();
}

void CartWarning::Finalize()
{
    Square::Finalize();
}

void CartWarning::SetBaseScale(const NVector3& scale)
{
    m_DefaultScale = scale;
    SetScale(m_DefaultScale);
}

void CartWarning::Activate(const NVector3& startPos)
{
    m_isVisible = true;
    m_PulseTimer = 0.0f;

    NVector3 directionToCenter = NVector3(0.0f, 0.0f, 0.0f) - startPos;
    const float length = directionToCenter.Length();

    if (length > 0.0f) {
        directionToCenter /= length;
    }

    const NVector3 offset = directionToCenter * m_TowardsCenterOffset;
    m_BasePosition = startPos + offset + m_AdditionalOffset;
    SetPos(m_BasePosition);
}

void CartWarning::Deactivate()
{
    if (!m_isVisible)
    {
        return;
    }

    m_isVisible = false;
    m_PulseTimer = 0.0f;
    SetScale(m_DefaultScale);
}

