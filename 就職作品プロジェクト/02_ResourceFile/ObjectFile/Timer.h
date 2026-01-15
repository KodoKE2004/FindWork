#pragma once
#include "Square.h"
#include "Circle.h"
#include <memory>

class Timer : public Square
{
private:
    
    std::shared_ptr<Circle> m_Meter = nullptr;
    float m_Progress = 1.0f;

public:
    Timer(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void SetProgress(const float ratio);
};

