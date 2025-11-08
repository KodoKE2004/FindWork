#pragma once
#include <cmath>
#include <SimpleMath.h>
#include <array>
#include <vector>
#include "Transform.h"
#include "Circle.h"
#include "Square.h"

namespace Collider
{
    namespace Collider2D
    {
        bool isHitSquareCircle(Square& square , Circle& circle);
        bool isHitCircleCircle(Circle& circleA, Circle& circleB);
        
        /// <summary>
        /// 距離がhalfsizeより大きければfalseを返す
        /// 基準は対角線の半分の長さ
        /// </summary>
        /// <param name="objectA"></param>
        /// <param name="objectB"></param>
        /// <returns></returns>
        bool ColliderMore(NVector3& vecA,NVector3& vecB);

        /// <summary>
        /// 四つ角の頂点の位置をワールド座標の値で返す
        /// </summary>
        /// <param name="object"></param> Transform
        /// <returns></returns>
        std::array<NVector3, 4> SettingVertex(Transform transform);
    }
};

