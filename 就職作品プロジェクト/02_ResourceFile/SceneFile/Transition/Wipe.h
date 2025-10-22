#pragma once
#include "TransitionBase.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <algorithm>

class Wipe : public TransitionBase
{
public:
    enum class DIRECTION
    {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT,
        TOP_TO_BOTTOM,
        BOTTOM_TO_TOP
    };

    enum class PHASE
    {
        WIPW_IN,
        WIPE_OUT
    };

private:

    DIRECTION m_Dir = DIRECTION::LEFT_TO_RIGHT;
    float     m_Progress = 0.0f; // 0..1
    bool      m_Invert = false;
    NVector3  m_BaseScale{};
    NVector3  m_BasePos{};
    bool      m_BaseInitialized = false;
    float     m_UOffset = 0.0f;
    float     m_VOffset = 0.0f;
    float     m_UScale  = 1.0f;
    float     m_VScale  = 1.0f;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_SnapshotSRV; // ̃V[SRV

    void UpdateLayout();

public:
    Wipe(Camera* cam);
    ~Wipe() = default;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void SetDirection(DIRECTION d) { m_Dir = d; UpdateLayout(); }
    void SetProgress (float p);
    void SetInvert   (bool inv) { m_Invert = inv; UpdateLayout(); }
    void SetSnapshot (ID3D11ShaderResourceView* srv);
    void SetAlpha(float a) { m_Overlay.SetAlpha(a); } // 既存 SnapshotOverlay API を委譲

    static void GetViewportSize(ID3D11DeviceContext* ctx, UINT& w, UINT& h);
};

