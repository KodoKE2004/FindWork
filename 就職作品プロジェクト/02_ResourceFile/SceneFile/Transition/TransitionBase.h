#pragma once
#include <memory>
#include <wrl/client.h>
#include <d3d11.h>

#include "Object.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Material.h"

// トランジションのフェーズ
// 遷移の進行状況を示す列挙型
enum class PHASE
{
    TRANS_IN,
    TRANS_OUT,
    FINISH
};

// トランジションのモード
// トランジションエフェクトの種類を示す列挙型
enum class TRANS_MODE
{
    FADE,
    WIPE_LEFT_TO_RIGHT,
    WIPE_RIGHT_TO_LEFT,
    WIPE_TOP_TO_BOTTOM,
    WIPE_BOTTOM_TO_TOP,
    ZOOM_IN,
    ZOOM_OUT,
    NUM
};

enum class EASING_TYPE
{
    NONE,
    IN_SINE,
    IN_QUAD,
    IN_CUBIC,
    IN_QUINT,
    IN_QUART,
    IN_EXPO,
    IN_CIRC,
    IN_BACK,
    IN_BOUNCE,
    IN_ELASTIC,
    OUT_SINE,
    OUT_QUAD,
    OUT_CUBIC,
    OUT_QUINT,
    OUT_QUART,
    OUT_CIRC,
    OUT_BACK,
    OUT_ElASTIC,
    OUT_BOUNCE,
    OUT_EXPO,
    IN_OUT_SINE,
    IN_OUT_QUAD,
    IN_OUT_CUBIC,
    IN_OUT_QUINT,
    IN_OUT_QUART,
    IN_OUT_CIRC,
    IN_OUT_BACK,
    IN_OUT_ELASTIC,
    IN_OUT_BOUNCE,
    IN_OUT_EXPO,
    NUM
};

struct SceneTransitionParam
{
    TRANS_MODE  mode     = TRANS_MODE::FADE;
    float       duration = 1.0f;
    EASING_TYPE easing   = EASING_TYPE::NONE;

    int ModeAsIndex()   const { return static_cast<int>(mode);   }
    int EasingAsIndex() const { return static_cast<int>(easing); }
};

struct TransitionState
{
    SceneTransitionParam inParam;
    SceneTransitionParam outParam;
};

static TransitionState TitleToWait;
static TransitionState WaitToGame;
static TransitionState WaitToResult;
static TransitionState ResultToTitle;

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
    PHASE m_Phase;
    TRANS_MODE m_TransMode;

    // 遷移演出のパラメータ
    SceneTransitionParam m_InParam{};
    SceneTransitionParam m_OutParam{};

    bool m_isChange = false;

    // UV座標の情報
    float m_NumU   = 1.0f;
    float m_NumV   = 1.0f;
    float m_SplitX = 1.0f;
    float m_SplitY = 1.0f;

    float m_Duration = 1.0f;    // 遷移の所要時間

public:

    TransitionBase() = default ;
    TransitionBase(Camera* cam);
    virtual ~TransitionBase() = default;
    virtual void Initialize() = 0;
    void Update(){};
    virtual void Update(float tick) = 0;
    virtual void Draw()       = 0;
    virtual void Finalize()   = 0;

    // SRVを直接セット
    // Textureを触らずに、派生がSRVをそのまま使える
    void SetTextureSRV(ID3D11ShaderResourceView* srv){ m_InputSRV = srv; }
    ID3D11ShaderResourceView* GetTextureSRV() const { return m_InputSRV.Get(); }
    void ClearTextureSRV() { m_InputSRV.Reset(); }


    //--------------------------------
    //          遷移パラメータ群
    //--------------------------------
    float EvaluateEasing(const SceneTransitionParam& param, float t);
    void  ApplyPhaseSetting(PHASE phase);

    //===============================
    //         セッター・ゲッター
    //===============================
    
    // テクスチャ
    void SetTexture(const char* imgname);
    void SetTexture(std::shared_ptr<Texture> texture);

    // UV座標
    void SetUV(const float& nu, const float& nv, const float& sx, const float& sy);
     
    // タイマー関連
    void SetDuration(const float& duration) { m_Duration = duration; }
    
    void SetTransitionParams(const SceneTransitionParam& inParam, const SceneTransitionParam& outParam)
    {
        m_InParam = inParam;
        m_OutParam = outParam;
    }

    // フェーズによってIN・OUTのパラメータを返す
    const SceneTransitionParam& GetParamForPhase(PHASE phase) const
    {
        return phase == PHASE::TRANS_IN ? m_InParam : m_OutParam;
    }

    float GetDurationForPhase(PHASE phase) const
    {
        return max(GetParamForPhase(phase).duration, 0.0001f);
    }
    
    
    // トランジションモード
    virtual void SetTransMode(TRANS_MODE transMode) = 0;    // 純粋仮想関数
    TRANS_MODE GetTransMode() {
        return m_TransMode;
    }
    
    // 遷移状況の変数
    void  SetPhase(PHASE phase) { m_Phase = phase; }
    PHASE GetPhase()            { return m_Phase; }
    bool IsChange()             { return m_isChange; }

};

