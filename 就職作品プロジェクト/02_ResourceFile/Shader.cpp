#include "Shader.h"
#include "dx11helper.h"
#include "Renderer.h"
#include <d3d11.h>

BaseShader::~BaseShader() = default;

BaseShader* BaseShader::TryCreateShaderFromName()
{

	// 命名規則を遵守していたら、対応するシェーダークラスを生成
    switch (m_ShaderStage) { //  |              if               |  |     true         |  | false |
    case ShaderStage::VS: return (m_HlslName.rfind("VS_", 0) == 0) ? new VertexShader()  : nullptr;
    case ShaderStage::PS: return (m_HlslName.rfind("PS_", 0) == 0) ? new PixelShader()   : nullptr;
    case ShaderStage::GS: return (m_HlslName.rfind("GS_", 0) == 0) ? new GeometryShader(): nullptr;
    case ShaderStage::CS: return (m_HlslName.rfind("CS_", 0) == 0) ? new ComputeShader() : nullptr;
    default:              return nullptr;
    }
}

//---------------------------------
//	      VertexShaderクラス
//---------------------------------
bool VertexShader::Create(std::string hlslName)
{
    m_HlslName = std::move(hlslName);
    m_ShaderStage = ShaderStage::VS;

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT,     0,
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,     0,
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0,
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,        0,
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    1, DXGI_FORMAT_R32_UINT,            1,
        0,                          D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };

    auto* device = Renderer::GetDevice();
    ComPtr<ID3DBlob> vsBlob, err;

    // Blob作成
    HRESULT hr = D3DCompileFromFile(
        std::wstring(m_HlslName.begin(), m_HlslName.end()).c_str(),
        nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main" , "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
        0,
        vsBlob.GetAddressOf(),
        err.GetAddressOf());
    if (FAILED(hr)) {
        return false;
    }
    
    // 頂点シェーダー生成
    hr = device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        m_VertexShader.GetAddressOf());
    if(FAILED(hr))  return false;

    // 入力レイアウト生成
    hr = device->CreateInputLayout(
        layout,
        static_cast<UINT>(std::size(layout)),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        m_InputLayout.GetAddressOf());
    if(FAILED(hr))  return false;

    m_Bytecode = vsBlob;
    return true;

}

void VertexShader::SetGPU()
{
    auto* ctx = Renderer::GetDeviceContext();
    ctx->VSSetShader(m_VertexShader.Get(), nullptr, 0);
    ctx->IASetInputLayout(m_InputLayout.Get());
}



//=====
//			PixelShaderクラス
//=====
bool
PixelShader::Create(std::string hlslName)
{
    m_HlslName = std::move(hlslName);
    m_ShaderStage = ShaderStage::PS;

    auto* device = Renderer::GetDevice();
    bool res = CreatePixelShader(
        device,
        m_HlslName.c_str(),
        "main",
        "ps_5_0",
        m_PixelShader.GetAddressOf()
    );

    return res;
}

void PixelShader::SetGPU()
{
    auto* ctx = Renderer::GetDeviceContext();
    ctx->PSSetShader(m_PixelShader.Get(), nullptr, 0);
}



//=====
//			ComputeShaderクラス
//=====
bool GeometryShader::Create(std::string hlslName)
{
    m_HlslName = std::move(hlslName);
    m_ShaderStage = ShaderStage::GS;

    auto* device = Renderer::GetDevice();
    bool res =  CreateGeometryShader(
        device,
        m_HlslName.c_str(),
        "main",
        "gs_5_0",
        m_GeometryShader.GetAddressOf()
    );
    return res;
}

void GeometryShader::SetGPU()
{
    auto* ctx = Renderer::GetDeviceContext();
    ctx->GSSetShader(m_GeometryShader.Get(), nullptr, 0);
}


//=====
//			GeometoryShaderクラス
//=====
bool ComputeShader::Create(std::string hlslName)
{
    m_HlslName = std::move(hlslName);
    m_ShaderStage = ShaderStage::CS;

    auto* device = Renderer::GetDevice();
    bool res = CreateComputeShader(
        device,
        m_HlslName.c_str(),
        "main",
        "cs_5_0",
        m_ComputeShader.GetAddressOf()
    );

    return res;
}

void ComputeShader::SetGPU()
{
    auto* ctx = Renderer::GetDeviceContext();
    ctx->CSSetShader(m_ComputeShader.Get(), nullptr, 0);
}

