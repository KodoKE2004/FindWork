#include "TextureDX.h"
#include "DirectXTex.h"
#ifdef _DEBUG
    #include<iostream>
#endif
Texture::Texture() = default;

bool Texture::LoadFromFile(const std::string& path)
{
    DirectX::TexMetadata meta{};
    DirectX::ScratchImage img{};

    wchar_t wpath[MAX_PATH];
    MultiByteToWideChar(0, 0, path.c_str(), -1, wpath, MAX_PATH);

    HRESULT hr = S_OK;
    if (path.find(".tga") != std::string::npos)
        hr = DirectX::LoadFromTGAFile(wpath, &meta, img);
    else
        hr = DirectX::LoadFromWICFile(wpath, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &meta, img);
    if (FAILED(hr)) return false;

    hr = CreateShaderResourceView(Renderer::GetDevice(), img.GetImages(), img.GetImageCount(), meta, &m_srv);
    if (FAILED(hr)) return false;

    m_width = static_cast<UINT>(meta.width);
    m_height = static_cast<UINT>(meta.height);

    std::cout << "テクスチャ読み取り成功" << std::endl;
    return true;
}

bool Texture::LoadFromMemory(const uint8_t* data, size_t size, DXGI_FORMAT format)
{
    DirectX::ScratchImage img{};
    HRESULT hr = DirectX::LoadFromWICMemory(data, size, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, nullptr, img);
    if (FAILED(hr)) return false;

    DirectX::TexMetadata meta = img.GetMetadata();
    meta.format = format;

    hr = CreateShaderResourceView(Renderer::GetDevice(), img.GetImages(), img.GetImageCount(), meta, &m_srv);
    if (FAILED(hr)) return false;

    m_width = static_cast<UINT>(meta.width);
    m_height = static_cast<UINT>(meta.height);
    return true;
}

void Texture::SetGPU(UINT slot) const
{
    ID3D11DeviceContext* ctx = Renderer::GetDeviceContext();
    ID3D11ShaderResourceView* view = m_srv.Get();
    ctx->PSSetShaderResources(slot, 1, &view);
}