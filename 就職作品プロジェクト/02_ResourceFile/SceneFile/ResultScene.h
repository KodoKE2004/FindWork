#pragma once
#include "Scene.h"
#include "Square.h"

class ResultScene : public Scene
{
private:

    pShared<Square> m_ButtonToTitle; 
    pShared<Square> m_ButtonRetry  ;

    bool m_isCorsorButtonToTitle = true;

    static constexpr float AlphaChangeTimer = 0.5f; 
    float m_DurationCuror = 0.0f;


public:
    //================================
    // コンストラクタとデストラクタ
    //================================
    ResultScene()  = delete;
    ResultScene(Camera& cam);
    ~ResultScene() = default;

    void Initialize()		override;	// シーンの初期化
    void Update(float tick) override;	// シーンの更新
    void Draw() 			override;
    void Finalize()         override;	// シーンの終了処理
    
    SCENE_NO GetSceneNo() const override {
        return SCENE_NO::RESULT;
    }

};

