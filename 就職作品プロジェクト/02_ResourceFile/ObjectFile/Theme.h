#pragma once
#include "Square.h"

// テーマを描画するクラス
class Theme : public Square
{
protected:
    bool m_isActive = false;

public:
    Theme(Camera& cam);
    ~Theme() = default;
    // 純粋仮想関数化
    // このクラス内にも関数を記述している
    void Initialize() override;
    void Update()	  override;
    void Draw()		  override;
    void Finalize()	  override;

    void SetActive(const bool isActive);
};

