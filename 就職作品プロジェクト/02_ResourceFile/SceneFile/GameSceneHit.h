#pragma once
#include <vector>
#include "GameSceneExe.h"
#include "Hammer.h"
#include "Enemy.h"

class GameSceneHit : public GameSceneExe
{
private:
    Hammer* m_Hammer = nullptr;
    
public:

   GameSceneHit() = default;
    ~GameSceneHit()         override = default;
    void Initialize()       override;
    void Update(float tick) override;
    void Finalize()         override;

    SCENE_NO GetSceneNo() const override {
        return SCENE_NO::GAME_HIT;
    }

};

