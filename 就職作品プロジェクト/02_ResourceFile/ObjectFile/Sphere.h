#pragma once
#include "Object.h"
class Sphere : public Object
{
private:

public:
	Sphere(Camera* cam);

	void Initialize() override;
	void Update()	  override;
	void Draw()		  override;
	void Finalize()	  override;

};

