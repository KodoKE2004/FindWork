#pragma once
#include "GameSceneExe.h"
#include "Square.h"
#include "Skydome.h"
#include "Enemy.h"

class GameSceneSlice : public GameSceneExe
{
private:
    std::shared_ptr<Enemy> m_Enemy   = nullptr;

public:
    GameSceneSlice() = delete;
    GameSceneSlice(Camera& cam);
    ~GameSceneSlice()       override = default;

    void Initialize()       override ;
    void Update(float tick) override ;
    void Draw() 			override;
    void Finalize()         override ;

    SCENE_NO GetSceneNo() const override {
        return SCENE_NO::GAME_SLICE;
    }
};

