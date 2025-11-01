#pragma once
#include <memory>
#include <wrl/client.h>
#include <d3d11.h>

#include "ObjectFile/Object.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Material.h"

/// @brief トランジションの基底クラス
/// @param トランジションエフェクトを実装するための基底クラスです。
/// @param このクラスは、描画に必要なメッシュ情報、テクスチャ、マテリアル、UV座標、タイマー情報を管理します。
/// @param 派生クラスは、Initialize、Update、Draw、Finalizeの各メソッドを実装する必要があります。
/// @param また、タイマーの進行状況を確認するためのisFinishメソッドも提供しています。
/// @param Updateメソッド内でタイマーを進行させ、Drawメソッド内で描画処理を実装してください。
class TransitionBase : public Object
{
protected:
    // 描画の為の情報（メッシュに関わる情報）
    IndexBuffer             m_IndexBuffer ; // インデックスバッファ
    VertexBuffer<VERTEX_3D> m_VertexBuffer; // 頂点バッファ

    // 描画の為の情報（見た目に関わる部分）
    std::shared_ptr<Texture>  m_Texture;	// テクスチャ
    std::unique_ptr<Material> m_Materiale;	//マテリアル

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_InputSRV;    

    // UV座標の情報
    float m_NumU   = 1.0f;
    float m_NumV   = 1.0f;
    float m_SplitX = 1.0f;
    float m_SplitY = 1.0f;

    float m_Timer    = 0.0f;    // タイマー
    float m_Limit    = 1.0f;    // タイマーの上限値
    float m_Duration = 1.0f;    // フェードの持続時間

public:
    
    //
    TransitionBase() = default ;
    TransitionBase(Camera* cam);
    virtual ~TransitionBase() = default;
    virtual void Initialize() = 0;
    virtual void Update()     = 0;
    virtual void Draw()       = 0;
    virtual void Finalize()   = 0;
    

    void ResetTimer() { m_Timer = 0.0f; }
    void CountTimer(const float& delta) { m_Timer += delta; }

    bool isFinish() { return m_Timer >= m_Limit; }              // 終了判定

    
    // SRVを直接セット
    // Textureを触らずに、派生がSRVをそのまま使える
    void SetTextureSRV(ID3D11ShaderResourceView* srv){ m_InputSRV = srv; }
    ID3D11ShaderResourceView* GetTextureSRV() const { return m_InputSRV.Get(); }
    void ClearTextureSRV() { m_InputSRV.Reset(); }

    // テクスチャを指定
    void SetTexture(const char* imgname);
    void SetTexture(std::shared_ptr<Texture> texture);

    // UV座標を指定
    void SetUV(const float& nu, const float& nv, const float& sx, const float& sy);

    // 
    // タイマー関連のセッター・ゲッター
    // 
    void SetTimer(const float& timer) { m_Timer = timer; }
    void SetLimit(const float& limit) { m_Limit = limit; }
    void SetDuration(const float& duration) { m_Duration = duration; }

    float GetTimer() const { return m_Timer; }

    // 上記３つの値をまとめて指定  
    /// @brief タイマー情報をまとめて設定する
    /// @param timer    現在のタイマー値
    /// @param limit    タイマーの上限値
    /// @param duration フェードの持続時間
    void SetTimerInfo(const float& timer, const float& limit, const float& duration) {
        m_Timer    = timer;
        m_Limit    = limit;
        m_Duration = duration;
    }

};

