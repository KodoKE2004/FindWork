#pragma once
#include <memory>
#include <string>
#include "Scene.h"
#include "../Renderer.h"
#include "../Application.h"
#include "../Shader.h"
#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Material.h"
#include "../Game.h"
#include <vector>

enum SWITCH
{
	OFF,
	START,
	DOING,
	FINISH,
};

enum TRANS_MODE
{
	FADE,
};

/// <summary>
/// 遷移の管理をするクラス
/// </summary>
class SceneTrans
{
public:
	// コンストラクタとデストラクタ
	SceneTrans()  = default;
	~SceneTrans() = default;

	// 画面遷移の初期化
	static void Initialize(TRANS_MODE mode);
	// 画面遷移の更新
	static void Update();
	// 画面遷移の描画
	static void Draw();

	static void   SetAlpha(float alpha);
	static void   SetChange(bool isChange);
	static float  GetAlpha(){ return m_Alpha; }
	static void	  SetTransition(SWITCH setSwitch);
	static SWITCH IsTransition();

private:
	static Scene*		m_NextScene;		// 次のシーン
	static float		m_Timer;			// タイマー
	static float		m_Duration;			// 遷移時間
	static bool			m_isChange;			// 切り替えOK？
	static SWITCH		m_isTransition;		// 遷移が完了したかどうか
	static TRANS_MODE	m_TransMode;		// 遷移の仕方

	static VertexBuffer<VERTEX_3D> s_Vertex;
	static IndexBuffer  s_Index;
	static Shader       s_Shader;
	static std::unique_ptr<Material> s_Material;

	// 演出の値管理
	static float m_Alpha;
	static float m_Delta;

};

