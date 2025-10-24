// SceneFile/Transition/Wipe.cpp
#include "Wipe.h"
#include "../../Renderer.h"
#include "../../main.h"  // SCREEN_WIDTH/HEIGHT 等があるなら
#include <vector>
#include <algorithm>

using Microsoft::WRL::ComPtr;

void Wipe::Initialize()
{
    // Fade と同様に全画面四角の頂点を用意（汎用 Quad）
    std::vector<VERTEX_3D> vertices(4);
    vertices[0].position = NVector3(-0.5f, 0.5f, 0.0f);
    vertices[1].position = NVector3(0.5f, 0.5f, 0.0f);
    vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
    vertices[3].position = NVector3(0.5f, -0.5f, 0.0f);
    for (auto& v : vertices) {
        v.color = Color(1, 1, 1, 1);
    }
    vertices[0].uv = Vector2(0, 0);
    vertices[1].uv = Vector2(1, 0);
    vertices[2].uv = Vector2(0, 1);
    vertices[3].uv = Vector2(1, 1);
    m_VertexBuffer.Create(vertices);

    std::vector<unsigned> indices{ 0,1,2,3 };
    m_IndexBuffer.Create(indices);

    // 専用シェーダに差し替え（Game::Initialize で登録しておく）
    SetShader("VS_Wipe", "PS_Wipe");

    // マテリアルは色だけ使う（テクスチャは SRV 直バインド）
    m_Materiale = std::make_unique<Material>();
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.Shiness = 1;
    mtrl.TextureEnable = false;
    m_Materiale->Create(mtrl);

    // 定数バッファ（b1 を使用）
    D3D11_BUFFER_DESC bd{};
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.ByteWidth = sizeof(WipeCB);
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    auto* dev = Renderer::GetDevice();
    dev->CreateBuffer(&bd, nullptr, m_CB.ReleaseAndGetAddressOf());

    // 画面アスペクトの初期値
    auto vp = Renderer::GetViewport();
    m_Aspect = (vp.Height > 0) ? (vp.Width / vp.Height) : 16.0f / 9.0f;

    // スクリーン全体にフィット
    SetScale(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
    SetColor(1, 1, 1, 1);
}

void Wipe::Draw()
{
    if (!m_Snapshot) return;

    // 1) 状態
    Renderer::SetDepthEnable(false);
    Renderer::SetBlendState(BS_ALPHABLEND);

    // 2) 行列（スクリーン直貼り）
    Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.x, m_Rotation.y, m_Rotation.z);
    Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
    Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);
    Matrix world = s * r * t;
    Renderer::SetWorldMatrix(&world);

    // 3) GPU セット
    SetGPU();
    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();
    m_Materiale->SetDiffuse(DirectX::XMFLOAT4(m_Color.x, m_Color.y, m_Color.z, m_Color.w));
    m_Materiale->Update();
    m_Materiale->SetGPU();

    // 4) 定数バッファ更新（円半径は Progress から算出）
    auto* ctx = Renderer::GetDeviceContext();

    // 画面端まで届く最大半径（アスペクト補正込み）
    const float cx = m_CenterX, cy = m_CenterY, asp = m_Aspect;
    const float dx = max(cx, 1.0f - cx);
    const float dy = max(cy, 1.0f - cy);
    const float rMax = std::sqrt((dx * asp) * (dx * asp) + dy * dy);

    WipeCB cb{};
    cb.CenterX = cx;
    cb.CenterY = cy;
    cb.Radius = (m_Invert ? (1.0f - m_Progress) : m_Progress) * rMax;
    cb.Feather = m_Feather;
    cb.Aspect = asp;
    cb.Zoom = std::lerp(m_ZoomFrom, m_ZoomTo, m_Progress);
    cb.Invert = m_Invert ? 1.0f : 0.0f;

    D3D11_MAPPED_SUBRESOURCE map{};
    ctx->Map(m_CB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
    std::memcpy(map.pData, &cb, sizeof(cb));
    ctx->Unmap(m_CB.Get(), 0);

    ctx->PSSetConstantBuffers(1, 1, m_CB.GetAddressOf());
    ctx->PSSetShaderResources(0, 1, m_Snapshot.GetAddressOf()); // t0

    // 5) 描画
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    Camera::ScopedMode scoped(m_Camera, CAMERA_2D);
    ctx->DrawIndexed(4, 0, 0);

    Renderer::SetDepthEnable(true);
}
