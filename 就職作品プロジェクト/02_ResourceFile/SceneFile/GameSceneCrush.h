#pragma once
#include <vector>
#include "GameSceneExe.h"
#include "Bullet.h"

class GameSceneCrush : public GameSceneExe
{
private:
    
    pShared<Square> m_Plane;
    pShared<Square> m_BackGround;
    pShared<Bullet> m_Bullet;
    vector<pShared<Bullet>> m_BulletList;

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

