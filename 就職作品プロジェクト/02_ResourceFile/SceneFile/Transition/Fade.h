#pragma once
#include "../../Texture2D.h"

// フェード用の画像を使用してシーン遷移を行うクラス
// このクラスは、フェードイン・フェードアウトの効果を持つテクスチャを管理します。
class Fade : public Texture2D
{
public:
	Fade(Camera* cam);

	void Initialize();
	void Update();
	void Draw() override;
	void Finalize();
};

