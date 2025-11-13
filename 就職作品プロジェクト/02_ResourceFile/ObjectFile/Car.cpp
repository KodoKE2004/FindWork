#include "Car.h"
#include "Application.h"

#include <cmath>

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
    m_MoveInfo.startPos = m_Position;

    const float defaultJumpApex = DefaultGroundHeight + DefaultJumpOffset;
    ConfigureStartPattern(m_StartPattern, DefaultGroundHeight, defaultJumpApex);
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

void Cart::SetStartPosition(const NVector3& start)
{
    m_MoveInfo.startPos = start;
    m_Position = start;
    UpdateTargetFromConfig();
    m_MoveInfo.Reset();
}

void Cart::ConfigureStartPattern(CarStartPattern pattern, float groundHeight, float jumpApexHeight)
{
    m_StartPattern = pattern;

    const bool startFromLeft =
        (pattern == CarStartPattern::GroundLeftToRight) ||
        (pattern == CarStartPattern::JumpLeftToRight);

    const bool useJumpHeight =
        (pattern == CarStartPattern::JumpLeftToRight) ||
        (pattern == CarStartPattern::JumpRightToLeft);

    const float startX = startFromLeft ? LeftStartX : RightStartX;
    const float startY = useJumpHeight ? jumpApexHeight : groundHeight;
    const CarDirection direction = startFromLeft ? CarDirection::Right : CarDirection::Left;

    m_Direction = direction;
    m_Distance = std::abs(RightStartX - LeftStartX);
    SetStartPosition(NVector3(startX, startY, StartPosZ));
    Start();
}

void Cart::SetDirection(CarDirection direction)
{
    if (m_Direction == direction)
    {
        return;
    }

    m_Direction = direction;
    UpdateTargetFromConfig();
    m_MoveInfo.Reset();
}

void Cart::SetMoveDistance(float distance)
{
    m_Distance = max(distance, 0.0f);
    UpdateTargetFromConfig();
    m_MoveInfo.Reset();
}

void Cart::SetDuration(float duration)
{
    m_MoveInfo.duration = max(duration, 0.0f);
    m_MoveInfo.Reset();
}

void Cart::SetSpeedFactor(float factor)
{
    m_MoveInfo.speedFactor = max(factor, 0.0f);
}

void Cart::SetEasing(CarEasingType type)
{
    m_MoveInfo.easingType = type;
}

void Cart::SetLoop(bool loopEnabled)
{
    m_MoveInfo.loop = loopEnabled;
}

void Cart::UpdateTargetFromConfig()
{
    const float sign = (m_Direction == CarDirection::Right) ? 1.0f : -1.0f;
    m_MoveInfo.targetPos = m_MoveInfo.startPos + NVector3(sign * m_Distance, 0.0f, 0.0f);
}
