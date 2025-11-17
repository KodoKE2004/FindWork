#pragma once
#include "Scene.h"
class ResultScene : public Scene
{
private:

public:
    //================================
    // コンストラクタとデストラクタ
    //================================
    ResultScene()  = default;
    ~ResultScene() = default;

    void Initialize()		override;	// シーンの初期化
    void Update(float tick) override;	// シーンの更新
    void Finalize()         override;	// シーンの終了処理
    
    SCENE_NO GetSceneNo() const override {
        return SCENE_NO::RESULT;
    }

};

