#pragma once
#include "Square.h"
#include "Application.h"

#include <memory>
#include <random>
#include <type_traits>
#include <vector>

class ParticleEmitter
{
private:
    struct Particle
    {
        std::unique_ptr<Square> quad;
        NVector3 velocity{ 0.0f, 0.0f, 0.0f };
        float lifeTime = 0.0f;
        float elapsed = 0.0f;
    };

    void Spawn(std::shared_ptr<Texture> texture,
               const NVector3& position,
               const NVector3& scale,
               float baseSpeed,
               float lifeTime
               );

    Camera& m_Camera;
    std::vector<Particle> m_Particles;

    std::mt19937 m_RandomEngine;
    std::uniform_real_distribution<float> m_AngleDist;
    std::uniform_real_distribution<float> m_SpeedScaleDist;
public :
    explicit ParticleEmitter(Camera& cam);
    ~ParticleEmitter() = default;
    void Clear();
    void Update(float deltaTime);
    void Draw();
};