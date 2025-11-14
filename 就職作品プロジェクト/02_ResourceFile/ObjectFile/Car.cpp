#include "Car.h"
#include "Application.h"

#include <algorithm>
#include <random>

// óêêîê∂ê¨äÌÇÃèâä˙âª
std::random_device rd;
std::mt19937       gen(rd());

void MoveInfo::Reset()
{
    elapsed = 0.0f;
}

float MoveInfo::Advance(float deltaTime)
{
    if (duration <= 0.0f)
    {
        elapsed = duration;
        return 1.0f;
    }

    elapsed += deltaTime * speedFactor;
    if (loop)
    {
        while (elapsed > duration)
        {
            elapsed -= duration;
        }
    }
    else if (elapsed > duration)
    {
        elapsed = duration;
    }

    return std::clamp(elapsed / duration, 0.0f, 1.0f);
}

NVector3 MoveInfo::Evaluate(float progress) const
{
    progress = std::clamp(progress, 0.0f, 1.0f);
    const float eased = ApplyEasing(easingType, progress);
    return startPos + (targetPos - startPos) * eased;
}

float MoveInfo::ApplyEasing(CarEasingType type, float t)
{
    switch (type)
    {
    case CarEasingType::EaseInSine:     return Math::Easing::EaseInSine(t);
    case CarEasingType::EaseOutSine:    return Math::Easing::EaseOutSine(t);
    case CarEasingType::EaseInOutSine:  return Math::Easing::EaseInOutSine(t);
    case CarEasingType::EaseInQuad:     return Math::Easing::EaseInQuad(t);
    case CarEasingType::EaseOutQuad:    return Math::Easing::EaseOutQuad(t);
    case CarEasingType::EaseInOutQuad:  return Math::Easing::EaseInOutQuad(t);
    case CarEasingType::EaseInCubic:    return Math::Easing::EaseInCubic(t);
    case CarEasingType::EaseOutCubic:   return Math::Easing::EaseOutCubic(t);
    case CarEasingType::EaseInOutCubic: return Math::Easing::EaseInOutCubic(t);
    default:                            return t;
    }
}

Cart::Cart(Camera* cam) : Square(cam)
{
}

void Cart::Initialize()
{
    Square::Initialize();
}

void Cart::Update()
{
    if (!m_isActive)
    {
        return;
    }

    const float progress = m_MoveInfo.Advance(Application::GetDeltaTime());
    m_Position = m_MoveInfo.Evaluate(progress);

    if (!m_MoveInfo.loop && progress >= 1.0f)
    {
        m_isActive = false;
        m_Position = m_MoveInfo.targetPos;
    }
}

void Cart::Draw()
{
    Square::Draw();
}

void Cart::Finalize()
{
    Square::Finalize();
}

void Cart::Start()
{
    m_MoveInfo.Reset();
    m_isActive = true;
    m_Position = m_MoveInfo.startPos;
}

void Cart::Stop()
{
    m_isActive = false;
}

void Cart::Reset()
{
    m_MoveInfo.Reset();
    m_isActive = false;
    m_Position = m_MoveInfo.startPos;
}

void Cart::CreateStartPattern(int difficulty)
{
    int diff = 0;
    
    if(difficulty >= 4)
    {
        static std::mt19937 engine{ std::random_device{}()};
        std::uniform_int_distribution<int> dist(0, 3);
        diff = dist(engine);
    }
    else if(difficulty > 1 && difficulty < 4)
    {
        static std::mt19937 engine{ std::random_device{}() };
        std::uniform_int_distribution<int> dist(0, difficulty);
        diff = dist(engine);
    }
    else
    {
        diff = 0;
    }
    switch (diff)
    {
    case 0: m_Direction = CarDirection::LeftTop;        break;
    case 1: m_Direction = CarDirection::LeftBottom;     break;
    case 2: m_Direction = CarDirection::RightTop;       break;
    case 3: m_Direction = CarDirection::RightBottom;    break;
    }
}

void Cart::SetStartPattern(CarDirection carDirection)
{
    // äJénà íuê›íË
    float startPosY = TopStartPosY;
    if (carDirection == CarDirection::LeftBottom ||
        carDirection == CarDirection::LeftTop)
    {
        if (carDirection == CarDirection::LeftBottom) {
            startPosY = BottomStartPosY;
        }
        m_Direction = carDirection;
        m_Rotation.y = 0.0f;
        m_MoveInfo.startPos = NVector3(LeftStartPosX, startPosY, StartPosZ);

    }
    else if(carDirection == CarDirection::RightBottom ||
            carDirection == CarDirection::RightTop)
    {
        if (carDirection == CarDirection::RightBottom) {
            startPosY = BottomStartPosY;
        }
        m_Direction = carDirection;
        m_Rotation.y = 180.0f;
        m_MoveInfo.startPos = NVector3(RightStartPosX, startPosY, StartPosZ);
    }


}

void Cart::Faint(float duration)
{
}

void Cart::UpdateTargetFromConfig()
{
    const float sign = (m_Direction == CarDirection::RightBottom) ? 1.0f : -1.0f;
    m_MoveInfo.targetPos = m_MoveInfo.startPos + NVector3(sign * m_Distance, 0.0f, 0.0f);
}
