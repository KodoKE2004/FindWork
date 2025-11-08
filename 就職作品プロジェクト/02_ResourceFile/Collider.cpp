#include "Collider.h"

bool Collider::Collider2D::isHitSquareCircle(Square& square, Circle& circle)
{
    if ( !ColliderMore(square.GetTransform(), circle.GetTransform())){
        return false;
    }

    std::array<NVector3, 4> squareVertex = SettingVertex(square.GetTransform());
    std::array<NVector3, 4> circleVertex = SettingVertex(circle.GetTransform()); 

    return false;
}

bool Collider::Collider2D::isHitCircleCircle(Circle& circleA, Circle& circleB)
{
    if (!ColliderMore(circleA.GetTransform(), circleA.GetTransform())) {
        return false;
    }

    std::array<NVector3, 4> circleAVertex = SettingVertex(circleA.GetTransform()); 
    std::array<NVector3, 4> circleBVertex = SettingVertex(circleB.GetTransform()); 


    return false;
}

bool Collider::Collider2D::ColliderMore(Transform traA, Transform traB)
{
    // 対角線の長さを作成
    float diagonalA = CreateDiagonalLength(traA);
    float diagonalB = CreateDiagonalLength(traB);
    
    // 対角線の合計 = ヒット時の最大距離
    float distanceMax = diagonalA + diagonalB;
    
    NVector3 distanceVec =( traA.GetPos().x - traB.GetPos().x,
                            traA.GetPos().y - traB.GetPos().y,
                            0.0f);

    float distance = distanceVec.Length();
    
    if (distance < distanceMax) {
        return true;
    }

    return false;
}

std::array<NVector3, 4> Collider::Collider2D::SettingVertex(Transform transform)
{
    std::array<NVector3, 4> resVertex;


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
        resVertex[i] = NVector3(
            position.x + rotatedX,
            position.y + rotatedY,
            position.z
        );
    }

    return resVertex;
}

float Collider::Collider2D::CreateDiagonalLength(Transform traA)
{
    float halfWidthA  = traA.GetScale().x / 2.0f;
    float halfHeightA = traA.GetScale().y / 2.0f;
    float diagonalA = sqrtf(halfWidthA * halfWidthA + halfHeightA * halfHeightA);

    return diagonalA;
}
