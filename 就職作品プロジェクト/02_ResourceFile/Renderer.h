#pragma once

#include	"Transform.h"
#include	<d3d11.h>
#include	<io.h>
#include	<string>
#include	<vector>
#include	<wrl/client.h>
#include	<DirectXMath.h>
#include	<SimpleMath.h>

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

struct InstancePaked
{
	UINT instanceID;
	UINT pad0;
	UINT pad1;
	BOOL isEnable;
};

class Renderer
{
public:
	//================================
	//		コンストラクタとデストラクタ
	//================================
	
	// デフォルトコンストラクタ
	Renderer() = default;		
	// デストラクタ
	~Renderer() = default;		
	//================================
	//			ループ内の処理
	//================================

	// レンダラーの初期化
	static void Initialize();
	// レンダラーの終了処理
	static void Finalize();
	// 描画の開始
	static void Start();
	// 描画の終了
	static void Finish();

	//================================
	//	 セッターとシングルトンパターンの実装
	//================================
	static void SetDepthEnable(bool Enable);
	static void SetATCEnable(bool Enable);

	static void SetWorldViewProjection2D();
	static void SetWorldMatrix	   (DirectX::SimpleMath::Matrix* WorldMatrix);
	static void SetViewMatrix	   (DirectX::SimpleMath::Matrix* ViewMatrix);
	static void SetProjectionMatrix(DirectX::SimpleMath::Matrix* ProjectionMatrix);

	static void SetMaterial(MATERIAL Material);
	static void SetLightBuffer(LightBuffer* LightBuffer);
	static void SetUV(float u, float v, float uw, float vh);
	static void SetBlendState(int nBlendState);		// ブレンド ステート設定
    static void SetDefaultInputLayout();			// デフォルトの入力レイアウト設定

	// インスタンシング描画テンプレート
	template<class T> static void SetInstanceID(T* method,InstancePaked iPaked)
	{
		
	}

	// ゲッター
	static D3D11_VIEWPORT		   GetRenderTargetView(void);

	// 追加実装: オフスクリーン連携ヘルパ =====
	static D3D11_VIEWPORT		   GetViewport();
	static ID3D11RenderTargetView* GetBackBufferRTV();
	
	static void BlitSRVToBackbuffer(ID3D11ShaderResourceView* srv, float alpha = 1.0f);
	static void CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName);
	static void CreatePixelShader (ID3D11PixelShader** PixelShader, const char* FileName);
	
	// 描画設定書き込み
	static void SetDrawState();
	
private:
	static D3D_FEATURE_LEVEL				m_FeatureLevel;			// Direct3Dの機能レベル

	static ID3D11Device*			m_Device			;	// Direct3Dデバイス
	static ID3D11DeviceContext*		m_DeviceContext		;	// デバイスコンテキスト
	static IDXGISwapChain*			m_SwapChain			;	// スワップチェーン
	static ID3D11RenderTargetView*	m_RenderTargetView	;	// レンダーターゲットビュー
	static ID3D11DepthStencilView*	m_DepthStencilView	;	// 深度ステンシルビュー

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
	
	inline std::vector<UINT> MakeInstanceIDs(UINT count);
	inline HRESULT CreateInstanceIDBuffer(const std::vector<UINT>& ids,
										  ID3D11Buffer** outVB,
										  D3D11_USAGE usage = D3D11_USAGE_IMMUTABLE,
										  UINT cpuAccess = 0);
	inline void BindInstanceIDs(ID3D11Buffer* vb, UINT slot = 1);
	inline D3D11_INPUT_ELEMENT_DESC MakeInstancneIDElement(UINT semanticIndex = 1,
														  UINT slot = 1,
														  UINT alignedByteOffset = 0,
														  UINT stepRate = 1);

public:
	static ID3D11Device*		GetDevice	    (void)	{ return m_Device; }
	static ID3D11DeviceContext* GetDeviceContext(void)	{ return m_DeviceContext; }
};

inline std::vector<UINT> Renderer::MakeInstanceIDs(UINT count)
{
	std::vector<UINT> v(count);
	for (UINT i = 0; i < count; ++i) v[i] = i;
	return v;
}

inline HRESULT Renderer::CreateInstanceIDBuffer(const std::vector<UINT>& ids,
	ID3D11Buffer** outVB,
	D3D11_USAGE usage,
	UINT cpuAccess)
{
	if (!m_Device || !outVB) return E_FAIL;

	D3D11_BUFFER_DESC bd{};
	bd.ByteWidth = static_cast<UINT>(ids.size() * sizeof(UINT));
	bd.Usage = usage;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = cpuAccess;

	D3D11_SUBRESOURCE_DATA init{};
	init.pSysMem = ids.empty() ? nullptr : ids.data();

	return m_Device->CreateBuffer(&bd, ids.empty() ? nullptr : &init, outVB);
}

inline void Renderer::BindInstanceIDs(ID3D11Buffer* vb, UINT slot)
{
	UINT stride = sizeof(UINT);
	UINT offset = 0;
	m_DeviceContext->IASetVertexBuffers(slot, 1, &vb, &stride, &offset);
}

inline D3D11_INPUT_ELEMENT_DESC Renderer::MakeInstancneIDElement(UINT semanticIndex, UINT slot, UINT alignedByteOffset, UINT stepRate)
{
	D3D11_INPUT_ELEMENT_DESC e{};
	e.SemanticName = "TEXCOORD";
	e.SemanticIndex = semanticIndex;					// 既定: TEXCOORD1
	e.Format = DXGI_FORMAT_R32_UINT;					// UINT 1個
	e.InputSlot = slot;									// 既定: slot1
	e.AlignedByteOffset = alignedByteOffset;            // 既定: 0
	e.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	e.InstanceDataStepRate = stepRate;                  // 既定: 1
	return e;
}


