#pragma once
// 次シーンのスナップショット（SRV）を最後にアルファ合成で全画面描画するだけのオブジェクト

#include "ObjectFile/Object.h" 
#include "Renderer.h"
#include <wrl/client.h>
#include <d3d11.h>

class SnapshotOverlay : public Object
{
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SRV;
    float m_Alpha = 0.0f;

public:
    SnapshotOverlay() = default;
    SnapshotOverlay(Camera& cam) : Object(cam), m_SRV(nullptr), m_Alpha(0.0f){	}
    ~SnapshotOverlay() override = default;

    void Initialize() override {}
    void Update() override {}
    void Draw() override
    {
        if (m_SRV) {
            Renderer::BlitSRVToBackbuffer(m_SRV.Get(), m_Alpha);
        }
    }
    void Finalize() override {}

    void SetSRV(ID3D11ShaderResourceView* srv) { m_SRV = srv; }
    void SetAlpha(float a) { m_Alpha = a; }

    ID3D11ShaderResourceView* GetSRV() const {return m_SRV.Get(); }
    float GetAlpha() const { return m_Alpha; }
};
