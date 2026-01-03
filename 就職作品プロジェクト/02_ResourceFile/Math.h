#pragma once
#include <array>
#include <cmath>
#include <SimpleMath.h>

#include "Circle.h"
#include "Square.h"
#include "Transform.h"

namespace Math
{
    struct SquareInfo
    {
        NVector3 pos;       // 頂点情報
        NVector3 axisX;     // 大きさ情報
        NVector3 axisY;     // 回転情報
        float halfW;        // 半分の幅
        float halfH;        // 半分の高さ
    };

    namespace Collider2D
    {
        static bool OverlapOnAxis(NVector3 & axis, std::array<NVector3,4>& vertsA, std::array<NVector3,4>& vertsB);
        static bool IsHitPoint(NVector3& pt, SquareInfo sq);
        bool isHitSquareCircle(Square& square , Circle& circle );
        bool isHitCircleCircle(Circle& circleA, Circle& circleB);
        bool isHitSquareSquare(Square& squareA, Square& squareB);

        // Tranformから当たり判定用の情報を取得
        SquareInfo SettingVertexInfo(Transform transform);

        // 対角線の長さを取得
        float CreateDiagonalLength(Transform tra);

        // 大きいかどうかを判定
        bool  ColliderMore(Transform traA, Transform traB);

        // 四隅のワールド座標を取得
        std::array<NVector3, 4> SettingVertex(Transform transform);
    }

    namespace Easing
    {
        float EaseInSine(float p);
        float EaseOutSine(float p);
        float EaseInOutSine(float p);
        float EaseInQuad(float p);
        float EaseOutQuad(float p);
        float EaseInOutQuad(float p);
        float EaseInCubic(float p);
        float EaseOutCubic(float p);
        float EaseInOutCubic(float p);
        float EaseInQuart(float p);
        float EaseOutQuart(float p);
        float EaseInOutQuart(float p);
        float EaseInQuint(float p);
        float EaseOutQuint(float p);
        float EaseInOutQuint(float p);
        float EaseInExpo(float p);
        float EaseOutExpo(float p);
        float EaseInOutExpo(float p);
        float EaseInCirc(float p);
        float EaseOutCirc(float p);
        float EaseInOutCirc(float p);
        float EaseInBack(float p);
        float EaseOutBack(float p);
        float EaseInOutBack(float p);
        float EaseInElastic(float p);
        float EaseOutElastic(float p);
        float EaseInOutElastic(float p);
        float EaseInBounce(float p);
        float EaseOutBounce(float p);
        float EaseInOutBounce(float p);

    };

    namespace Physics
    {
        struct VerticalMotionState
        {
            float velocity = 0.0f;
            float gravity = 980.0f;
            float weight = 1.0f;
            float terminalVelocity = -2500.0f;
            float groundY = 0.0f;
        };

        float UpdateVerticalPosition(VerticalMotionState& state, float currentPosY, float deltaTime);

        struct HorizontalMotionState
        {
            float velocity = 0.0f;
            float acceleration = 4200.0f;
            float maxSpeed     = 650.0f;
            float airFriction  = 2200.0f;
        };

        struct HorizontalMotionResult
        {
            float velocity = 0.0f;
            float positionX = 0.0f;
        };

    };

};

