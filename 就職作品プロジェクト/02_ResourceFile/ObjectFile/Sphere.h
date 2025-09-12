#pragma once
#include "Object.h"

#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Texture.h"

class Sphere : public Object
{
protected:
	VertexBuffer<VERTEX_3D> m_VertexBuffer;
	IndexBuffer				m_IndexBuffer;
	IndexBuffer				m_IndexBufferInside;
	Texture*				m_Texture;

	unsigned int m_IndexCount = 0;
	unsigned int m_InsideIndexCount = 0;

	int			 m_Sline = 32;
	int			 m_Stack = 64;
	float		 m_Radius = 2.0f;

	bool m_IsSky = false;
	float m_SkyRadius = 800.0f;

	// 一時的に切り替えるためのレンダーステート
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>   m_RS_CullFront; // 内側を見る
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DSS_NoWrite_Lequal; // 深度書き込みなし

	void CreateSkyStates();   // RS/DSS の生成
	void DrawAsSky();         // スカイドーム描画パス
	void DrawAsMesh();        // 通常メッシュ描画パス（既存）

public:

	void SetSline(int sline) { m_Sline = sline; }
	void SetStack(int stack) { m_Stack = stack; }
	void SetRadius(float radius) { m_Radius = radius; }


public:
	Sphere(Camera* cam);

	void Initialize() override;
	void Update()	  override;
	void Draw()		  override;
	void Finalize()	  override;

	void SetSkyDomeMode(bool flg) { m_IsSky = flg ;}

	void EnableSkyDome(const std::string& texPath, float radius = 500.0f, bool useSRGB = true);
	void DisableSkyDome();

};

