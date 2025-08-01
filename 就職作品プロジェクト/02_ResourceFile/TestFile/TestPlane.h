#pragma once

#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Shader.h"
#include "../TextureDX.h"

//-----------------------------------------------------------------------------
//TestPlaneクラス
//-----------------------------------------------------------------------------
class TestPlane : public Transform
{
	// 描画の為の情報（メッシュに関わる情報）
	IndexBuffer	 m_IndexBuffer;					// インデックスバッファ
	VertexBuffer<VERTEX_3D>	m_VertexBuffer;		// 頂点バッファ

	// 描画の為の情報（見た目に関わる部分）
	Shader m_Shader;   // シェーダー
	Texture m_Texture; // テクスチャ

public:
	void Init();
	void Draw();
	void Update();
	void Uninit();
};