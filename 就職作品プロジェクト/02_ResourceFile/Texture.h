#pragma once
#include "Renderer.h"
#include <d3d11.h>
#include <SimpleMath.h>
#include <string>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class Texture
{
public:
    Texture();
    bool LoadFromFile(const std::string& path);
    bool LoadFromMemory(const uint8_t* data, size_t size, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
    void SetGPU(UINT slot = 0) const;
    UINT GetWidth()  const { return m_width; }
    UINT GetHeight() const { return m_height; }
    ID3D11ShaderResourceView* GetSRV() const { return m_srv.Get(); }

private:
    UINT m_width{ 0 };
    UINT m_height{ 0 };
    ComPtr<ID3D11Texture2D> m_texture;
    ComPtr<ID3D11ShaderResourceView> m_srv;
};