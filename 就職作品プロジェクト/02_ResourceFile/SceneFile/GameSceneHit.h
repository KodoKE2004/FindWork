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

    SceneNo GetSceneNo() const override {
        return SceneNo::GAME_HIT;
    }

};

