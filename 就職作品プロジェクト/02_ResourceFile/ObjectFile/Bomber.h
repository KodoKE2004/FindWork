#pragma once
#include "Square.h"
class Bomber : public Square
{

public:
    Bomber(Camera* cam);
    ~Bomber() = default;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

};

