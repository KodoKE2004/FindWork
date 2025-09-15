#pragma once
#include "Sphere.h"

// Skydome: specialized Sphere that automatically behaves as a sky dome
// Uses unlit shader and sphere's built-in sky rendering logic.
class Skydome : public Sphere {
public:
    // Forward to Sphere constructor
    Skydome(Camera* cam);

    // Initialize sphere geometry and enable sky-dome mode with default settings
    void Initialize() override;
};
