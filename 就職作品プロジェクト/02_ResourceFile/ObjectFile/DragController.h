#pragma once
#include "Square.h"
#include <SimpleMath.h>

class DragController : public Square
{
private:
    bool m_IsDragging = false;
    DirectX::SimpleMath::Vector2 m_DragOffset{};

public:
    DragController(Camera& cam);
    ~DragController();
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

};

