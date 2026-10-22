#pragma once

#include	"Transform.h"
#include    <memory>
#include <cstddef>
#include	<d3d11.h>
#include	<io.h>
#include	<string>
#include	<vector>
#include	<wrl/client.h>
#include	<DirectXMath.h>
#include	<SimpleMath.h>

#include	"RenderTarget.h"
#include    "Application.h"

using namespace DirectX;
using namespace SimpleMath;
using Microsoft::WRL::ComPtr;

//外部ライブラリ
//#pragma comment(lib,"DirectXTK.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"d3dcompiler.lib")

/// @brief 3DゲームのVretex情報
/// @param position  = 座標情報
/// @param normal	 = 法線情報
/// @param color	 = 色情報
/// @param uv	     = UV座標
struct VERTEX_3D
{
	NVector3 position;
	NVector3 normal;
	Color   color;
	Vector2 uv;
};

// ブレンドステート
enum EBlendState {
	BS_NONE = 0,							// 半透明合成無し
	BS_ALPHABLEND,							// 半透明合成
	BS_ADDITIVE,							// 加算合成
	BS_SUBTRACTION,							// 減算合成
	MAX_BLENDSTATE
};

//サブセット
struct SUBSET {

	std::string MtrlName;			//マテリアル名
	unsigned int IndexNum    = 0;	//インデックス数
	unsigned int VertexNum   = 0;	//頂点数
	unsigned int IndexBase   = 0;	//開始インデックス
	unsigned int VertexBase  = 0;	//頂点ベース
	unsigned int MaterialIdx = 0;	//マテリアルの番号

};

//マテリアル
struct MATERIAL {
	DirectX::SimpleMath::Color Ambient;		//環境反射
	DirectX::SimpleMath::Color Diffuse;		//拡散反射
	DirectX::SimpleMath::Color Specular;	//鏡面反射
	DirectX::SimpleMath::Color Emission;	//発光
	float Shiness;							//光沢の滑らかさ
	BOOL TextureEnable;						//テクスチャを使うか否かのフラグ
	BOOL Dummy[2]{};
};

struct LightBuffer
{
	DirectX::SimpleMath::Vector4 Direction;	// ライトの方向
	DirectX::SimpleMath::Color LightColor;			// ライトの色
	float AmbientIntensity;							// 環境光の強さ
	float DiffuseIntensity;							// 拡散光の強さ
	float SpecularIntensity;						// 鏡面光の強さ
	float Padding;									// パディング（アライメント調整用）
};

// Absolute world transforms; tint multiplies the shared material/vertex color.
// UV = base UV * uvRect.zw + uvRect.xy, then the shared texture transform.
struct InstanceData
{
    Matrix world = Matrix::Identity;
    Color color = Color(1, 1, 1, 1);
    Vector4 uvRect = Vector4(0, 0, 1, 1);
};

// Slot 1 ABI, shared with Common.hlsl. Matrices are rows, NOT transposed.
struct InstanceVertex
{
    Matrix world;
    Matrix normal;
    Color color;
    Vector4 uvRect;
};
static_assert(sizeof(VERTEX_3D) == 48, "Vertex input layout mismatch");
static_assert(sizeof(InstanceVertex) == 160, "Instance input layout mismatch");
static_assert(offsetof(InstanceVertex, normal) == 64);
static_assert(offsetof(InstanceVertex, color) == 128);
static_assert(offsetof(InstanceVertex, uvRect) == 144);
class VertexShader;

class Renderer
{
public:
	//---------------------------------
	//		コンストラクタとデストラクタ
	//---------------------------------
	
	// デフォルトコンストラクタ
	Renderer() = default;		
	// デストラクタ
	~Renderer() = default;		
	//---------------------------------
	//			ループ内の処理
	//---------------------------------

	static void Initialize(D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_HARDWARE);

	static void Finalize();

	static void Start();

	static void EndGameRender();
	static void ClearBackBuffer(const float clearColor[4] = nullptr);
	static void Finish();

	//---------------------------------
	//	 セッターとシングルトンパターンの実装
	//---------------------------------
	static void SetDepthEnable(bool Enable);
	static void SetATCEnable(bool Enable);
	// フルスクリーン合成用に描画状態を既定化する
	// (State破壊やScissor/Blend/Depth/Rasterizerの影響を排除するため)
	static void ResetForFullscreenPass(ID3D11DeviceContext* ctx, UINT w, UINT h);

	static void SetWorldViewProjection2D();
	static void SetWorldMatrix	   (DirectX::SimpleMath::Matrix* WorldMatrix);
	static void SetViewMatrix	   (DirectX::SimpleMath::Matrix* ViewMatrix);
	static void SetProjectionMatrix(DirectX::SimpleMath::Matrix* ProjectionMatrix);

	static void SetMaterial(MATERIAL Material);
	static void SetLightBuffer(LightBuffer* LightBuffer);
	static void SetUV(float u, float v, float uw, float vh);
	static void SetBlendState(int nBlendState);		// ブレンド ステート設定
    static void SetDefaultInputLayout();			// デフォルトの入力レイアウト設定
	static void BindDefaultSamplers();
	static void BindDefaultConstantBuffers();

    // Scoped submission preserves ordinary Draw() implementations and subsets.
    class ScopedInstances
    {
        const std::vector<InstanceData>* previous;
        VertexShader* previousShader;
    public:
        ScopedInstances(const std::vector<InstanceData>& instances, VertexShader& shader);
        ~ScopedInstances();
        ScopedInstances(const ScopedInstances&) = delete;
        ScopedInstances& operator=(const ScopedInstances&) = delete;
    };
    static void DrawIndexed(UINT count, UINT startIndex = 0, INT baseVertex = 0);
    static bool DrawIndexedInstanced(UINT count, UINT startIndex, INT baseVertex,
        const std::vector<InstanceData>& instances, VertexShader& shader);

	// ゲッター
	static D3D11_VIEWPORT		   GetRenderTargetView(void);

	// 追加実装: オフスクリーン連携ヘルパ =====
	static D3D11_VIEWPORT		   GetViewport();
	static ID3D11RenderTargetView* GetBackBufferRTV();
	static ID3D11ShaderResourceView* GetGameRenderSRV();
	
	static void BlitSRVToBackbuffer(ID3D11ShaderResourceView* srv);
	static ID3D11ShaderResourceView* BlitSRVToTexture(ID3D11ShaderResourceView* srv,float alpha = 1.0f);
	static void CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
	static void CreatePixelShader (ID3D11PixelShader** PixelShader, const char* FileName);
	// 描画設定書き込み
	static void SetDrawState();
	
private:
	static D3D_FEATURE_LEVEL				m_FeatureLevel;			// Direct3Dの機能レベル

	static ID3D11Device*			 m_Device			 ;	// Direct3Dデバイス
	static ID3D11DeviceContext*		 m_DeviceContext	 ;	// デバイスコンテキスト
	static IDXGISwapChain*			 m_SwapChain		 ;	// スワップチェーン
	static ID3D11RenderTargetView*	 m_RenderTargetView	 ;	// レンダーターゲットビュー
	static ID3D11DepthStencilView*	 m_DepthStencilView	 ;	// 深度ステンシルビュー
    static D3D11_VIEWPORT			m_BackBufferViewport ;	// バックバッファのビューポート

	static ID3D11DepthStencilState* m_DepthStateEnable ;	// 深度ステンシルステート（有効）
	static ID3D11DepthStencilState* m_DepthStateDisable;	// 深度ステンシルステート（無効）

	static ID3D11Buffer* m_WorldBuffer		;				// ワールド行列バッファ
	static ID3D11Buffer* m_ViewBuffer		;				// ビュー行列バッファ
	static ID3D11Buffer* m_ProjectionBuffer	;				// プロジェクション行列バッファ

	static ID3D11Buffer* m_MaterialBuffer	;				// マテリアル情報バッファ
	static ID3D11Buffer* m_TextureBuffer	;				// プロジェクション行列バッファ
	static ID3D11Buffer* m_LightBuffer;						// ライト情報バッファ

	static ID3D11BlendState* m_BlendState[MAX_BLENDSTATE];	// ブレンド ステート;
	static ID3D11BlendState* m_BlendStateATC			 ;	// ブレンド ステート（加算合成）
	static ID3D11SamplerState* m_DefaultSampler;			// デフォルトサンプラー
	static std::unique_ptr<RenderTarget> m_GameRenderTarget;
	static std::unique_ptr<RenderTarget> m_BlitRenderTarget;

	static UINT m_GameRenderWidth;
	static UINT m_GameRenderHeight;
	static UINT m_BlitRenderWidth;
	static UINT m_BlitRenderHeight;

	static void EnsureGameRenderTarget();
	static void EnsureBlitRenderTarget();
    static ComPtr<ID3D11Buffer> m_InstanceBuffer;
    static const std::vector<InstanceData>* m_Instances;
    static VertexShader* m_InstanceShader;
    static bool CreateInstanceBuffer();
public:
	static ID3D11Device*		GetDevice	    (void)	{ return m_Device; }
	static ID3D11DeviceContext* GetDeviceContext(void)	{ return m_DeviceContext; }

	static float GetScreenWidth();
    static float GetScreenHeight();
};

inline float Renderer::GetScreenWidth()
{
	return static_cast<float>(Application::GetWidth());
}

inline float Renderer::GetScreenHeight()
{
	return static_cast<float>(Application::GetHeight());
}
