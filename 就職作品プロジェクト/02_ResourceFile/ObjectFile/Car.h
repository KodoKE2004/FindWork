#pragma once

#include <algorithm>

#include "Square.h"
#include "Collider.h"


enum class CarEasingType
{
    EaseInSine,
    EaseOutSine,
    EaseInOutSine,
    EaseInQuad,
    EaseOutQuad,
    EaseInOutQuad,
    EaseInCubic,
    EaseOutCubic,
    EaseInOutCubic,
};

class MoveInfo
{
private:
    static float ApplyEasing(CarEasingType type, float t);
public:
    NVector3      startPos = NVector3();                    
    NVector3      targetPos = NVector3();                   
    float         duration = 1.0f;                          
    float         elapsed = 0.0f;                           
    float         speedFactor = 1.0f;                       
    CarEasingType easingType = CarEasingType::EaseInOutSine;
    bool          loop = false;                             

    void  Reset();
    float Advance(float deltaTime);
    NVector3 Evaluate(float progress) const;

};

class Car : public Square
{
public:
    using Square::Square;
};