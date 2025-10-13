#pragma once
#include "TransitionBase.h"


// フェード用の画像を使用してシーン遷移を行うクラス
// このクラスは、フェードイン・フェードアウトの効果を持つテクスチャを管理します。
class Fade : public TransitionBase
{
private:



public:
	Fade(Camera* cam);

	void Initialize()	override;
	void Update()		override;
	void Draw()			override;
	void Finalize()		override;

};

