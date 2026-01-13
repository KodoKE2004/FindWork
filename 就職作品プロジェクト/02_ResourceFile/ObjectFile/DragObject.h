#pragma once
#include "Square.h"

class DragObject : public Square
{
protected:
    NVector3 m_BaseScale;

    bool m_isDrag   = false;
    bool m_isInside = false;
public:
    DragObject(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    bool IsDrag();
};

