#pragma once
#include "GameSceneExe.h"
class GameScenePush : public GameSceneExe
{
private:

public:
    GameScenePush()  = default;
    ~GameScenePush() = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Finalize()         override;

    SceneNo GetSceneNo(){
        return SceneNo::GAME_PUSH;
    }
};

