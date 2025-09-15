#include "Skydome.h"

// Skydome behaves like a Sphere rendered from the inside as a sky dome.
// It sets unlit shaders and enables the sphere's sky rendering states by default.
Skydome::Skydome(Camera* cam) : Sphere(cam) {}

void Skydome::Initialize() {
    // Build sphere geometry using base implementation
    Sphere::Initialize();

    // Use simple unlit shader for the sky.
    SetShader("VS_Unlit", "PS_Unlit");

    // Enable sky-dome mode with default texture and radius.
    // Users may call EnableSkyDome again after creation to change texture or radius.
    EnableSkyDome("space.png", 1000.0f);
}
