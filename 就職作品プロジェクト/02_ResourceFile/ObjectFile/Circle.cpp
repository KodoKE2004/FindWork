#include "Circle.h"



Circle::Circle(Camera& cam) : Object(cam)
{
}

void Circle::Initialize()
{
    // 最低限のチェック
    if (m_Segments < 3) m_Segments = 3;
    if (m_Radius <= 0.0f) m_Radius = 50.0f;

    // 頂点（中心 + 円周）を作成
    std::vector<VERTEX_3D> vertices;
    vertices.reserve(m_Segments + 1);

    VERTEX_3D center{};
    center.position = NVector3(0.0f, 0.0f, 0.0f);
    center.normal = NVector3(0.0f, 0.0f, -1.0f);
    center.color = Color(1, 1, 1, 1);
    center.uv = Vector2(0.5f, 0.5f);
    vertices.push_back(center);

    const float twoPi = DirectX::XM_2PI;
    for (int i = 0; i < m_Segments; ++i)
    {
        float t = static_cast<float>(i) / static_cast<float>(m_Segments);
        float angle = t * twoPi;
        float x = std::cosf(angle) * m_Radius;
        float y = std::sinf(angle) * m_Radius;

        VERTEX_3D v{};
        v.position = NVector3(x, y, 0.0f);
        v.normal = NVector3(0.0f, 0.0f, -1.0f);
        v.color = Color(1, 1, 1, 1);
        // UV を [0,1] にマップ（中心を 0.5,0.5 とする）
        v.uv = Vector2(0.5f + 0.5f * std::cosf(angle), 0.5f + 0.5f * std::sinf(angle));
        vertices.push_back(v);
    }

    // インデックス（中心を0として扇状に三角形を作る）
    std::vector<unsigned int> indices;
    indices.reserve(m_Segments * 3);
    for (int i = 1; i <= m_Segments; ++i)
    {
        int a = 0;
        int b = i;
        int c = (i == m_Segments) ? 1 : (i + 1);
        indices.push_back(a);
        indices.push_back(b);
        indices.push_back(c);
    }

    m_IndexCount = static_cast<unsigned int>(indices.size());

    // バッファ作成
    m_VertexBuffer.Create(vertices);
    m_IndexBuffer.Create(indices);

    // シェーダ設定（アルファ付き汎用を使用）
    SetShader("VS_Alpha", "PS_Alpha");

    // マテリアル準備
    m_Materiale = std::make_unique<Material>();
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.Shiness = 1;
    mtrl.TextureEnable = true;
    m_Materiale->Create(mtrl);

    // デフォルト：2Dモードのカメラ（必要なら変更）
    m_Camera.SetCamera(CAMERA_2D);
}

void Circle::Update()
{
}

void Circle::Draw()
{
    // ワールド行列の構築
    Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
    Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
    Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);
    Matrix world = s * r * t;
    Renderer::SetWorldMatrix(&world);

    ID3D11DeviceContext* dc = Renderer::GetDeviceContext();
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // シェーダ/バッファ/テクスチャをセット
    SetGPU();
    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();
    if (m_Texture) {
        m_Texture->SetGPU();
    }

    if (m_Materiale)
    {
        m_Materiale->SetDiffuse(DirectX::XMFLOAT4(m_Color.x, m_Color.y, m_Color.z, m_Color.w));
        m_Materiale->Update();
        m_Materiale->SetGPU();
    }

    // 描画
    dc->DrawIndexed(m_IndexCount, 0, 0);
}

void Circle::Finalize()
{
    m_Texture = nullptr;
    m_IndexCount = 0;
}
