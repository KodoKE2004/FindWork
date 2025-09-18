#include "Skydome.h"
#include "../Game.h"

Skydome::Skydome(Camera* cam) : Sphere(cam)
{
    m_Texture = nullptr;
}

void Skydome::Initialize() {

    Sphere::Initialize();

    SetShader("VS_Unlit","PS_Unlit");

}
