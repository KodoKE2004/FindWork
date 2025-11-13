#pragma once
#include "GameSceneExe.h"
#include "Player.h"
#include "Car.h"

class GameScenePush : public GameSceneExe
{
private:
    Cart*    m_Cart     = nullptr;
    Player* m_Player  = nullptr;
public:
    GameScenePush()  = default;
    ~GameScenePush() = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Finalize()         override;

    SCENE_NO GetSceneNo(){
        return SCENE_NO::GAME_PUSH;
    }
};

