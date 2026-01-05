#pragma once
#include <wrl.h>
#include <memory>

#include "TransitionBase.h"
#include "Renderer.h"

using namespace Microsoft::WRL;

// フェード用コンスタントバッファ構造体
// ピクセルシェーダーで使用するフェード効果のパラメータを格納する構造体です。
// centerUV: フェードの中心位置をUV座標で指定します。
// fadeProgress: フェードの進行状況を示す値（0.0から1.0の範囲）。
// maxInnerRadius: フェードの内側の最大半径を指定します。
// edgeWidth: フェードのエッジの幅を指定します。
struct FADE_BUFFER
{
    DirectX::XMFLOAT2 centerUV          = XMFLOAT2(0.0f, 0.0f);
    float             fadeProgress      = 0.0f;
    float             maxInnerRadius    = 0.0f;
    float             edgeWidth         = 0.0f;
    float             pad[3]            = {0.0f, 0.0f, 0.0f};
};

// 中心からフェードするクラス
// 中心からフェードするエフェクトを実装するクラスです。
// FADE_BUFFER構造体を使用して、フェードの進行状況や中心位置、内側の最大半径、エッジの幅などのパラメータを管理します。
class CenterFade
{
private:
    // フェード用コンスタントバッファ
    struct ConstanctBufferFade
    {
        FADE_BUFFER fadeBuffer;
    };

    FADE_BUFFER m_FadeBufferData;
    ComPtr<ID3D11Buffer> m_pConstantBufferFade;

public:
    CenterFade(){}
    CenterFade(FADE_BUFFER cFade) {
        Create(cFade);
    }
    ~CenterFade() {
        Uninit();
    }

    bool Create(FADE_BUFFER cFade)
    {
        ID3D11Device* dev;
        dev = Renderer::GetDevice();

        // コンスタントバッファ作成
        bool sts = CreateConstantBuffer(
            dev,                                        // デバイス
            sizeof(ConstanctBufferFade),                // サイズ
            m_pConstantBufferFade.GetAddressOf());      // コンスタントバッファ４

        if (!sts) {
            MessageBox(NULL, "CreateBuffer(constant buffer Fade) error", "Error", MB_OK);
            return false;
        }

        m_FadeBufferData.centerUV       = cFade.centerUV      ;
        m_FadeBufferData.fadeProgress   = cFade.fadeProgress  ;
        m_FadeBufferData.maxInnerRadius = cFade.maxInnerRadius;
        m_FadeBufferData.edgeWidth      = cFade.edgeWidth     ;

        Update();

        return true;
    }

    void Update()
    {
        ConstanctBufferFade cb{};
        cb.fadeBuffer = m_FadeBufferData;
        ID3D11DeviceContext* devcontext;
        devcontext = Renderer::GetDeviceContext();
        // コンスタントバッファ更新
        devcontext->UpdateSubresource(
            m_pConstantBufferFade.Get(),    // 更新するバッファ
            0,                              // サブリソースインデックス
            nullptr,                        // 更新する領域（全領域更新なのでnullptr）
            &cb,                            // 更新データへのポインタ
            0,                              // 行サイズ（構造体単位で更新するので0）
            0);                             // スライスサイズ（構造体単位で更新するので0）
        // ピクセルシェーダーにセット
        devcontext->PSSetConstantBuffers(
            6,                              // スロット番号
            1,                              // セットするバッファ数
            m_pConstantBufferFade.GetAddressOf()); // コンスタントバッファのアドレス
    }

    void Uninit()
    {
        m_pConstantBufferFade.Reset();
    }

    void SetGPU()
    {
        ID3D11DeviceContext* devcontext;
        devcontext = Renderer::GetDeviceContext();

        // コンスタントバッファをｂ6レジスタへセット（ピクセルシェーダー用）
        devcontext->PSSetConstantBuffers(6, 1, m_pConstantBufferFade.GetAddressOf());

    }

    void SetFadeProgress(float progress) {
        m_FadeBufferData.fadeProgress = progress;
    }

    void SetCenterUV(DirectX::XMFLOAT2 centerUV) {
        m_FadeBufferData.centerUV = centerUV;
    }

    void SetMaxInnerRadius(float radius) {
        m_FadeBufferData.maxInnerRadius = radius;
    }

};



class Zoom : public TransitionBase
{
private:
    std::unique_ptr<CenterFade> m_CenterFade{};

public:
    Zoom(Camera& cam);
    void Initialize()       override;  
    void Update(float tick) override;      
    void Draw()             override;        
    void Finalize()         override;    

    void ZOOM_IN(float tick);
    void ZOOM_OUT(float tick);
    
    void SetTransMode(TRANS_MODE transMode) override;
};

