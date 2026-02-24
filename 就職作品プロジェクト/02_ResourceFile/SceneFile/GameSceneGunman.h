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
    bool m_isShot = false;
    float m_MoveUpElapsed = 0.0f;  // 上昇に時間をカウント
    float m_SubjectScaleElapsed = 0.0f;
    bool m_isSubjectScaleUp = true;
    NVector3 m_SubjectBaseScale = NVector3(1.0f, 1.0f, 1.0f);
    size_t m_ShotIndex = 0;        // 撃ったスロットのインデックス
private:
    // 下から上に出現する
    void TargetMovePosition();
    void ShotReaction();
    void UpdateSubjectScale(float tick);
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

