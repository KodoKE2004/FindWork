#pragma once
#include <cmath>
#include <SimpleMath.h>
#include <array>

#include "Transform.h"
#include "Circle.h"
#include "Square.h"

namespace Calculator
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
        bool isHitSquareCircle(Transform& square,  Transform& circle);
        bool isHitCircleCircle(Transform& circleA, Transform& circleB);
        bool isHitSquareSquare(Transform& squareA, Transform& squareB);

        // Transformから当たり判定用の情報を取得
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
        enum class EASING_TYPE
        {
            NONE,
            IN_SINE,
            IN_QUAD,
            IN_CUBIC,
            IN_QUINT,
            IN_QUART,
            IN_EXPO,
            IN_CIRC,
            IN_BACK,
            IN_BOUNCE,
            IN_ELASTIC,
            OUT_SINE,
            OUT_QUAD,
            OUT_CUBIC,
            OUT_QUINT,
            OUT_QUART,
            OUT_CIRC,
            OUT_BACK,
            OUT_ElASTIC,
            OUT_BOUNCE,
            OUT_EXPO,
            IN_OUT_SINE,
            IN_OUT_QUAD,
            IN_OUT_CUBIC,
            IN_OUT_QUINT,
            IN_OUT_QUART,
            IN_OUT_CIRC,
            IN_OUT_BACK,
            IN_OUT_ELASTIC,
            IN_OUT_BOUNCE,
            IN_OUT_EXPO,
            NUM
        };

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

        float EvaluateEasing(EASING_TYPE type,float p);

        NVector3 ScalingObject(float elapsed, float duration, NVector3 base, NVector3 goal, EASING_TYPE easing);
    };

    namespace Physics
    {
        enum class CollisionQuadrant
        {
            NONE,
            LEFTUP,
            LEFTDOWN,
            RIGHTUP,
            RIGHTDOWN,
        };

        enum class CollisionShape
        {
            NONE,
            POINT,
            COLLISION,
        };

        enum class DampingMode
        {
            STAND,
            CIRCLE,
        };

        struct ContactPointInfo
        {
            Vector2 pos = Vector2::Zero;
            float normalAngle = 0.0f;
        };

        struct CollisionInfo
        {
            bool isHit = false;
            CollisionShape shape = CollisionShape::NONE;
            ContactPointInfo closspoint{};
            Vector2 circlePos = Vector2::Zero;
            CollisionQuadrant quadrant = CollisionQuadrant::NONE;
            float blockAngle = 0.0f;
            bool  useHorizonUpdate = false;
            float horizonSpeed = 0.0f;
            float friction = 0.0f;
            bool  useDamping = false;
            float damping = 0.0f;
            DampingMode dampingMode = DampingMode::STAND;
        };

        struct VerticalMotionState
        {
            Vector2 velocity = Vector2::Zero;
            float mass = 1.0f;
            float mag = 1.0f;
            float finalNormalAngle = 0.0f;
            float vectorNum = 0.0f;
            float dt = 1.0f / 60.0f;
            float groundY = 0.0f;
            bool  clampToGround = true;
        };

        struct MotionState
        {
            Vector2 velocity = Vector2::Zero;
            float mass = 1.0f;
            float mag = 1.0f;
            bool enableGravity = true;
            bool integrateX = true;
            bool integrateY = true;
            float finalNormalAngle = 0.0f;
            float vectorNum = 0.0f;
            float groundY = 0.0f;
            bool  clampToGround = false;
        };

        float NormalizeDegree(float degree);
        float NormalizeRadian(float radian);
        float ConvertToDegree(float radian);
        float ConvertToRadian(float degree);
        float CalcRefrectAngle(float myAngleD, float nrmAngleD);
        void AddForce(Vector2& state, const Vector2& force);
        void FreeFall(VerticalMotionState& state);
        void CalcFinalNormalAngle(VerticalMotionState& state, const CollisionInfo& collision);
        void Repulsion(VerticalMotionState& state);
        void HorizonUpdate(VerticalMotionState& state, float speed, float friction);
        void DampingVector(VerticalMotionState& state, DampingMode mode, float damping);
        Vector2 UpdateRigidBodyPosition(VerticalMotionState& state, const Vector2& currentPosition, const CollisionInfo* collision = nullptr);
        Vector2 StepRigidBody(MotionState& state, const Vector2& currentPosition, float deltaTime, const CollisionInfo* collision = nullptr);

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

