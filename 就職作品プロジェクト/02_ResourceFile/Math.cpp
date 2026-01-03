#include "Math.h"
#include <cmath>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <algorithm>

namespace Math
{
    namespace Collider2D
    {
        bool OverlapOnAxis(NVector3& axis, std::array<NVector3, 4>& vertsA, std::array<NVector3, 4>& vertsB)
        {
            // 無効軸はスキップ扱い
            if (axis.x == 0.0f && axis.y == 0.0f && axis.z == 0.0f)
            {
                return true;
            }

            const NVector3 n = axis.Normalize();

            float minA = vertsA[0].Dot(n);
            float maxA = minA;
            for (int i = 1; i < 4; ++i)
            {
                const float p = vertsA[i].Dot(n);
                if (p < minA) minA = p;
                if (p > maxA) maxA = p;
            }

            float minB = vertsB[0].Dot(n);
            float maxB = minB;
            for (int i = 1; i < 4; ++i)
            {
                const float p = vertsB[i].Dot(n);
                if (p < minB) minB = p;
                if (p > maxB) maxB = p;
            }

            // 区間が離れていなければ重なっている
            return !(maxA < minB || maxB < minA);
        }

        bool IsHitPoint(NVector3& pt, SquareInfo sq)
        {
            // 中心 → 点
            const float dx = pt.x - sq.pos.x;
            const float dy = pt.y - sq.pos.y;

            // ローカル軸方向の距離
            const float distX = dx * sq.axisX.x + dy * sq.axisX.y;
            const float distY = dx * sq.axisY.x + dy * sq.axisY.y;

            return (std::fabs(distX) <= sq.halfW) &&
                (std::fabs(distY) <= sq.halfH);
        }

        bool isHitSquareCircle(Square& square, Circle& circle)
        {
            if (!ColliderMore(square.GetTransform(), circle.GetTransform())){
                return false;
            }

            const SquareInfo sq = SettingVertexInfo(square.GetTransform());
            const Transform  ct = circle.GetTransform();
            const NVector3   cp = ct.GetPos();

            const float r = ct.GetScale().x * 0.5f; // 半径の決め方は必要に応じて調整

            // Circle中心 → Square中心 をローカル座標へ
            const float dx = cp.x - sq.pos.x;
            const float dy = cp.y - sq.pos.y;

            const float localX = dx * sq.axisX.x + dy * sq.axisX.y;
            const float localY = dx * sq.axisY.x + dy * sq.axisY.y;

            // 四角形内にClampされた最近接点
            const float clampedX = max(-sq.halfW, min(localX, sq.halfW));
            const float clampedY = max(-sq.halfH, min(localY, sq.halfH));

            // 最近接点との距離
            const float vx = localX - clampedX;
            const float vy = localY - clampedY;
            const float distSq = vx * vx + vy * vy;

            return distSq <= (r * r);
        }

        bool isHitCircleCircle(Circle& circleA, Circle& circleB)
        {
            const Transform traA = circleA.GetTransform();
            const Transform traB = circleB.GetTransform();

            const NVector3 posA = traA.GetPos();
            const NVector3 posB = traB.GetPos();

            const float dx = posA.x - posB.x;
            const float dy = posA.y - posB.y;
            const float disttance = dx * dx + dy * dy;

            // 半径は scale.x の半分を利用（必要なら Circle 側の Getter に変更）
            const float radiusA = traA.GetScale().x * 0.5f;
            const float radiusB = traB.GetScale().x * 0.5f;

            // 衝突判定用の距離
            const float distanceMax = radiusA + radiusB;

            return disttance <= (distanceMax * distanceMax);
        }

        bool isHitSquareSquare(Square& squareA, Square& squareB)
        {
            // 粗判定：対角線距離
            if (!ColliderMore(squareA.GetTransform(), squareB.GetTransform()))
            {
                return false;
            }

            // 頂点
            auto vertsA = SettingVertex(squareA.GetTransform());
            auto vertsB = SettingVertex(squareB.GetTransform());

            // 軽量情報
            const SquareInfo infoA = SettingVertexInfo(squareA.GetTransform());
            const SquareInfo infoB = SettingVertexInfo(squareB.GetTransform());

            // 1) A の頂点が B の内側？
            for (int i = 0; i < 4; ++i)
            {
                if (IsHitPoint(vertsA[i], infoB))
                {
                    return true;
                }
            }

            // 2) B の頂点が A の内側？
            for (int i = 0; i < 4; ++i)
            {
                if (IsHitPoint(vertsB[i], infoA))
                {
                    return true;
                }
            }

            // 3) 頂点が互いに内側でない場合:
            //    辺同士クロスの可能性を SAT でチェック
            NVector3 axes[4] = {
                vertsA[1] - vertsA[0], // A の一辺
                vertsA[3] - vertsA[0], // A のもう一辺
                vertsB[1] - vertsB[0], // B の一辺
                vertsB[3] - vertsB[0]  // B のもう一辺
            };

            for (int i = 0; i < 4; ++i)
            {
                if (!OverlapOnAxis(axes[i], vertsA, vertsB))
                {
                    // この軸で分離 → 衝突なし
                    return false;
                }
            }

            // どの軸でも分離していない → 衝突
            return true;
        }

        SquareInfo SettingVertexInfo(Transform transform)
        {
            SquareInfo info;
        
            info.pos = transform.GetPos();
            const NVector3 sca = transform.GetScale();
            info.halfW = sca.x * 0.5f;
            info.halfH = sca.y * 0.5f;
        
            // rot.z をラジアン想定
            const NVector3 rot = transform.GetRotate();
            const float c = std::cos(rot.z);
            const float s = std::sin(rot.z);
        
            // 回転四角形のローカル軸
            info.axisX = NVector3( c, s, 0.0f);      // 右
            info.axisY = NVector3(-s, c, 0.0f);     // 上
        
            return info;
        }
        
        float CreateDiagonalLength(Transform tra)
        {
            const float halfW = tra.GetScale().x * 0.5f;
            const float halfH = tra.GetScale().y * 0.5f;
            return std::sqrt(halfW * halfW + halfH * halfH);
        }
        
        bool ColliderMore(Transform traA, Transform traB)
        {
            const float distMax =
                CreateDiagonalLength(traA) +
                CreateDiagonalLength(traB);
        
            NVector3 diff(
                traA.GetPos().x - traB.GetPos().x,
                traA.GetPos().y - traB.GetPos().y,
                0.0f
            );
        
            const float dist = diff.Length();
            return dist < distMax;
        }
        
        std::array<NVector3, 4> SettingVertex(Transform transform)
        {
            std::array<NVector3, 4> res{};
        
            const NVector3 pos = transform.GetPos();
            const NVector3 sca = transform.GetScale();
            const NVector3 rot = transform.GetRotate();
        
            const float halfW = sca.x * 0.5f;
            const float halfH = sca.y * 0.5f;
        
            std::array<NVector3, 4> local = {
                NVector3(-halfW,  halfH, 0.0f), // 左上
                NVector3(halfW,  halfH, 0.0f), // 右上
                NVector3(-halfW, -halfH, 0.0f), // 左下
                NVector3(halfW, -halfH, 0.0f)  // 右下
            };
        
            const float c = std::cos(rot.z);
            const float s = std::sin(rot.z);
        
            for (int i = 0; i < 4; ++i)
            {
                const float rx = local[i].x * c - local[i].y * s;
                const float ry = local[i].x * s + local[i].y * c;
        
                res[i] = NVector3(
                    pos.x + rx,
                    pos.y + ry,
                    pos.z
                );
            }
        
            return res;
        }
    };

    namespace Physics
    {
        float UpdateVerticalPosition(VerticalMotionState& state, float currentPosY, float deltaTime)
        {
            if (deltaTime <= 0.0f)
            {
                return currentPosY;
            }

            const float gravityForce = state.gravity * state.weight * deltaTime;
            state.velocity -= gravityForce;

            state.terminalVelocity = - abs(state.terminalVelocity);
            const float minVelocity = state.terminalVelocity;
            state.velocity = max(state.velocity, minVelocity);

            float nextPosition = currentPosY + state.velocity * deltaTime;
            if (nextPosition < state.groundY)
            {
                nextPosition = state.groundY;
                state.velocity = 0.0f;
            }

            return nextPosition;
        }
    }


    namespace Easing
    {
        float EaseInSine(float p)
        {
            constexpr float PI = 3.14159265358979323846f;
            return 1.0f - std::cosf((p * PI) * 0.5f);
        }

        float EaseOutSine(float p)
        {
            constexpr float PI = 3.14159265358979323846f;
            return std::sinf((p * PI) * 0.5f);
        }

        float EaseInOutSine(float p)
        {
            constexpr float PI = 3.14159265358979323846f;
            return -0.5f * (std::cosf(PI * p) - 1.0f);
        }

        float EaseInQuad(float p)
        {
            return p * p;
        }

        float EaseOutQuad(float p)
        {
            const float inv = 1.0f - p;
            return 1.0f - inv * inv;
        }

        float EaseInOutQuad(float p)
        {
            if (p < 0.5f)
            {
                return 2.0f * p * p;
            }
            const float inv = -2.0f * p + 2.0f;
            return 1.0f - (inv * inv) * 0.5f;
        }

        float EaseInCubic(float p)
        {
            return p * p * p;
        }

        float EaseOutCubic(float p)
        {
            const float inv = 1.0f - p;
            return 1.0f - inv * inv * inv;
        }

        float EaseInOutCubic(float p)
        {
            if (p < 0.5f)
            {
                return 4.0f * p * p * p;
            }
            const float inv = -2.0f * p + 2.0f;
            return 1.0f - (inv * inv * inv) * 0.5f;
        }

        float EaseInQuart(float p)
        {
            const float p2 = p * p;
            return p2 * p2;
        }

        float EaseOutQuart(float p)
        {
            const float inv = 1.0f - p;
            const float inv2 = inv * inv;
            return 1.0f - inv2 * inv2;
        }

        float EaseInOutQuart(float p)
        {
            if (p < 0.5f)
            {
                const float p2 = p * p;
                return 8.0f * p2 * p2;
            }
            const float inv = -2.0f * p + 2.0f;
            const float inv2 = inv * inv;
            return 1.0f - (inv2 * inv2) * 0.5f;
        }

        float EaseInQuint(float p)
        {
            const float p2 = p * p;
            return p2 * p2 * p;
        }

        float EaseOutQuint(float p)
        {
            const float inv = 1.0f - p;
            const float inv2 = inv * inv;
            return 1.0f - inv2 * inv2 * inv;
        }

        float EaseInOutQuint(float p)
        {
            if (p < 0.5f)
            {
                const float p2 = p * p;
                return 16.0f * p2 * p2 * p;
            }
            const float inv = -2.0f * p + 2.0f;
            const float inv2 = inv * inv;
            return 1.0f - (inv2 * inv2 * inv) * 0.5f;
        }

        float EaseInExpo(float p)
        {
            if (p <= 0.0f)
            {
                return 0.0f;
            }
            return std::powf(2.0f, 10.0f * p - 10.0f);
        }

        float EaseOutExpo(float p)
        {
            if (p >= 1.0f)
            {
                return 1.0f;
            }
            return 1.0f - std::powf(2.0f, -10.0f * p);
        }

        float EaseInOutExpo(float p)
        {
            if (p <= 0.0f)
            {
                return 0.0f;
            }
            if (p >= 1.0f)
            {
                return 1.0f;
            }
            if (p < 0.5f)
            {
                return 0.5f * std::powf(2.0f, 20.0f * p - 10.0f);
            }
            return 1.0f - 0.5f * std::powf(2.0f, -20.0f * p + 10.0f);
        }

        float EaseInCirc(float p)
        {
            return 1.0f - std::sqrtf(1.0f - p * p);
        }

        float EaseOutCirc(float p)
        {
            const float inv = p - 1.0f;
            return std::sqrtf(1.0f - inv * inv);
        }

        float EaseInOutCirc(float p)
        {
            if (p < 0.5f)
            {
                const float t = 2.0f * p;
                return 0.5f * (1.0f - std::sqrt(1.0f - t * t));
            }
            const float t = -2.0f * p + 2.0f;
            return 0.5f * (std::sqrtf(1.0f - t * t) + 1.0f);
        }

        float EaseInBack(float p)
        {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            return c3 * p * p * p - c1 * p * p;
        }

        float EaseOutBack(float p)
        {
            const float c1 = 1.70158f;
            const float c3 = c1 + 1.0f;
            const float t = p - 1.0f;
            return 1.0f + c3 * t * t * t + c1 * t * t;
        }

        float EaseInOutBack(float p)
        {
            const float c1 = 1.70158f;
            const float c2 = c1 * 1.525f;
            if (p < 0.5f)
            {
                const float t = 2.0f * p;
                return 0.5f * (t * t * ((c2 + 1.0f) * t - c2));
            }
            const float t = 2.0f * p - 2.0f;
            return 0.5f * (t * t * ((c2 + 1.0f) * t + c2) + 2.0f);
        }

        float EaseInElastic(float p)
        {
            constexpr float PI = 3.14159265358979323846f;
            if (p <= 0.0f)
            {
                return 0.0f;
            }
            if (p >= 1.0f)
            {
                return 1.0f;
            }
            const float c4 = (2.0f * PI) / 3.0f;
            return -std::powf(2.0f, 10.0f * p - 10.0f) * std::sinf((p * 10.0f - 10.75f) * c4);
        }

        float EaseOutElastic(float p)
        {
            constexpr float PI = 3.14159265358979323846f;
            if (p <= 0.0f)
            {
                return 0.0f;
            }
            if (p >= 1.0f)
            {
                return 1.0f;
            }
            const float c4 = (2.0f * PI) / 3.0f;
            return std::powf(2.0f, -10.0f * p) * std::sinf((p * 10.0f - 0.75f) * c4) + 1.0f;
        }

        float EaseInOutElastic(float p)
        {
            constexpr float PI = 3.14159265358979323846f;
            if (p <= 0.0f)
            {
                return 0.0f;
            }
            if (p >= 1.0f)
            {
                return 1.0f;
            }
            const float c5 = (2.0f * PI) / 4.5f;
            if (p < 0.5f)
            {
                return -0.5f * std::powf(2.0f, 20.0f * p - 10.0f) * std::sinf((p * 20.0f - 11.125f) * c5);
            }
            return std::powf(2.0f, -20.0f * p + 10.0f) * std::sinf((p * 20.0f - 11.125f) * c5) * 0.5f + 1.0f;
        }

        float EaseInBounce(float p)
        {
            return 1.0f - EaseOutBounce(1.0f - p);
        }

        float EaseOutBounce(float p)
        {
            const float n1 = 7.5625f;
            const float d1 = 2.75f;
            if (p < 1.0f / d1)
            {
                return n1 * p * p;
            }
            else if (p < 2.0f / d1)
            {
                const float t = p - 1.5f / d1;
                return n1 * t * t + 0.75f;
            }
            else if (p < 2.5f / d1)
            {
                const float t = p - 2.25f / d1;
                return n1 * t * t + 0.9375f;
            }
            else
            {
                const float t = p - 2.625f / d1;
                return n1 * t * t + 0.984375f;
            }
        }

        float EaseInOutBounce(float p)
        {
            if (p < 0.5f)
            {
                return (1.0f - EaseOutBounce(1.0f - 2.0f * p)) * 0.5f;
            }
            return (1.0f + EaseOutBounce(2.0f * p - 1.0f)) * 0.5f;
        }
    }

};
