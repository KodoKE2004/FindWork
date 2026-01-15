#include "Timer.h"

Timer::Timer(Camera& cam) : Square(cam)
{
}

void Timer::Initialize()
{
    Square::Initialize();
}

void Timer::Update()
{
}

void Timer::Draw()
{
    Square::Draw();
}

void Timer::Finalize()
{
    Square::Finalize();
}
