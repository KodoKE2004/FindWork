#include "Car.h"

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
