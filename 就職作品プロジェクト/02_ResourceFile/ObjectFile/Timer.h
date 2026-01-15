#pragma once
#include "Square.h"
class Timer : public Square
{
private:
    
public:
    Timer(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;
};

