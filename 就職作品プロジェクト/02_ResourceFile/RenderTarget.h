#pragma once
// オフスクリーン描画用の薄いラッパ（DX11）
// Color Tex + RTV + SRV (+ 任意で DSV) をまとめて管理します。

#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class RenderTarget
{
private:
    UINT m_Width = 0;
    UINT m_Height = 0;
    DXGI_FORMAT m_ColorFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    ComPtr<ID3D11Texture2D>          m_ColorTex;
    ComPtr<ID3D11RenderTargetView>   m_Rtv;
    ComPtr<ID3D11ShaderResourceView> m_Srv;
    ComPtr<ID3D11Texture2D>          m_DepthTex;
    ComPtr<ID3D11DepthStencilView>   m_Dsv;
    ComPtr<ID3D11RenderTargetView>   m_PrevRTV;
    ComPtr<ID3D11DepthStencilView>   m_PrevDSV;
    D3D11_VIEWPORT                   m_Vp{};
    D3D11_VIEWPORT                   m_PrevVp{};
    bool                             m_HasPrevState = false;

public:
    RenderTarget() = default;

    // color + SRV (+ optional depth)
    bool Create(ID3D11Device* dev, UINT width, UINT height, bool withDepth, bool srvMipGen = false);

    // bind & clear
    void Begin(ID3D11DeviceContext* ctx, const float clear[4], ID3D11DepthStencilView* clearDsv = nullptr);

    void End(ID3D11DeviceContext* ctx);

    // accessors
    ID3D11RenderTargetView* GetRTV() const { return m_Rtv.Get(); }
    ID3D11ShaderResourceView* GetSRV() const { return m_Srv.Get(); }
    ID3D11DepthStencilView* GetDSV() const { return m_Dsv.Get(); }
    const D3D11_VIEWPORT& GetViewport() const { return m_Vp; }

    // sRGB 運用なら DXGI_FORMAT_R8G8B8A8_UNORM_SRGB を指定して Create 前に呼ぶ
    void SetFormat(DXGI_FORMAT fmt) { m_ColorFormat = fmt; }

};