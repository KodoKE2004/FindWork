#pragma once
#include "Square.h"

// テーマを描画するクラス
class Theme : public Square
{
protected:
    bool m_isActive = false;
    float m_ScaleMass = 0.0f;
    float m_Elapsed   = 0.0f;
    float m_Duration  = 0.0f;
    NVector3 m_ScaleBase;
    bool m_DebugSolidDraw = false;
    Color m_DebugSolidColor = Color(1.0f, 0.0f, 1.0f, 1.0f);

#ifdef _DEBUG
    bool     m_DebugViewAdjustEnabled = false;
    float    m_DebugViewScaleMul = 1.0f;                    
    NVector3 m_DebugViewPosOffset = NVector3(0, 0, 0);      // 例: 左上寄せ分
#endif

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
    void SetScaleBase(NVector3 scale);
    void SetDebugSolidDraw(bool enable, const Color& color = Color(1.0f, 0.0f, 1.0f, 1.0f))
    {
        m_DebugSolidDraw = enable;
        m_DebugSolidColor = color;
    }

#ifdef _DEBUG
    // Debug時だけ、描画直前に Position/Scale を倍率＋オフセット補正する
    void SetDebugViewAdjust(bool enable, float scaleMul, const NVector3& posOffset)
    {
//        m_DebugViewAdjustEnabled = enable;
//        m_DebugViewScaleMul = scaleMul;
//        m_DebugViewPosOffset = posOffset;
    }
#else
    void SetDebugViewAdjust(bool, float, const NVector3&) {}
#endif

};
