#pragma once
#include "TransitionBase.h"
#include "SnapshotOverlay.h"
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

    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RSBackup;   // 元のRS
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RSScissor;  // Scissor有効RS
    DIRECTION m_Dir = DIRECTION::LEFT_TO_RIGHT;
    float     m_Progress = 0.0f; // 0..1
    bool      m_Invert = false;
    SnapshotOverlay m_Overlay; // オーバーレイ描画用（SRVとAlpha管理）

public:
    Wipe(Camera* cam);
    ~Wipe() = default;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void SetDirection(DIRECTION d) { m_Dir = d; }
    void SetProgress (float p) { m_Progress = std::clamp(p, 0.0f, 1.0f); }
    void SetInvert   (bool inv) { m_Invert = inv; }
    void SetPhase    (PHASE) { /* 必要なら保持。今回は m_Progress の向きだけで足りる */ }
    void SetSRV      (ID3D11ShaderResourceView* srv) { m_Overlay.SetSRV(srv); }  // ← Overlayへ転送
    void SetAlpha(float a) { m_Overlay.SetAlpha(a); } // 既存 SnapshotOverlay API を委譲

    static void GetViewportSize(ID3D11DeviceContext* ctx, UINT& w, UINT& h);
};

