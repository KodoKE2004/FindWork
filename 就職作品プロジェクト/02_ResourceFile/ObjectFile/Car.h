#pragma once

#include <algorithm>

#include "Square.h"
#include "Collider.h"

/// <summary>
/// フェイントの挙動タイプ
/// </summary>
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
    LeftTop,
    LeftBottom,
    RightTop,
    RightBottom,
    NUM,
};

enum CarFaint
{
    None,
    Jumping,
    Landing,
    Fainting,
    Fainted,
    Recovering
};

enum class CarStartPattern
{
    GroundLeftToRight,
    GroundRightToLeft,
    JumpLeftToRight,
    JumpRightToLeft
};

class Cart : public Square
{
private:
    static constexpr float LeftStartPosX   = -700.0f;
    static constexpr float RightStartPosX  =  700.0f;
    static constexpr float TopStartPosY    =  200.0f;
    static constexpr float BottomStartPosY = -200.0f;
    static constexpr float StartPosZ       =  0.0f;
    static constexpr float DefaultGroundHeight = -100.0f;
    static constexpr float DefaultJumpOffset   =  300.0f;
    
    // 実際に移動する方向
    float m_MoveDirection = 1.0f;

    // 必要な移動量
    inline static constexpr Vector2 MoveValue {1400.0f, 400.0f};

    // 移動にかかる時間
    // ジャンプの場合でも上昇と下降で両方に同じ時間がかかるようにする
    // またゲームスピードに依存して変わるので定数として扱わない
    float m_MoveDuration = 1.5f;
    
    float m_Distance = 1.0f;
    bool  m_isActive = false;
    CarDirection m_Direction = CarDirection::RightBottom;
    CarStartPattern m_StartPattern = CarStartPattern::GroundLeftToRight;
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

    /// <summary>
    /// 難易度依存のパターン設定
    /// </summary>
    /// <param name="difficulty"></param> ゲームシーン側で求めた難易度
    void CreateStartPattern(int difficulty);

    void SetStartPattern(CarDirection carDirection);
    void Faint(float duration);

    bool IsActive() const { return m_isActive; }
    CarStartPattern GetStartPattern() const { return m_StartPattern; }
};
