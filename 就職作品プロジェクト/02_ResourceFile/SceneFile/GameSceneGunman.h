#pragma once
#include "GameSceneExe.h"
#include "MouseObject.h"


class GameSceneGunman : public GameSceneExe
{
private:
    enum REDY_SLOT
    {
        REDY_A,
        REDY_B,
        SLOT_SIZE
    };

    pShared<Square> m_Background;
    std::array<pShared<MouseObject>, SLOT_SIZE> m_RedyList;
    pShared<Square>      m_Wanted;
    pShared<Square>      m_Subject;
    pShared<MouseObject> m_Oldman;

    bool m_isCreateReactionSE = false;

private:
    // â∫Ç©ÇÁè„Ç…èoåªÇ∑ÇÈ
    void TargetUpPosition();

public:
    GameSceneGunman() = delete;
    GameSceneGunman(Camera& cam);
    ~GameSceneGunman() = default;

    void Initialize()       override;
    void Update(float tick) override;
    void Draw() 			override;
    void Finalize()         override;

    SCENE_NO GetSceneNo() {
        return SCENE_NO::NONE;
    }
};

