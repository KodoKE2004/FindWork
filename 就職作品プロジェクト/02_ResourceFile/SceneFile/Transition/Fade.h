#pragma once
#include "../../Texture2D.h"

class Fade : public Texture2D
{
public:
	Fade(Camera* cam);

	void Initialize();
	void Update();
	void Finalize();
};

