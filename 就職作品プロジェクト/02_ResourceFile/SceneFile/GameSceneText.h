#pragma once
#include "GameSceneExe.h"
#include "DragObject.h"
#include <memory>
class GameSceneText : public GameSceneExe
{
private:
    std::shared_ptr<DragObject> m_DragObject;
    bool  m_HasSpawnedCartWarning = false;

    size_t m_CurrentCartPatternIndex = 0;

    float GenerateActivationDelay();
public:
    GameSceneText() = default;
    ~GameSceneText() = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Finalize()         override;

    SCENE_NO GetSceneNo() {
        return SCENE_NO::GAME_TEXT;
    }
};
