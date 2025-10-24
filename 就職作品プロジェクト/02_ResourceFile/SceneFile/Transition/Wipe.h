// SceneFile/Transition/Wipe.h
#pragma once
#include "TransitionBase.h"
#include <wrl/client.h>
#include <d3d11.h>

class Wipe : public TransitionBase
{
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Snapshot; // 次シーンの絵
    // パラメータ
    float m_Progress = 0.0f;     // 0..1 （半径の比）
    float m_Feather = 0.02f;    // ふちのソフトさ
    float m_Aspect = 16.0f / 9.0f; // 画面の W/H
    float m_ZoomFrom = 1.0f;     // UV のズーム（旧→新）
    float m_ZoomTo = 1.2f;     // 例: 1.2 で “入っていく” 感
    float m_CenterX = 0.5f;     // 中心（0..1）
    float m_CenterY = 0.5f;
    bool  m_Invert = false;    // 逆方向（閉じる）に使いたい時用

    // 定数バッファ
    struct WipeCB {
        float CenterX, CenterY;
        float Radius, Feather;
        float Aspect;
        float Zoom;
        float Invert;
        float _pad;
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_CB;

public:
    using TransitionBase::TransitionBase; // Camera* を親から

    void Initialize() override;
    void Update()     override {} // ロジックは TransScene 側で進める
    void Draw()       override;
    void Finalize()   override {}

    // セッター（読みやすい名前で）
    void SetSnapshot(ID3D11ShaderResourceView* srv) { m_Snapshot = srv; }
    void SetProgress(float v) { m_Progress = v; }
    void SetFeather(float v) { m_Feather = v; }
    void SetAspect(float v) { m_Aspect = v; }
    void SetCenter(float x, float y) { m_CenterX = x; m_CenterY = y; }
    void SetZoom(float from, float to) { m_ZoomFrom = from; m_ZoomTo = to; }
    void SetInvert(bool inv) { m_Invert = inv; }
};
