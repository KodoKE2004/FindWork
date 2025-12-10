#pragma once
#include "Square.h"
class Sord : public Square
{
private:
    
public:
    Sord(Camera& cam);
    ~Sord() = default;
    // 純粋仮想関数化
    // このクラス内にも関数を記述している
    void Initialize() override;
    void Update()	  override;
    void Draw()		  override;
    void Finalize()	  override;

    void Attack();

};

