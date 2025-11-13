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

enum class CarDirection
{
    Left,
    Right,
};

class Cart : public Square
{
private:
    const float START_POSITION      = -700.0f;
    const float FIFNISHING_POSITION =  700.0f;

    float m_Distance = 1.0f; 
    bool  m_isActive = false;
    CarDirection m_Direction = CarDirection::Right;
    MoveInfo m_MoveInfo;

    void UpdateTargetFromConfig();

public:
    Cart(Camera* cam);

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void Start();
    void Stop();
    void Reset();

    void SetStartPosition(const NVector3& start);
    void SetDirection(CarDirection direction);
    void SetMoveDistance(float distance);
    void SetDuration(float duration);
    void SetSpeedFactor(float factor);
    void SetEasing(CarEasingType type);
    void SetLoop(bool loopEnabled);
};