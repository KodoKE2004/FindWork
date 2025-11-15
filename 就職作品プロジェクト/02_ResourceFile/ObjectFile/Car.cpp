#include "Car.h"
#include "Application.h"

#include <algorithm>
#include <random>

// 乱数生成器の初期化
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
   

    return std::clamp(elapsed / duration, 0.0f, 1.0f);
}

NVector3 MoveInfo::Evaluate(float tick) const
{
    tick = std::clamp(tick, 0.0f, 1.0f);
    const float eased = ApplyEasing(easingType, tick);
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
    m_MoveInfo = {
        NVector3( StartPosX,  StartPosY, StartPosZ), // startPos
        NVector3( StartPosX,  StartPosY, StartPosZ), // targetPos
        MoveInfoBaseDuration,                        // duration
        0.0f,                                        // elapsed
        1.0f,                                        // speedFactor
        CarEasingType::EaseInOutSine                 // easingType
    };
    m_Position = m_MoveInfo.startPos;

}

void Cart::Update()
{
    if (!m_isActive) { return; }

    const float deltaTime = Application::GetDeltaTime();
    auto advanceAxis = [&](float& position, float& target, float& timer, float& duration, float& speed)
    {
        if (duration <= 0.0f)
        {
            position = target;
            timer = duration;
            return;
        }

        const float remainingTime = duration - target;
        const float appliedDelta = min(deltaTime, remainingTime);

        if (appliedDelta <= 0.0f)
        {
            position = target;
            timer    = duration;
            return;
        }

        position += speed * appliedDelta;
        timer    += appliedDelta;
        if (timer >= duration)
        {
            position = target;
            timer = duration;
        }
    };

    advanceAxis(m_Position.x, m_MoveInfo.targetPos.x, m_MoveTimer.x, m_MoveDuration.x, m_MoveSpeed.x);
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
    m_MoveTimer = { 0.0f, 0.0f };
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
    m_MoveTimer = { 0.0f, 0.0f };
}


void Cart::CreateStartPattern(int difficulty)
{
    int diff = 0;
    
    if(difficulty >= 3)
    {
        static std::mt19937 engine{ std::random_device{}()};
        std::uniform_int_distribution<int> dist(0, 3);
        diff = dist(engine);
    }

    else if(difficulty > 1 && difficulty < 3)
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

void Cart::SetStartPattern()
{
    // 開始位置設定
    Vector2 directionMass{1.0f, 1.0f};

    // 左右の識別
    if (m_Direction == CarDirection::RightTop ||
        m_Direction == CarDirection::RightBottom)
    {
        // 左から右の場合
        directionMass.x *= - 1.0f;
        m_MoveValue.x *= -directionMass.x;
    }
    // 上下の識別
    if (m_Direction % 2 == 0)
    {
        // 上から下の場合
        directionMass.y *= -1.0f;
        m_MoveValue.y *= directionMass.y;
    }

    // 開始位置の設定
    m_MoveInfo.startPos = NVector3(
        StartPosX * directionMass.x,
        StartPosY * directionMass.y,
        StartPosZ
    );

    // 目標位置の設定
    m_MoveInfo.targetPos = NVector3(
        StartPosX * ( - directionMass.x),
        StartPosY * ( - directionMass.y),
        StartPosZ
    );

    // 必要な移動量の設定
    m_MoveValue = {
        m_MoveValue.x * (- directionMass.x),
        m_MoveValue.y * (  directionMass.y)
    };

    const float speedFactor = (m_SpeedFactor > 0.0f) ? m_SpeedFactor : 1.0f;
    
    m_MoveDuration = {
        HorizontalDurationBase / m_SpeedFactor,
        VerticalDurationBase   / m_SpeedFactor
    };

    // 移動速度の設定
    m_MoveSpeed = {
        m_MoveValue.x / m_MoveDuration.x,
        m_MoveValue.y / m_MoveDuration.y
    };

    m_MoveInfo.duration = MoveInfoBaseDuration / speedFactor;

}

void Cart::SetSpeedFactor(float factor)
{
    m_SpeedFactor = factor;
}

void Cart::SetFaint()
{
    
}

void Cart::Faint(float duration)
{

}

void Cart::UpdateTargetFromConfig()
{
    const float sign = (m_Direction == CarDirection::RightBottom) ? 1.0f : -1.0f;
    m_MoveInfo.targetPos = m_MoveInfo.startPos + NVector3(sign * m_Distance, 0.0f, 0.0f);
}
