#pragma once
#include "IndexBuffer.h"
#include "Material.h"
#include "Object.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include <memory>

class Circle : public Object
{
protected:
    float m_Radius = 50.0f;
    int m_Segments = 32; // 円を構成するセグメント数
    unsigned int m_IndexCount = 0;

    VertexBuffer<VERTEX_3D>   m_VertexBuffer; // 頂点バッファ
    IndexBuffer               m_IndexBuffer;  // インデックスバッファ
    std::shared_ptr<Texture>  m_Texture;	  // テクスチャ
    std::unique_ptr<Material> m_Materiale;	  //マテリアル

public:
    Circle(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void  SetRadius(float radius);
    float GetRadius() const { return m_Radius; }
};

