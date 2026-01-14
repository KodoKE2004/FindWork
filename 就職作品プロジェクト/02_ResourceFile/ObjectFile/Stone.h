#pragma once
#include "Square.h"
class Stone : public Square
{
private:
    bool m_isActive;

public:
    Stone(Camera& cam);
    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;
    
    bool IsActive() const {
        return m_isActive;
    }
};


