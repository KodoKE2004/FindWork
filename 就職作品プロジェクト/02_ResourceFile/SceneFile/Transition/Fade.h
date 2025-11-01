#pragma once
#include "TransitionBase.h"


    enum class FADE_PHASE
    {
        OUT,
        IN,
        FINISH,
    };
    float      m_Alpha      = 0.0f; // ݂̃At@l
    float      m_AlphaValue = 0.0f; // At@l̕ω
    FADE_PHASE m_Phase      = FADE_PHASE::OUT;
    void ApplyAlpha();
        Fade(Camera* cam);
        void Initialize()       override;
        void Update()           override;       // UpdateŃtF[Yis
        void Draw()                     override;
        void Finalize()         override;
        void FADE_IN();
        void FADE_OUT();
public:
	Fade(Camera* cam);

	void Initialize()	override;
	void Update(){}				;	// Updateは使わない
	void Draw()			override;
	void Finalize()		override;

	void FADE_IN();
	void FADE_OUT();
};

