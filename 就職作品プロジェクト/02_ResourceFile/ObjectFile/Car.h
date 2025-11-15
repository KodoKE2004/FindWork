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

    void  Reset();
    float Advance(float deltaTime);
    NVector3 Evaluate(float progress) const;

};

enum CarDirection
{
    LeftTop   = 0,
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

class Cart : public Square
{
private:
    static constexpr float StartPosX = 700.0f;
    static constexpr float StartPosY = 200.0f;
    static constexpr float StartPosZ = 0.0f;
    static constexpr float MoveInfoBaseDuration   = 1.5f;
    static constexpr float HorizontalDurationBase = 3.0f;
    static constexpr float VerticalDurationBase   = 0.5f;

    // 実際に移動する方向
    float m_MoveDirection = 1.0f;

    // 必要な移動量 
    // X = 移動 
    // Y = ジャンプ
    inline static Vector2 m_MoveValue {1400.0f, 400.0f};

    // 移動にかかる時間
    // ゲームスピードに依存して変わるので定数として扱わない
    float m_SpeedFactor = 1.0f;
    // ジャンプの場合でも上昇と下降で両方に同じ時間がかかるようにする
    // またゲームスピードに依存して変わるので定数として扱わない
    Vector2 m_MoveDuration{ HorizontalDurationBase, VerticalDurationBase };
    Vector2 m_MoveTimer   { 0.0f, 0.0f };
    Vector2 m_MoveSpeed   { 0.0f, 0.0f };

    float m_Distance = 1.0f;
    bool  m_isActive = false;
    CarDirection m_Direction = CarDirection::RightBottom;
    MoveInfo     m_MoveInfo;

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

    // CreateStartPattern 
    // 進行方向の設定
    // 乱数を用いる
    // 難易度によってパターンを制限する
    // 難易度アップごとに１パターン増加
    // デフォルトの値が１なのでその時は乱数を仕様しない
    // 最大値は４パターンまで    
    void CreateStartPattern(int difficulty);

    // SetStartPattern
    // 開始パターンの設定
    // 必要な移動量の算出
    // m_Directionを基に下記の設定
    //ロロロロロロロロロロロロロロロロロロ 
    // m_MoveInfo.startPos
    // m_MoveInfo.targetPos
    // MoveValue
    void SetStartPattern();

    // スピード倍率の設定
    void SetSpeedFactor(float factor);

    // フェイント動作の設定
    void SetFaint();

    void Faint(float duration);

    bool IsActive() const { return m_isActive; }

    const NVector3& GetStartPos() const { return m_MoveInfo.startPos; }
    CarDirection GetDirection() const { return m_Direction; }
};
