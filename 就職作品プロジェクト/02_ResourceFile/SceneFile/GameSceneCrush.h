#pragma once
#include <vector>
#include "GameSceneExe.h"
#include "Hammer.h"


class GameSceneCrush : public GameSceneExe
{
private:
    Hammer* m_Hammer = nullptr;
    
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

