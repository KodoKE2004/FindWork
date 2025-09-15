#pragma once
#include "Scene.h"
#include "../ObjectFile/Bullet.h"
#include "../ObjectFile/Sphere.h"

#include "../Game.h"
#include <vector>
#include <memory>
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
    const float m_ChangeTime = 30.0f; // シーン変更までの時間

    std::vector<Bullet*> m_Bullet;
    Camera* m_Camera = nullptr;
    std::unique_ptr<Sphere> m_SkyDome ;
    int m_ShotCount = 0;

	bool isStart = false;
    bool isEnd   = false;
};

