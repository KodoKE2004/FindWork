#pragma once
#include "Square.h"

//---------------------------------------
//        制限時間爆弾クラス
//  3-2-1-BOOM！までのカウントダウンを表示
//---------------------------------------
class BomTimeLimit : public Square
{
protected:
   int m_Count = 3;

public:
    BomTimeLimit(Camera& cam);
    ~BomTimeLimit() = default;

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void ChangeTexture();

    void SetCount(int count) {
        m_Count = count;
    }
};

