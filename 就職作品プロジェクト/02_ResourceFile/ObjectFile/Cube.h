#pragma once

#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Texture.h"
#include "../Camera.h"
#include "Object.h"

//-----------------------------------------------------------------------------
//TestPlaneクラス
//-----------------------------------------------------------------------------
class Cube : public Object
{
	// SRT情報（姿勢情報）
	NVector3 m_Position = NVector3(0.0f, 0.0f, 0.0f);
	NVector3 m_Rotation = NVector3(0.0f, 0.0f, 0.0f);
	NVector3 m_Scale    = NVector3(1.0f, 1.0f, 1.0f);

	// 描画の為の情報（メッシュに関わる情報）
	IndexBuffer	 m_IndexBuffer; // インデックスバッファ
	VertexBuffer<VERTEX_3D>	m_VertexBuffer; // 頂点バッファ

	// 描画の為の情報（見た目に関わる部分）
	Texture m_Texture;		// 画像

public:
	Cube(Camera* cam);  // コンストラクタ
	~Cube() = default;  // デストラクタ

	void Initialize();
	void Draw();
	void Update();
	void Finalize();
};