#include "Wood.h"
#include "Game.h"

Wood::Wood(Camera* cam) : Square(cam)
{
}

void Wood::Initialize()
{   
    Square::Initialize();
}

void Wood::Update()
{
}

void Wood::Draw()
{
    Square::Draw();
}

void Wood::Finalize()
{
    Square::Finalize();
}

void Wood::ChangeWood()
{
    // –ØÞ‚ÌŒ©‚½–Ú‚ð•Ï‚¦‚é
    auto textureMgr = GAME_MANAGER_TEXTURE;
    SetTexture(textureMgr->GetTexture("Black.png"));
}
