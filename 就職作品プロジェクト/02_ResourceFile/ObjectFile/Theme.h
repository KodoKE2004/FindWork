#pragma once
#include "Square.h"

// テーマを描画するクラス
class Theme : public Square
{
protected:
    bool m_isActive = false;

    float m_Elapsed = 0.0f;

public:
    Theme(Camera& cam);
    ~Theme() = default;
    // 純粋仮想関数化
    // このクラス内にも関数を記述している
    void Initialize() override;
    void Update()	  override;
    void Draw()		  override;
    void Finalize()	  override;

    void SetActive();
    void SetActive(const bool isActive);
};

