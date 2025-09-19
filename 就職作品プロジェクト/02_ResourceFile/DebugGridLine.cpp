#include "DebugGridLine.h"
#include "Debug.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

using namespace DirectX;
using namespace DirectX::SimpleMath;



void DebugGridLine::Initialize(ShaderManager& shaderManager)
{
    if (!shaderManager.HasShader("VS_DebugGrid"))
    {
        shaderManager.AddShader("VS_DebugGrid", ShaderStage::VS);
    }
    if (!shaderManager.HasShader("PS_Unlit"))
    {
        shaderManager.AddShader("PS_Unlit", ShaderStage::PS);
    }

    m_VertexShader = dynamic_cast<VertexShader*>(shaderManager.GetShader("VS_DebugGrid"));
    m_PixelShader = dynamic_cast<PixelShader*>(shaderManager.GetShader("PS_Unlit"));

    if (!m_VertexShader || !m_PixelShader)
    {
        Debug::Log("DebugGridLine: Failed to acquire shaders.");
        return;
    }

    constexpr Color kBaseColor(0.35f, 0.35f, 0.35f, 1.0f);
    NVector3 kNormal = {0.0f, 1.0f, 0.0f};

    const float min = -m_HalfExtent;
    const float max = m_HalfExtent;

    std::array<VERTEX_3D, 2> lineX = {
        VERTEX_3D{ NVector3(min, 0.0f, 0.0f), kNormal, kBaseColor, Vector2::Zero },
        VERTEX_3D{ NVector3(max, 0.0f, 0.0f), kNormal, kBaseColor, Vector2::Zero }
    };

    std::array<VERTEX_3D, 2> lineZ = {
        VERTEX_3D{ NVector3(0.0f, 0.0f, min), kNormal, kBaseColor, Vector2::Zero },
        VERTEX_3D{ NVector3(0.0f, 0.0f, max), kNormal, kBaseColor, Vector2::Zero }
    };

    m_LineX.Create(std::vector<VERTEX_3D>(lineX.begin(), lineX.end()));
    m_LineZ.Create(std::vector<VERTEX_3D>(lineZ.begin(), lineZ.end()));

    if (!CreateConstantBuffer(Renderer::GetDevice(), sizeof(GridParams), m_GridParamBuffer.GetAddressOf()))
    {
        Debug::Log("DebugGridLine: Failed to create constant buffer.");
        return;
    }

    const float total = (m_HalfExtent * 2.0f);
    const float steps = total / m_Spacing;
    m_InstanceCount = max(1u, static_cast<unsigned int>(std::floor(steps)) + 1u);

    std::vector<UINT> ids(m_InstanceCount);
    for (UINT i = 0; i < m_InstanceCount; ++i)
        ids[i] = i;

    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth    = static_cast<UINT> (ids.size() * sizeof(UINT));
    bd.Usage        = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags    = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init{ ids.data(),0, 0};
    Renderer::GetDevice()->CreateBuffer(&bd, &init, m_InstanceIDVB.GetAddressOf());
    
    std::vector<D3D11_INPUT_ELEMENT_DESC> elems = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,      0,  0, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,      0, 12, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,   0, 24, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,         0, 40, D3D11_INPUT_PER_VERTEX_DATA,   0 },
        { "TEXCOORD", 1, DXGI_FORMAT_R32_UINT,             1,  0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
    };

    Renderer::GetDevice()->CreateInputLayout(
        elems.data(), (UINT)elems.size(),
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
        m_GridInputLayout.GetAddressOf()
    );

    m_Initialized = true;
}

void DebugGridLine::Finalize()
{
    m_LineX = VertexBuffer<VERTEX_3D>();
    m_LineZ = VertexBuffer<VERTEX_3D>();
    m_GridParamBuffer.Reset();
    m_VertexShader = nullptr;
    m_PixelShader = nullptr;
    m_Initialized = false;
}

void DebugGridLine::UpdateGridParams(float axisFlag)
{
    GridParams params{};
    params.start = m_Start;
    params.spacing = m_Spacing;
    params.axis = axisFlag;

    auto* context = Renderer::GetDeviceContext();
    context->UpdateSubresource(m_GridParamBuffer.Get(), 0, nullptr, &params, 0, 0);
    ID3D11Buffer* buffer = m_GridParamBuffer.Get();
    context->VSSetConstantBuffers(6, 1, &buffer);
}

void DebugGridLine::DrawAxis(VertexBuffer<VERTEX_3D>& buffer, float axisFlag)
{
    UpdateGridParams(axisFlag);
    buffer.SetGPU();
    Renderer::GetDeviceContext()->DrawInstanced(2, m_InstanceCount, 0, 0);
}

void DebugGridLine::Draw()
{
    if (!m_Initialized || !m_Enabled)
    {
        return;
    }

    auto* context = Renderer::GetDeviceContext();

    if (!context || !Renderer::GetDevice())
    {
        return;
    }

    m_VertexShader->SetGPU();
    m_PixelShader->SetGPU();

    Matrix identity = Matrix::Identity;
    Renderer::SetWorldMatrix(&identity);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    DrawAxis(m_LineX, 0.0f);
    DrawAxis(m_LineZ, 1.0f);
}