#include "Collider.h"

bool Collider::Collider2D::isHitSquareCircle(Square& square, Circle& circle)
{
    std::array<Vector3, 4> squareVertex = SettingVertex(square.GetTransform());
    std::array<Vector3, 4> circleVertex = SettingVertex(circle.GetTransform()); // 未実装

    return false;
}

bool Collider::Collider2D::isHitCircleCircle(Circle circleA, Circle circleB)
{
    return false;
}

std::array<Vector3, 4> Collider::Collider2D::SettingVertex(Transform& transform)
{
    std::array<Vector3, 4> resVertex;


    NVector3 position = transform.GetPos();
    NVector3 scale    = transform.GetScale();
    NVector3 rotation = transform.GetRotate();

    float halfWidth  = scale.x / 2.0f;
    float halfHeight = scale.y / 2.0f;

    // 四つの頂点のローカル座標を計算
    std::array<NVector3, 4> localVertices = {
        NVector3(- halfWidth,   halfHeight, 0.0f), // 左上
        NVector3(  halfWidth,   halfHeight, 0.0f), // 右上
        NVector3(- halfWidth, - halfHeight, 0.0f), // 左下
        NVector3(  halfWidth, - halfHeight, 0.0f)  // 右下
    };

    // 回転行列の計算
    float cosR = cosf(rotation.z);
    float sinR = sinf(rotation.z);
    
    for (size_t i = 0; i < localVertices.size(); ++i)
    {
        // 回転の適用
        float rotatedX = localVertices[i].x * cosR - localVertices[i].y * sinR;
        float rotatedY = localVertices[i].x * sinR + localVertices[i].y * cosR;

        // ワールド座標への変換
        resVertex[i] = Vector3(
            position.x + rotatedX,
            position.y + rotatedY,
            position.z
        );
    }

    return resVertex;
}
