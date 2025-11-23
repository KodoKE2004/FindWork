#pragma once
#include "Scene.h"
#include "Square.h"

class ResultScene : public Scene
{
private:

    Square* m_ButtonToTitle = nullptr; 
    Square* m_ButtonRetry   = nullptr;

    bool m_isCorsorButtonToTitle = true;

    static constexpr float AlphaChangeTimer = 0.5f; 
    float m_DurationCuror = 0.0f;


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

