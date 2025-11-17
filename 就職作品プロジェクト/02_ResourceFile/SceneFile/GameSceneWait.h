#pragma once
#include "Scene.h"

class GameSceneWait : public Scene
{
private:
    uint32_t m_MyLife = 4;					   // 自分のライフ
    SCENE_NO m_SelectedScene = SCENE_NO::NONE; // 選択されたシーン
	bool m_ShouldTransitionToStage = false;

    void PrepareNextStage();

public:
	//================================
	//		コンストラクタとデストラクタ
	//================================

	GameSceneWait()  = default;
	~GameSceneWait() = default;

	//================================
	// 			ループ内の処理
	//================================

	void Initialize()		override;	// シーンの初期化
	void Update(float tick) override;	// シーンの更新
	void Finalize()         override;	// シーンの終了処理

	SCENE_NO GetSceneNo() const override {
		return SCENE_NO::GAME_WAIT;
	}
};

