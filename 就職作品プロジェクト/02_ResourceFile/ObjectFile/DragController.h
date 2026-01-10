#pragma once
#include "Square.h"
#include <SimpleMath.h>

enum MOVE_DIR
{
    MOVE_UP = 0,
    MOVE_DOWN_RIGHT,
    MOVE_RIGHT,
    MOVE_UP_RIGHT,
    MOVE_MAX,
    MOVE_NONE = -1,
};

const float MOVE_ANGLE[MOVE_MAX] = {
    90.0f,
    315.0f,
    0.0f,
    45.0f,
};

const float MOVE_INV_SQRT2 = 0.70710678f;

const DirectX::SimpleMath::Vector2 MOVE_DIR_VEC[MOVE_MAX] = {
    { 0.0f, 1.0f },                        // MOVE_UP (90Åã)
    { MOVE_INV_SQRT2, MOVE_INV_SQRT2 },    // MOVE_DOWN_RIGHT (315Åã)
    { 1.0f, 0.0f },                        // MOVE_RIGHT (0Åã)
    { MOVE_INV_SQRT2, -MOVE_INV_SQRT2 },   // MOVE_UP_RIGHT (45Åã)
};

class DragController : public Square
{
protected:

    bool m_IsDragging = false;
    DirectX::SimpleMath::Vector2 m_MouseDownPos{};
    DirectX::SimpleMath::Vector2 m_ObjectDownPos{};
    DirectX::SimpleMath::Vector2 m_Dir{};
    MOVE_DIR m_MoveDir = MOVE_NONE;
public:
    DragController(Camera& cam);
    ~DragController();
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;
    void SetDirection(const MOVE_DIR dir) {

        m_MoveDir    = dir;
        m_Dir        = MOVE_DIR_VEC[dir];
        m_Rotation.z = MOVE_ANGLE[dir];
    }

    bool IsDragging() const {
        return m_IsDragging;
    }
};

