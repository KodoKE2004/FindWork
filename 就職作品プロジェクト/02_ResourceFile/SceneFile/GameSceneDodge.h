#pragma once
#include "GameSceneExe.h"
#include "Player.h"
#include "Bird.h"
#include "Stone.h"

#include <vector>

class GameSceneDodge : public GameSceneExe
{
private:
    std::shared_ptr<Square> m_Background;
    std::shared_ptr<Bird>  m_Bird;
    std::vector<std::shared_ptr<Stone>> m_StoneList;

    float m_StoneSpawnElapsed = 0.0f;
    size_t m_CurrentCartPatternIndex = 0;

    public:
    GameSceneDodge()  = delete;
    GameSceneDodge(Camera& cam);
    ~GameSceneDodge() = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Draw() 			override;
    void Finalize()         override;


    SCENE_NO GetSceneNo(){
        return SCENE_NO::GAME_DODGE;
    }
};

