#pragma once
#include "Square.h"
#include "Application.h"

#include <memory>
#include <random>
#include <type_traits>
#include <vector>
#include <cstdint>

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

    float m_BaseSpeed = 250.0f; 
    float m_LifeTime = 0.5f;    
    float m_Scale = 50.0f;      

    Camera& m_Camera;
    std::vector<Particle> m_Particles;

    std::mt19937 m_RandomEngine;
    std::uniform_real_distribution<float> m_AngleDist;
    std::uniform_real_distribution<float> m_SpeedScaleDist;

public :
    ParticleEmitter(Camera& cam)
        : m_Camera(cam),
        m_RandomEngine(std::random_device{}()),
        m_AngleDist(0.0f, DirectX::XM_2PI)
    {
    }

    void SetParams(float speed, float life, float scale)
    {
        m_BaseSpeed = speed;
        m_LifeTime = life;
        m_Scale = scale;
    }

    // 指定位置にテクスチャ`tex`で`count`個のパーティクルを発生させる
    void Emit(std::shared_ptr<Texture> tex, const NVector3& pos, uint32_t count);

    // デルタ時間でパーティクルの寿命と位置を更新する
    void Update(float dt);
    // 生存しているパーティクルを描画する
    void Draw();
    // 全パーティクルを削除して内部状態をリセットする
    void Clear();
};