#pragma once
#include <vector>
#include "GameSceneExe.h"
#include "Player.h"

class GameSceneShot : public GameSceneExe
{
private:
    
    pShared<Square> m_Plane;
    pShared<Player> m_Player; 

    float m_CreateBulletTime = 0.0f;
    float m_CreateBulletElapsed = 0.0f;

public:

   GameSceneShot() = delete;
   GameSceneShot(Camera& cam);
    ~GameSceneShot()         override = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Draw() 			override;
    void Finalize()         override;

    SCENE_NO GetSceneNo() const override {
        return SCENE_NO::GAME_SHOT;
    }

    void CreateBullet();
};

