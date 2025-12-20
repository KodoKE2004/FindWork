#include "DragController.h"

DragController::DragController(Camera& cam) : Square(cam)
{
}

DragController::~DragController()
{
}

void DragController::Initialize()
{   
    Square::Initialize();
}

void DragController::Update()
{
    
}

void DragController::Draw()
{
    Square::Draw();
}

void DragController::Finalize()
{
    Square::Finalize();
}
