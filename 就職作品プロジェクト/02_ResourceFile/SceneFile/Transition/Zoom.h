#pragma once
#include "TransitionBase.h"
class Zoom : public TransitionBase
{
private:

public:
    explicit Zoom(Camera* cam);
    void Initialize()       override;  
    void Update(float tick) override;      
    void Draw()             override;        
    void Finalize()         override;    

    void ZOOM_IN(float tick);
    void ZOOM_OUT(float tick);
    
};

