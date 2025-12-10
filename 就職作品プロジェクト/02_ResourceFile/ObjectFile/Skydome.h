#pragma once
#include "Sphere.h"

// Skydome: specialized Sphere that automatically behaves as a sky dome
// Uses unlit shader and sphere's built-in sky rendering logic.
class Skydome : public Sphere {
public:
    // Forward to Sphere constructor
    Skydome(Camera& cam);

    // Initialize sphere geometry and enable sky-dome mode with default settings
    void Initialize() override;

    void SetRotationSpeed(float yawSpeed) {SetSkyRotationSpeed(0.0f, yawSpeed, 0.0f); }
    void SetRotationSpeed(const NVector3& speed) {SetSkyRotationSpeed(speed); }

    void Spin(float spinX,float spinY, float spinZ);
    void Spin(const NVector3& spinVec3);
};
