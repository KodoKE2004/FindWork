#pragma once
#include "Square.h"
#include <SimpleMath.h>

enum MOVE_DIR
{
    MOVE_UP = 0,
    MOVE_UP_RIGHT,
    MOVE_RIGHT,
    MOVE_DOWN_RIGHT,
    MOVE_MAX,
    MOVE_NONE = -1,
};

const float MOVE_ANGLE[MOVE_MAX] = {
    90.0f,
    315.0f,
    0.0f,
    45.0f,
};

class DragController : public Square
{
protected:

    bool m_IsDragging = false;
    DirectX::SimpleMath::Vector2 m_DragOffset{};
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

        m_MoveDir = dir;
    
        // 角度から単位ベクトルを計算
        const float dirX = std::cosf(DirectX::XMConvertToRadians(MOVE_ANGLE[dir]));
        const float dirY = std::sinf(DirectX::XMConvertToRadians(MOVE_ANGLE[dir]));
        m_Dir = DirectX::SimpleMath::Vector2(dirX, dirY);
    }

    bool IsDragging() const {
        return m_IsDragging;
    }
};

