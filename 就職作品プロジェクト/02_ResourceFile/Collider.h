#pragma once
#include <cmath>
#include <SimpleMath.h>
#include <array>

#include "Transform.h"
#include "Circle.h"
#include "Square.h"


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
        float EaseInQuad(float t, float b, float c, float d);
        float EaseOutQuad(float t, float b, float c, float d);
        float EaseInOutQuad(float t, float b, float c, float d);


    };
};

