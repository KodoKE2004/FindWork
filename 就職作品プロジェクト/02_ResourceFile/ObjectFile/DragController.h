#pragma once
#include "Square.h"
#include <SimpleMath.h>

class DragController : public Square
{
private:
    enum MOVE_DIR
    {
        MOVE_NONE = -1,
        MOVE_UP,
        MOVE_UP_RIGHT,
        MOVE_RIGHT,
        MOVE_DOWN_RIGHT,
        MOVE_DOWN,
        MOVE_DOWN_LEFT,
        MOVE_LEFT,
        MOVE_UP_LEFT,
        MOVE_MAX
    };

    const float MOVE_ANGLE[MOVE_MAX] = {
        0.0f,
        45.0f,
        90.0f,
        135.0f,
        180.0f,
        225.0f,
        270.0f,
        315.0f
    };

    bool m_IsDragging = false;
    DirectX::SimpleMath::Vector2 m_DragOffset{};
    MOVE_DIR m_MoveDir = MOVE_NONE;
public:
    DragController(Camera& cam);
    ~DragController();
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;
    void SetDirection(const MOVE_DIR dir) {
        m_MoveDir = dir;
    }
};

