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
    auto* device = Renderer::GetDevice();
    if (!device) return false;
    std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
    auto compile = [&](const D3D_SHADER_MACRO* defines, ComPtr<ID3DBlob>& blob) {
        ComPtr<ID3DBlob> errors;
        const HRESULT hr = D3DCompileFromFile(std::wstring(hlslName.begin(), hlslName.end()).c_str(),
            defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, "main", "vs_5_0", flags, 0,
            blob.GetAddressOf(), errors.GetAddressOf());
        if (errors) OutputDebugStringA(static_cast<const char*>(errors->GetBufferPointer()));
        return SUCCEEDED(hr);
    };
    ComPtr<ID3DBlob> bytecode;
    ComPtr<ID3D11VertexShader> normalVS, instanceVS;
    ComPtr<ID3D11InputLayout> normalLayout, instanceLayout;
    if (!compile(nullptr, bytecode)) return false;
    if (FAILED(device->CreateVertexShader(bytecode->GetBufferPointer(), bytecode->GetBufferSize(),
        nullptr, normalVS.GetAddressOf()))) return false;
    if (FAILED(device->CreateInputLayout(layout.data(), static_cast<UINT>(layout.size()),
        bytecode->GetBufferPointer(), bytecode->GetBufferSize(), normalLayout.GetAddressOf()))) return false;

    const auto filename = hlslName.substr(hlslName.find_last_of("/\\") + 1);
    const bool instanced = filename == "VS_Alpha.hlsl" || filename == "VS_Default.hlsl"
        || filename == "VS_Unlit.hlsl" || filename == "VS_Instansing2D.hlsl"
        || filename == "VS_Skydome.hlsl";
    if (instanced)
    {
        for (UINT i = 0; i < 4; ++i)
            layout.push_back({ "INSTANCEWORLD", i, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
                i * 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
        for (UINT i = 0; i < 4; ++i)
            layout.push_back({ "INSTANCENORMAL", i, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
                64 + i * 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
        layout.push_back({ "INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
            128, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
        layout.push_back({ "INSTANCEUV", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1,
            144, D3D11_INPUT_PER_INSTANCE_DATA, 1 });
        const D3D_SHADER_MACRO defines[] = { { "USE_INSTANCING", "1" }, { nullptr, nullptr } };
        ComPtr<ID3DBlob> instanceBytecode;
        if (!compile(defines, instanceBytecode)) return false;
        if (FAILED(device->CreateVertexShader(instanceBytecode->GetBufferPointer(), instanceBytecode->GetBufferSize(),
            nullptr, instanceVS.GetAddressOf()))) return false;
        if (FAILED(device->CreateInputLayout(layout.data(), static_cast<UINT>(layout.size()),
            instanceBytecode->GetBufferPointer(), instanceBytecode->GetBufferSize(), instanceLayout.GetAddressOf()))) return false;
    }
    // Commit only after BOTH variants succeed; a failed reload keeps working shaders.
    m_HlslName = std::move(hlslName);
    m_ShaderStage = ShaderStage::VS;
    m_Bytecode = bytecode;
    m_VertexShader = normalVS;
    m_InputLayout = normalLayout;
    m_InstancedShader = instanceVS;
    m_InstancedLayout = instanceLayout;
    return true;
}

void VertexShader::SetInstancedGPU()
{
    auto* context = Renderer::GetDeviceContext();
    context->VSSetShader(m_InstancedShader.Get(), nullptr, 0);
    context->IASetInputLayout(m_InstancedLayout.Get());
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

