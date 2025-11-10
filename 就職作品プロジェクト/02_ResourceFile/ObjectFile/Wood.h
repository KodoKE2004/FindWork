#pragma once
#include "Square.h"
class Wood : public Square
{
private:
    
public:
    Wood(Camera* cam);
    ~Wood() = default ;
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void ChangeWood();
};

