#pragma once
#include <vector>
#include "GameSceneExe.h"


class GameSceneCrush : public GameSceneExe
{
private:
    
public:

   GameSceneCrush() = default;
    ~GameSceneCrush()         override = default;
    void Initialize()       override;
    void Update(float tick) override;
    void Finalize()         override;

    SCENE_NO GetSceneNo() const override {
        return SCENE_NO::GAME_CRUSH;
    }

};

