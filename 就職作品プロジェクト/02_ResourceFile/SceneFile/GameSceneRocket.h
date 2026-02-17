#pragma once
#include "GameSceneExe.h"
class GameSceneRocket : public GameSceneExe
{
private:
    pShared<Square>        m_Background;



public:
    GameSceneRocket() = delete;
    GameSceneRocket(Camera& cam);
    ~GameSceneRocket() = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Draw() 			override;
    void Finalize()         override;


    SCENE_NO GetSceneNo() {
        return SCENE_NO::GAME_ROCKET;
    }
};

