#pragma once
#include "GameSceneExe.h"
#include "Square.h"
#include "Skydome.h"
#include "Enemy.h"

class GameSceneSlice : public GameSceneExe
{
private:
    Enemy* m_Enemy   = nullptr;

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

