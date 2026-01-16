#pragma once
#include <string>
#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include "imgui.h"
#include "Object.h"
#include "Camera.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Material.h"
#include "Calculator.h"

using namespace Math::Easing;

// トランジションのフェーズ
// 遷移の進行状況を示す列挙型
enum class TRANS_PHASE
{
    NONE,
    TRANS_IN,
    TRANS_OUT,
    FINISH,
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


struct SceneTransitionParam
{
    TRANS_MODE  mode     = TRANS_MODE::FADE;
    float       duration = 1.0f;
    EASING_TYPE easing   = EASING_TYPE::NONE;
    std::shared_ptr<Texture> texture = nullptr;

    int ModeAsIndex()   const { return static_cast<int>(mode);   }
    int EasingAsIndex() const { return static_cast<int>(easing); }
};

extern SceneTransitionParam TitleToWait;
extern SceneTransitionParam WaitToGame;
extern SceneTransitionParam GameToWait;
extern SceneTransitionParam WaitToResult;
extern SceneTransitionParam ResultToTitle;
extern SceneTransitionParam ResultToGame;

bool SaveTransitionSettingsToCsv  (const std::string& filePath, std::string& errorMessage);
bool LoadTransitionSettingsFromCsv(const std::string& filePath, std::string& errorMessage);

void DrawTransitionStateGUI();

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
    TRANS_PHASE m_Phase;
    TRANS_MODE m_TransMode;

    // 遷移演出のパラメータ
    SceneTransitionParam m_transParam{};

    bool m_isChange = false;

    // UV座標の情報
    float m_NumU   = 1.0f;
    float m_NumV   = 1.0f;
    float m_SplitX = 1.0f;
    float m_SplitY = 1.0f;

    float m_Duration = 1.0f;    // 遷移の所要時間

    // デバッグ用: 単色フルスクリーンを描けるようにする
    bool m_DebugSolidDraw = false;
    Color m_DebugSolidColor = Color(1.0f, 0.0f, 1.0f, 1.0f);

    // 描画パイプラインの既定化（他の描画で状態が変わっても影響を受けないようにする）
    void SetPipeline();
    void DrawDebugFullscreenSolid();


public:

    TransitionBase() = default ;
    TransitionBase(Camera& cam);
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
    void  ApplyPhaseSetting(TRANS_PHASE phase);

    //===============================
    //         セッター・ゲッター
    //===============================
    
    // テクスチャ
    void SetTexture(const char* imgname);
    void SetTexture(std::shared_ptr<Texture> texture);

    // UV座標
    void SetUV(const float& nu, const float& nv, const float& sx, const float& sy);

    // デバッグ用フルスクリーン描画の切替
    void SetDebugSolidDraw(bool enable, const Color& color = Color(1.0f, 0.0f, 1.0f, 1.0f))
    {
        m_DebugSolidDraw = enable;
        m_DebugSolidColor = color;
    }
     
    // タイマー関連
    void SetDuration(const float& duration) { m_Duration = duration; }
    
    void SetTransitionParams(SceneTransitionParam param)
    {
        m_transParam = param;
    }

    float GetDurationForPhase(TRANS_PHASE phase) const
    {
        return max(m_transParam.duration, 0.0001f);
    }
    
    
    // トランジションモード
    virtual void SetTransMode(TRANS_MODE transMode) = 0;    // 純粋仮想関数
    TRANS_MODE GetTransMode() {
        return m_TransMode;
    }
    
    // 遷移状況の変数
    void  SetPhase(TRANS_PHASE phase) { m_Phase = phase; }
    TRANS_PHASE GetPhase()            { return m_Phase; }
    bool IsChange()             { return m_isChange; }

};
