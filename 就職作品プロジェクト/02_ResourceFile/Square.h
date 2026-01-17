#pragma once
#include "Object.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Material.h"

//----------------------------------------
//		2D	当たり判定の結果を受け取るクラス
//----------------------------------------
struct isHitResult
{

	bool m_Old = false;
	bool m_Now = false;

	bool isTriggered() {
		return (!m_Old && m_Now);
	}
	bool isReleased() {
		return (m_Old && !m_Now);
	}
	bool isHold() {
		return m_Now;
	}

	void SetHitResult(const bool hit)
	{
		m_Old = m_Now;
		m_Now = hit;
	}

	void Update()
	{
		m_Old = m_Now;
	}
};


//-----------------------------------------------------------------------------
// 2DのSquareクラス
//-----------------------------------------------------------------------------
class Square : public Object
{
protected:
	// 描画の為の情報（メッシュに関わる情報）
	IndexBuffer m_IndexBuffer; // インデックスバッファ
	VertexBuffer<VERTEX_3D> m_VertexBuffer; // 頂点バッファ

	// 描画の為の情報（見た目に関わる部分）
	std::shared_ptr<Texture>  m_Texture;	// テクスチャ
	std::unique_ptr<Material> m_Materiale;	//マテリアル

	// UV座標の情報

	float m_NumU = 1.0f;		// U座標の位置
	float m_NumV = 1.0f;		// V座標の位置
	float m_SplitX = 1.0f;		// U座標の分割数
	float m_SplitY = 1.0f;		// V座標の分割数

	// 描画パイプラインの既定化（他の描画で状態が変わっても影響を受けないようにする）
	void SetPipeline();

public:
	isHitResult m_HitResult;

	Square(Camera& cam); // コンストラクタ
	~Square(); // デストラクタ

	// 純粋仮想関数化
    // このクラス内にも関数を記述している
	void Initialize() override;
	void Update()	  override;
	void Draw()		  override;
	void Finalize()	  override;

	// テクスチャを指定
	void SetTexture(const char* imgname);
	void SetTexture(std::shared_ptr<Texture> texture);
	// UV座標を指定
	void SetUV(const float& nu, const float& nv, const float& sx, const float& sy);

	Vector2 GetUV () const {
		return Vector2(m_NumU, m_NumV);
    }

	Vector2 GetSplit() const {
		return Vector2(m_SplitX, m_SplitY);
    }

};
