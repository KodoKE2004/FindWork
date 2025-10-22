#include "Wipe.h"

Wipe::Wipe(Camera* cam) : TransitionBase(cam)
{

}

void Wipe::Initialize()
{
    ID3D11Device*        device = Renderer::GetDevice();            // 環境に合わせて
    ID3D11DeviceContext* ctx    = Renderer::GetDeviceContext();     // 環境に合わせて

    // いまのRSを保存
    ctx->RSGetState(m_RSBackup.ReleaseAndGetAddressOf());

    // バックアップのDescをベースに Scissor を有効にしたRSを作る
    D3D11_RASTERIZER_DESC desc{};
    if (m_RSBackup)
    {
        m_RSBackup->GetDesc(&desc);
    }
    else
    {
        // もしRSがnullなら、一般的な既定値から作る
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthBias = 0;
        desc.DepthBiasClamp = 0.0f;
        desc.SlopeScaledDepthBias = 0.0f;
        desc.DepthClipEnable = TRUE;
        desc.ScissorEnable = FALSE; // ここは後でTRUEに
        desc.MultisampleEnable = FALSE;
        desc.AntialiasedLineEnable = FALSE;
    }

    desc.ScissorEnable = TRUE;
    device->CreateRasterizerState(&desc, m_RSScissor.ReleaseAndGetAddressOf());
}

void Wipe::Update()
{
}

void Wipe::Draw()
{
    ID3D11DeviceContext* ctx = Renderer::GetDeviceContext();

    // ビューポートサイズを取得
    UINT W=0, H=0;
    GetViewportSize(ctx, W, H);
    if (W == 0 || H == 0) return;

    // 進行方向とprogressからシザー矩形を計算
    D3D11_RECT rc{};
    float p = m_Invert ? (1.0f - m_Progress) : m_Progress;
    p = (p < 0 ? 0 : (p > 1 ? 1 : p));

    switch (m_Dir)
    {
    case DIRECTION::LEFT_TO_RIGHT:
        rc.left = 0; rc.top = 0;
        rc.right = static_cast<LONG>(W * p);
        rc.bottom = static_cast<LONG>(H);
        break;

    case DIRECTION::RIGHT_TO_LEFT:
        rc.left = static_cast<LONG>(W * (1.0f - p)); rc.top = 0;
        rc.right = static_cast<LONG>(W);
        rc.bottom = static_cast<LONG>(H);
        break;

    case DIRECTION::TOP_TO_BOTTOM:
        rc.left = 0; rc.top = 0;
        rc.right = static_cast<LONG>(W);
        rc.bottom = static_cast<LONG>(H * p);
        break;

    case DIRECTION::BOTTOM_TO_TOP:
        rc.left = 0; rc.top = static_cast<LONG>(H * (1.0f - p));
        rc.right = static_cast<LONG>(W);
        rc.bottom = static_cast<LONG>(H);
        break;
    }

    // RS差し替え & シザー設定
    ComPtr<ID3D11RasterizerState> rsOrig;
    ctx->RSGetState(rsOrig.ReleaseAndGetAddressOf());
    ctx->RSSetState(m_RSScissor.Get());
    ctx->RSSetScissorRects(1, &rc);

    // この呼び出しは「全画面ブリット」。シザーで必要領域だけ描かれる
    SnapshotOverlay::Draw(); // ← m_SRV を使って alpha合成（あなたの既存の動作）:contentReference[oaicite:1]{index=1}

    // 復帰
    ctx->RSSetScissorRects(0, nullptr);
    ctx->RSSetState(rsOrig.Get());
}

void Wipe::Finalize()
{
}

void Wipe::GetViewportSize(ID3D11DeviceContext* ctx, UINT& w, UINT& h)
{
    D3D11_VIEWPORT vp{};
    UINT count = 1;
    ctx->RSGetViewports(&count, &vp);
    w = static_cast<UINT>(vp.Width);
    h = static_cast<UINT>(vp.Height);
}
