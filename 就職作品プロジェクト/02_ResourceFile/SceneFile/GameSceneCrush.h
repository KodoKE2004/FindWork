#pragma once
#include <vector>
#include "GameSceneExe.h"
#include "Player.h"

class GameSceneCrush : public GameSceneExe
{
private:
    
    pShared<Square> m_Plane;
    pShared<Player> m_Player; 
public:

   GameSceneCrush() = delete;
   GameSceneCrush(Camera& cam);
    ~GameSceneCrush()         override = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Draw() 			override;
    void Finalize()         override;

    SCENE_NO GetSceneNo() const override {
        return SCENE_NO::GAME_CRUSH;
    }

};

