#include "RenderTarget.h"

bool RenderTarget::Create(ID3D11Device* dev, UINT width, UINT height, bool withDepth, bool srvMipGen)
{
    m_Width = width;
    m_Height = height;

    // Color Tex
    D3D11_TEXTURE2D_DESC td{};
    td.Width = m_Width;
    td.Height = m_Height;
    td.MipLevels = srvMipGen ? 0 : 1;              // 0 で全 Mip 可能（GenerateMips 用）
    td.ArraySize = 1;
    td.Format = m_ColorFormat;                     // _SRGB 指定も可
    td.SampleDesc.Count = 1;                       // MSAA なし（合成が楽）
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    td.MiscFlags = srvMipGen ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

    HRESULT hr = dev->CreateTexture2D(&td, nullptr, m_ColorTex.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = dev->CreateRenderTargetView(m_ColorTex.Get(), nullptr, m_Rtv.GetAddressOf());
    if (FAILED(hr)) return false;

    D3D11_SHADER_RESOURCE_VIEW_DESC sd{};
    sd.Format = td.Format;
    sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    sd.Texture2D.MipLevels = srvMipGen ? -1 : 1;   // -1 で全 Mip
    hr = dev->CreateShaderResourceView(m_ColorTex.Get(), srvMipGen ? &sd : nullptr, m_Srv.GetAddressOf());
    if (FAILED(hr)) return false;

    // Depth (optional)
    if (withDepth)
    {
        D3D11_TEXTURE2D_DESC dd = td;
        dd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        dd.MipLevels = 1;
        dd.MiscFlags = 0;

        hr = dev->CreateTexture2D(&dd, nullptr, m_DepthTex.GetAddressOf());
        if (FAILED(hr)) return false;

        hr = dev->CreateDepthStencilView(m_DepthTex.Get(), nullptr, m_Dsv.GetAddressOf());
        if (FAILED(hr)) return false;
    }

    // VP
    m_Vp.TopLeftX = 0.0f; m_Vp.TopLeftY = 0.0f;
    m_Vp.Width = static_cast<float>(m_Width);
    m_Vp.Height = static_cast<float>(m_Height);
    m_Vp.MinDepth = 0.0f; m_Vp.MaxDepth = 1.0f;
    return true;
}

void RenderTarget::Begin(ID3D11DeviceContext* ctx, const float clear[4], ID3D11DepthStencilView* clearDsv)
{
    m_PrevRTV.Reset();
    m_PrevDSV.Reset();
    m_PrevVp = {};
    m_HasPrevState = true;

    // SRV/RTV 競合の安全策: オフスクリーンへ切り替える直前に SRV を全解除する
    // (直前までSRVで使用していた場合の競合が原因ならこれで復帰する)
    ID3D11ShaderResourceView* nullSrvs[16] = {};
    ctx->PSSetShaderResources(0, 16, nullSrvs);
    ctx->VSSetShaderResources(0, 16, nullSrvs);
    ctx->GSSetShaderResources(0, 16, nullSrvs);
    ctx->CSSetShaderResources(0, 16, nullSrvs);

    ID3D11RenderTargetView* prevRtv = nullptr;
    ID3D11DepthStencilView* prevDsv = nullptr;
    ctx->OMGetRenderTargets(1, &prevRtv, &prevDsv);
    m_PrevRTV = prevRtv;
    m_PrevDSV = prevDsv;
    if (prevRtv) prevRtv->Release();
    if (prevDsv) prevDsv->Release();

    UINT vpCount = 1;
    ctx->RSGetViewports(&vpCount, &m_PrevVp);
    if (vpCount == 0)
    {
        m_PrevVp.TopLeftX = 0.0f; m_PrevVp.TopLeftY = 0.0f;
        m_PrevVp.Width = static_cast<float>(m_Width);
        m_PrevVp.Height = static_cast<float>(m_Height);
        m_PrevVp.MinDepth = 0.0f; m_PrevVp.MaxDepth = 1.0f;
    }

    ID3D11RenderTargetView* rtvs[] = { m_Rtv.Get() };
    ctx->OMSetRenderTargets(1, rtvs, m_Dsv.Get());
    ctx->RSSetViewports(1, &m_Vp);
    ctx->ClearRenderTargetView(m_Rtv.Get(), clear);
    if (m_Dsv) ctx->ClearDepthStencilView(m_Dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    if (clearDsv) ctx->ClearDepthStencilView(clearDsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderTarget::End(ID3D11DeviceContext* ctx)
{
    if (!m_HasPrevState) { return; }

    ID3D11RenderTargetView* rtvs[] = { m_PrevRTV.Get() };
    ctx->OMSetRenderTargets(1, rtvs, m_PrevDSV.Get());
    ctx->RSSetViewports(1, &m_PrevVp);
    m_HasPrevState = false;

}
