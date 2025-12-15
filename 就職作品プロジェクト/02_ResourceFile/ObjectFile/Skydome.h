#pragma once
#include "Sphere.h"


class Skydome : public Sphere {
public:
    // コンストラクタ
    Skydome(Camera& cam);

    // メインループ処理
    void Initialize() override;

    void SetRotationSpeed(float yawSpeed) {SetSkyRotationSpeed(0.0f, yawSpeed, 0.0f); }
    void SetRotationSpeed(const NVector3& speed) {SetSkyRotationSpeed(speed); }

    void Spin(float spinX,float spinY, float spinZ);
    void Spin(const NVector3& spinVec3);
};
