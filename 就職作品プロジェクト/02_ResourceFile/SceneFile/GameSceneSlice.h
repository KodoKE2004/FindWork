#pragma once
#include "GameSceneExe.h"
#include "Square.h"
#include "Player.h"
#include "Skydome.h"

class GameSceneSlice : public GameSceneExe
{
private:
    float m_SliceInterval = 1.0f; // スライスの間隔
    float m_SliceTimer = 0.0f;    // スライスタイマー
    int   m_SliceCount   = 0;     // スライスカウント
    const int m_MaxSlices = 5;    // 最大スライス数

    Skydome* m_Skydome         = nullptr;
    Player*  m_Sord            = nullptr;
             
public:
    GameSceneSlice() = default;
    ~GameSceneSlice()       override = default;
    void Initialize()       override ;
    void Update(float tick) override ;
    void Finalize()         override ;

    SCENE_NO GetSceneNo() const override {
        return SCENE_NO::GAME_SLICE;
    }
};

