#pragma once
#include "Square.h"
class Bird : public Square
{
private:

public:
    Bird(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

};

