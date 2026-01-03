#pragma once
#include "Enemy.h"
#include "GameSceneExe.h"
#include "Skydome.h"
#include "Square.h"

class GameSceneSlice : public GameSceneExe
{
private:
    std::shared_ptr<Enemy> m_Enemy   = nullptr;

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

