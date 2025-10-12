#pragma once
#include "../../ObjectFile/Object.h"
#include "../../Camera.h"
#include "../../VertexBuffer.h"
#include "../../IndexBuffer.h "
#include "../../Texture.h"
#include "../../Material.h"

class TransitionBase : public Object
{
protected:
    // 描画の為の情報（メッシュに関わる情報）
    IndexBuffer m_IndexBuffer; // インデックスバッファ
    VertexBuffer<VERTEX_3D> m_VertexBuffer; // 頂点バッファ

    // 描画の為の情報（見た目に関わる部分）
    std::shared_ptr<Texture>  m_Texture;	// テクスチャ
    std::unique_ptr<Material> m_Materiale;	//マテリアル

    // UV座標の情報
    float m_NumU   = 1;
    float m_NumV   = 1;
    float m_SplitX = 1;
    float m_SplitY = 1;

public:
    
    TransitionBase(Camera* cam);
    virtual ~TransitionBase() = default;
    virtual void Initialize() = 0;
    virtual void Update()     = 0;
    virtual void Draw()       = 0;
    virtual void Finalize()   = 0;
    
    // テクスチャを指定
    void SetTexture(const char* imgname);
    void SetTexture(std::shared_ptr<Texture> texture);

    // UV座標を指定
    void SetUV(const float& nu, const float& nv, const float& sx, const float& sy);

};

