#pragma once
#include "GameSceneExe.h"
class GameSceneHit : public GameSceneExe
{
private:

public:

   GameSceneHit() = default;
    ~GameSceneHit()       override = default;
    void Initialize()       override ;
    void Update(float tick) override ;
    void Finalize()         override ;

    SCENE_NO GetSceneNo() const override {
        return SCENE_NO::GAME_HIT;
    }

};

