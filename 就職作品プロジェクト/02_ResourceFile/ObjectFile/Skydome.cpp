#include "Skydome.h"
#include "../Game.h"

Skydome::Skydome(Camera* cam) : Sphere(cam) {}

void Skydome::Initialize() {

    Sphere::Initialize();

    auto shaderMgr = GAME_MANAGER_SHADER;
    m_Shaders.clear();
    m_Shaders.emplace_back(shaderMgr->GetShader("VS_Unlit"));
    m_Shaders.emplace_back(shaderMgr->GetShader("PS_Unlit"));

    EnableSkyDome("space.png", 1000.0f);
}
