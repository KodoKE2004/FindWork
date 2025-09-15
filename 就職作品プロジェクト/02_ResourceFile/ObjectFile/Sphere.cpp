#include "Sphere.h"
#include "../Game.h"
#include <SimpleMath.h>
#include <vector>
#include <cmath>

using namespace DirectX::SimpleMath;

Sphere::Sphere(Camera* cam) : Object(cam)
{
    
}

void Sphere::Initialize()
{
    m_Position = NVector3(0.0f,0.0f,0.0f);
    m_Rotation = NVector3(0.0f,0.0f,0.0f);
    m_Scale    = NVector3(1.0f,1.0f,1.0f);

    std::vector<VERTEX_3D> vertices;
    std::vector<unsigned int> indices;

    for (int stack = 0; stack <= m_Stack; ++stack)
    {
        float phi = DirectX::XM_PI *( static_cast<float>(stack) / m_Stack);
        float y = std::cosf(phi);
        float r = std::sinf(phi);
        for (int sline = 0; sline <= m_Sline; ++sline)
        {
            float theta = DirectX::XM_2PI * static_cast<float>(sline) / m_Sline;
            float x = r * std::cosf(theta);
            float z = r * std::sinf(theta);

            VERTEX_3D v{};
            v.position = NVector3(x * m_Radius, y * m_Radius, z * m_Radius);

            NVector3 n(x, y, z);
            n.Normalize();
            v.normal = n;

            v.color = Color(1.0f, 1.0f, 1.0f, 1.0f);
            v.uv    = Vector2(static_cast<float>(sline) / m_Sline,
                              static_cast<float>(stack) / m_Stack);
            vertices.emplace_back(v);
           
            if (stack < m_Stack && sline < m_Sline)
            {
                const unsigned int first  =  stack * (m_Sline + 1) + sline;
                const unsigned int second = (stack + 1) * (m_Sline + 1) + sline;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }

        }
    }

    m_IndexCount = static_cast<unsigned int>(indices.size());

    m_VertexBuffer.Create(vertices);
    m_IndexBuffer.Create(indices);

    auto shaderMgr = GAME_MANAGER_SHADER;
    m_Shaders.emplace_back(shaderMgr->GetShader("VS_Default"));
    m_Shaders.emplace_back(shaderMgr->GetShader("PS_Default"));
    
    if (m_Texture == nullptr)
    {
        m_Texture = Game::GetInstance().GetTextureManager()->GetTexture("space.png");
    }

    m_Camera->SetCamera(CAMERA_3D);
}

void Sphere::Update()
{
    if (m_IsSky)
    {
        // Sky dome follows camera position
        auto camPos = m_Camera->GetPosition();
        m_Position = NVector3(camPos.x, camPos.y, camPos.z);
        m_Rotation = NVector3(GetRotation().x + 2.0f, GetRotation().y, GetRotation().z);
    }
 
}

void Sphere::Draw()
{
    if (m_IsSky)
    {
        DrawAsSky();
    }
    else
    {
        DrawAsMesh();
    }
}

void Sphere::Finalize()
{
    m_Texture = nullptr;
    m_IndexCount = 0;
    m_Shaders.clear();
}


void Sphere::DisableSkyDome()
{
    m_IsSky = false;
}

void Sphere::DrawAsSky()
{
    ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();
    ID3D11RasterizerState* prevRS = nullptr;
    devicecontext->RSGetState(&prevRS);
    ID3D11DepthStencilState* prevDSS = nullptr;
    UINT prevRef = 0;
    devicecontext->OMGetDepthStencilState(&prevDSS, &prevRef);

    // Set sky-specific states
    devicecontext->RSSetState(m_RS_CullFront.Get());
    devicecontext->OMSetDepthStencilState(m_DSS_NoWrite_Lequal.Get(), 0);

    // Build world matrix centered on camera
    auto camPos = m_Camera->GetPosition();
    Matrix s = Matrix::CreateScale(m_SkyRadius);
    Matrix t = Matrix::CreateTranslation(camPos.x, camPos.y, camPos.z);
    Matrix world = s * t;
    Renderer::SetWorldMatrix(&world);

    devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (auto shader : m_Shaders)
    {
        shader->SetGPU();
    }

    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();
    if (m_Texture != nullptr)
    {
        m_Texture->SetGPU();
    }
    m_Camera->SetCamera(CAMERA_3D);

    devicecontext->DrawIndexed(m_IndexCount, 0, 0);

    // Restore previous states
    devicecontext->RSSetState(prevRS);
    devicecontext->OMSetDepthStencilState(prevDSS, prevRef);
    if (prevRS) prevRS->Release();
    if (prevDSS) prevDSS->Release();
}

void Sphere::DrawAsMesh()
{
    Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
    Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
    Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);
    Matrix world = s * r * t;
    Renderer::SetWorldMatrix(&world);

    ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();
    devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (auto shader : m_Shaders)
    {
        shader->SetGPU();
    }

    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();
    if (m_Texture != nullptr)
    {
        m_Texture->SetGPU();
    }
    m_Camera->SetCamera(CAMERA_3D);

    devicecontext->DrawIndexed(m_IndexCount, 0, 0);
}


