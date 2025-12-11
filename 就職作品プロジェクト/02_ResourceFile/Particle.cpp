#include "Particle.h"

void ParticleEmitter::Emit(std::shared_ptr<Texture> tex,
    const NVector3& pos,
    uint32_t count)
{
    if (!tex || count == 0) return;

    for (uint32_t i = 0; i < count; i++)
    {
        float angle = m_AngleDist(m_RandomEngine);
        float vx = std::cos(angle) * m_BaseSpeed;
        float vy = std::sin(angle) * m_BaseSpeed;

        Particle p;
        p.quad = std::make_unique<Square>(m_Camera);
        p.quad->Initialize();
        p.quad->SetTexture(tex);
        p.quad->SetPos(pos);
        p.quad->SetScale(m_Scale, m_Scale, 1.0f);

        p.velocity = { vx, vy, 0.0f };
        p.lifeTime = m_LifeTime;
        p.elapsed = 0.0f;

        m_Particles.emplace_back(std::move(p));
    }
}

void ParticleEmitter::Update(float dt)
{
    for (auto& p : m_Particles)
    {
        p.elapsed += dt;
        p.quad->SetPos(p.quad->GetPos() + p.velocity * dt);
    }

    m_Particles.erase(
        std::remove_if(m_Particles.begin(), m_Particles.end(),
            [](const Particle& p) { return p.elapsed >= p.lifeTime; }),
        m_Particles.end());
}

void ParticleEmitter::Draw()
{
    for (auto& p : m_Particles)
        p.quad->Draw();
}

void ParticleEmitter::Clear()
{
    m_Particles.clear();
}