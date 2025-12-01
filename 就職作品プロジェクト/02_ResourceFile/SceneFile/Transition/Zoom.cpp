#include "Zoom.h"
#include "Game.h"

Zoom::Zoom(Camera* cam) : TransitionBase(cam)
{
    auto& instance = Game::GetInstance();
    TextureManager* textureMgr = instance;
    m_Texture = textureMgr->GetTexture("Black.png");

    std::vector<VERTEX_3D> vertices(4);
    vertices[0].position = NVector3(-0.5f, 0.5f, 0.0f);
    vertices[1].position = NVector3(0.5f, 0.5f, 0.0f);
    vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
    vertices[3].position = NVector3(0.5f, -0.5f, 0.0f);

    for (auto& v : vertices) v.color = Color(1, 1, 1, 1);

    vertices[0].uv = Vector2(0, 0);
    vertices[1].uv = Vector2(1, 0);
    vertices[2].uv = Vector2(0, 1);
    vertices[3].uv = Vector2(1, 1);

    m_VertexBuffer.Create(vertices);
    std::vector<unsigned int> indices{ 0,1,2,3 };
    m_IndexBuffer.Create(indices);

    SetShader("VS_Alpha", "PS_Alpha");

    m_Materiale = std::make_unique<Material>();
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.Shiness = 1;
    mtrl.TextureEnable = true;
    m_Materiale->Create(mtrl);
}

void Zoom::Initialize()
{   

}

void Zoom::Update(float tick)
{
}

void Zoom::Draw()
{
}

void Zoom::Finalize()
{
}

void Zoom::ZOOM_IN(float tick)
{
}

void Zoom::ZOOM_OUT(float tick)
{
}
