#pragma once
#include "TransitionBase.h"


// フェード用の画像を使用してシーン遷移を行うクラス
// このクラスは、フェードイン・フェードアウトの効果を持つテクスチャを管理します。
class Fade : public TransitionBase
{
private:

    float m_Alpha = 0.0f;      // 現在のアルファ値
    float m_AlphaValue = 0.0f; // アルファ値の変化量


public:
	Fade(Camera* cam);

	void Initialize()	override;
	void Update()		override;
	void Draw()			override;
	void Finalize()		override;

	void FADE_IN();
	void FADE_OUT();

};

