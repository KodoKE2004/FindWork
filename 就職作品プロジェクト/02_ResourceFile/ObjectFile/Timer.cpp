#include "Timer.h"
#include <algorithm>
#include <DirectXMath.h>

Timer::Timer(Camera& cam) : Square(cam)
{
}

void Timer::Initialize()
{
    Square::Initialize();
    SetScale(120.0f, 120.0f, 1.0f);
    SetColor(1.0f, 1.0f, 1.0f, 0.8f);

    m_Meter = std::make_shared<Circle>(m_Camera);
    m_Meter->SetRadius(45.0f);
    m_Meter->Initialize();
    m_Meter->SetColor(0.2f, 0.9f, 1.0f, 0.85f);
    m_Meter->SetRotate(0.0f, 0.0f, -DirectX::XM_PIDIV2);
}

void Timer::Update()
{
    if (!m_Meter)
    {
        return;
    }

    m_Meter->SetPos(m_Position.x, m_Position.y, m_Position.z);
    m_Meter->SetFillRatio(m_Progress);
}

void Timer::Draw()
{
    Square::Draw();
    if (m_Meter)
    {
        m_Meter->Draw();
    }
}

void Timer::Finalize()
{
    if (m_Meter)
    {
        m_Meter->Finalize();
        m_Meter = nullptr;
    }
    Square::Finalize();
}

void Timer::SetProgress(const float ratio)
{
    m_Progress = std::clamp(ratio, 0.0f, 1.0f);
}
