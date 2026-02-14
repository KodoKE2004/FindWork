#pragma once
#include "Square.h"
#include "Camera.h"
#include "Bullet.h"
#include "Calculator.h"


class Player : public Square
{
private:
    pShared<Bullet>         m_Bullet;
    vector<pShared<Bullet>> m_BulletList;
    
    float m_CreateBulletTime    = 0.0f;
    float m_CreateBulletElapsed = 0.0f;

public:
    Player(Camera & cam);
    ~Player() override = default;

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    
private:

};