#pragma once
#include "Scene.h"
class SelectScene : public Scene
{
public:
	//================================
	// コンストラクタとデストラクタ
	//================================

	SelectScene()  = default;
	~SelectScene() = default;

	//================================
	// 			ループ内の処理
	//================================

	// シーンの初期化
	void Initialize() override;
	// シーンの更新
	void Update(float tick)     override;

	// シーンの終了処理
	void Finalize()   override;

private:
    float m_Time = 0.0f;
    float m_ReadyTime = 3.0f; // 操作可能になるまでの時間
    const float m_ChangeTime = 15.0f; // シーン変更までの時間

	bool isStart = false;
    bool isEnd   = false;
};

