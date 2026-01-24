#pragma once
#include <wrl.h>
#include <memory>

#include "TransitionBase.h"
#include "Renderer.h"

using namespace Microsoft::WRL;

class Zoom : public TransitionBase
{
private:
    NVector3 m_BaseScale;
    NVector3 m_TargetScale;
public:
    Zoom(Camera& cam);
    void Initialize()       override;  
    void Update(float tick) override;      
    void Draw()             override;        
    void Finalize()         override;    

    void ZOOM_IN(float tick);
    void ZOOM_OUT(float tick);
    
    void SetTransMode(TRANS_MODE transMode) override;
    void SetBaseScale(const NVector3& scale);
    void SetTargetScale(const NVector3& scale);
};

