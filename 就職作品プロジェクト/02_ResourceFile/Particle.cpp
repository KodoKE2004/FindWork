#include "Particle.h"

ParticleEmitter::ParticleEmitter(Camera& cam) :
    m_Camera(cam),
    m_RandomEngine(std::random_device{}()),
    m_AngleDist(0.0f, DirectX::XM_2PI),
    m_SpeedScaleDist(0.6f, 1.0f)
{
    
}

void ParticleEmitter::Spawn(std::shared_ptr<Texture> texture, const NVector3& position, const NVector3& scale, float baseSpeed, float lifeTime)
{
    Particle particle;
    particle.quad = std::make_unique<Square>(m_Camera);
    particle.quad->Initialize();
    particle.quad->SetTexture(texture);
    particle.quad->SetPos(position);
    particle.quad->SetScale(scale);

    float angle = m_AngleDist(m_RandomEngine);
    float speed = baseSpeed * m_SpeedScaleDist(m_RandomEngine);
    particle.velocity = NVector3(std::cos(angle) * speed, std::sin(angle) * speed, 0.0f);

    particle.lifeTime = lifeTime;
    particle.elapsed = 0.0f;

    m_Particles.emplace_back(std::move(particle));
}

void ParticleEmitter::Clear()
{
    m_Particles.clear();
}

void ParticleEmitter::Update(float deltaTime)
{
    float dt = deltaTime;
    if (dt <= 0.0f)
    {
        return;
    }

    for (auto& particle : m_Particles)
    {
        particle.elapsed += dt;
        auto nextPos = particle.quad->GetPos() + particle.velocity * dt;
        particle.quad->SetPos(nextPos);
    }

    m_Particles.erase(
        std::remove_if(
            m_Particles.begin(),
            m_Particles.end(),
            [](const Particle& p)
            {
                return p.elapsed >= p.lifeTime;
            }),
        m_Particles.end());
}

void ParticleEmitter::Draw()
{
    for (auto& particle : m_Particles)
    {
        particle.quad->Draw();
    }
}

