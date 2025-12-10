#include "Renderer.h"
#include "Application.h"
#include "Debug.hpp"
#include <d3dcompiler.h>
#include <algorithm>

#pragma comment(lib, "d3dcompiler.lib")


D3D_FEATURE_LEVEL Renderer::m_FeatureLevel = D3D_FEATURE_LEVEL_11_0; // Direct3Dの機能レベル（DirectX 11.0）

ID3D11Device*					Renderer::m_Device;				// Direct3Dデバイス
ID3D11DeviceContext*			Renderer::m_DeviceContext;		// デバイスコンテキスト
IDXGISwapChain*					Renderer::m_SwapChain;			// スワップチェーン
ID3D11RenderTargetView*			Renderer::m_RenderTargetView;	// レンダーターゲットビュー
ID3D11DepthStencilView*			Renderer::m_DepthStencilView;	// 深度ステンシルビュー

D3D11_VIEWPORT                              Renderer::m_BackBufferViewport;

#ifdef _DEBUG
std::unique_ptr<RenderTarget>               Renderer::m_DebugGameTarget;
D3D11_VIEWPORT                              Renderer::m_DebugPresentViewport;
#endif


ID3D11DepthStencilState*		Renderer::m_DepthStateEnable;	// 深度ステンシルステート（有効）
ID3D11DepthStencilState*		Renderer::m_DepthStateDisable;	// 深度ステンシルステート（無効）

ID3D11Buffer*					Renderer::m_WorldBuffer;		// ワールド行列バッファ
ID3D11Buffer*					Renderer::m_ViewBuffer;			// ビュー行列バッファ
ID3D11Buffer*					Renderer::m_ProjectionBuffer;	// プロジェクション行列バッファ

ID3D11Buffer*					Renderer::m_MaterialBuffer;		// マテリアル設定
ID3D11Buffer*					Renderer::m_TextureBuffer;		// テクスチャ設定
ID3D11Buffer*					Renderer::m_LightBuffer;		// ライト設定

ID3D11BlendState*				Renderer::m_BlendState[MAX_BLENDSTATE];		// ブレンド ステート
ID3D11BlendState*				Renderer::m_BlendStateATC;


void Renderer::Initialize()
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーン作成
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 1;									// バックバッファの数を1に設定（ダブルバッファリング）
	swapChainDesc.BufferDesc.Width  = Application::GetGameWidth();	// バッファの幅をウィンドウサイズに合わせる
	swapChainDesc.BufferDesc.Height = Application::GetGameHeight();	// バッファの高さをウィンドウサイズに合わせる
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// バッファのピクセルフォーマットを設定
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;			// リフレッシュレートを設定（Hz）
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;			// リフレッシュレートの分母を設定（1で60Hz）
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// バッファの使用用途を設定
	swapChainDesc.OutputWindow = Application::GetWindow();			// スワップチェーンのターゲットウィンドウを設定
	swapChainDesc.SampleDesc.Count = 1;								// マルチサンプリングの設定（アンチエイリアスのサンプル数とクオリティ）
	swapChainDesc.SampleDesc.Quality = 0;							//同上
	swapChainDesc.Windowed = TRUE;									// ウィンドウモード（フルスクリーンではなく、ウィンドウモードで実行）

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0,
		D3D11_SDK_VERSION, &swapChainDesc, &m_SwapChain, &m_Device, &m_FeatureLevel, &m_DeviceContext);
	if (FAILED(hr)) return;

	// レンダーターゲットビュー作成
	ID3D11Texture2D* rt = nullptr;
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&rt);
	if (SUCCEEDED(hr) && rt) {
		hr = m_Device->CreateRenderTargetView(rt, nullptr, &m_RenderTargetView);
	}
	if (rt) rt->Release();
	if (FAILED(hr)) return;

	// デプスステンシルバッファ作成
	ID3D11Texture2D* depthStencile{};
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = swapChainDesc.BufferDesc.Width;   // バッファの幅をスワップチェーンに合わせる
	textureDesc.Height = swapChainDesc.BufferDesc.Height; // バッファの高さをスワップチェーンに合わせる
	textureDesc.MipLevels = 1;                            // ミップレベルは1（ミップマップは使用しない）
	textureDesc.ArraySize = 1;                            // テクスチャの配列サイズ（通常1）
	textureDesc.Format = DXGI_FORMAT_D16_UNORM;           // フォーマットは16ビットの深度バッファを使用
	textureDesc.SampleDesc = swapChainDesc.SampleDesc;    // スワップチェーンと同じサンプル設定
	textureDesc.Usage = D3D11_USAGE_DEFAULT;              // 使用方法はデフォルト（GPUで使用）
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;     // 深度ステンシルバッファとして使用
	textureDesc.CPUAccessFlags = 0;                       // CPUからのアクセスは不要
	textureDesc.MiscFlags = 0;                            // その他のフラグは設定なし

	hr = m_Device->CreateTexture2D(&textureDesc, NULL, &depthStencile);
	if (FAILED(hr)) return;

	// デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = textureDesc.Format;					// デプスステンシルバッファのフォーマットを設定
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // ビューの次元を2Dテクスチャとして設定（2Dテクスチャ用のデプスステンシルビュー）
	depthStencilViewDesc.Flags = 0; // 特別なフラグは設定しない（デフォルトの動作）
	if (depthStencile != nullptr)m_Device->CreateDepthStencilView(depthStencile, &depthStencilViewDesc, &m_DepthStencilView);
	depthStencile->Release();

	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView); // レンダーターゲットとデプスステンシルビューを設定

	// ビューポート設定
	m_BackBufferViewport.Width  = (FLOAT)Application::GetGameWidth();   // ビューポートの幅
	m_BackBufferViewport.Height = (FLOAT)Application::GetGameHeight();  // ビューポートの高さ
	m_BackBufferViewport.MinDepth = 0.0f;								// 深度範囲の最小値
	m_BackBufferViewport.MaxDepth = 1.0f;								// 深度範囲の最大値
	m_BackBufferViewport.TopLeftX = 0;									// ビューポートの左上隅のX座標
	m_BackBufferViewport.TopLeftY = 0;									// ビューポートの左上隅のY座標）
	m_DeviceContext->RSSetViewports(1, &m_BackBufferViewport);

#ifdef _DEBUG
	m_DebugGameTarget = std::make_unique<RenderTarget>();
	m_DebugGameTarget->Create(m_Device, Application::GetGameWidth(), Application::GetGameHeight(), true);

	const float aspect = static_cast<float>(Application::GetGameWidth()) / static_cast<float>(Application::GetGameHeight());
	const float maxPreviewWidth  = static_cast<float>(Application::GetWidth())  * 0.8f;
	const float maxPreviewHeight = static_cast<float>(Application::GetHeight()) * 0.8f;
	float previewWidth = min(static_cast<float>(Application::GetGameWidth()), maxPreviewWidth);
	float previewHeight = previewWidth / aspect;
	if (previewHeight > maxPreviewHeight) {
		previewHeight = maxPreviewHeight;
		previewWidth = previewHeight * aspect;
	}

	m_DebugPresentViewport.TopLeftX = 0.0f;
	m_DebugPresentViewport.TopLeftY = 0.0f;
	m_DebugPresentViewport.Width  = previewWidth;
	m_DebugPresentViewport.Height = previewHeight;
	m_DebugPresentViewport.MinDepth = 0.0f;
	m_DebugPresentViewport.MaxDepth = 1.0f;
#endif

	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;

	ID3D11RasterizerState* rs;
	hr = m_Device->CreateRasterizerState(&rasterizerDesc, &rs);
	if (FAILED(hr)) return;

	m_DeviceContext->RSSetState(rs);

	// ブレンド ステート生成
	D3D11_BLEND_DESC BlendDesc = {};
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));                       // BlendDesc構造体をゼロで初期化し、メモリをクリア
	BlendDesc.AlphaToCoverageEnable = FALSE;                         // アルファ・トゥ・カバレッジを無効化（透明度をカバレッジとして利用しない）
	BlendDesc.IndependentBlendEnable = TRUE;                         // 各レンダーターゲットに対して個別のブレンド設定を有効化
	BlendDesc.RenderTarget[0].BlendEnable = FALSE;                   // ブレンドを無効に設定（不透明な描画）
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;      // ソース（描画するピクセル）のアルファ値を使用
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // デスティネーション（既存のピクセル）の逆アルファ値を使用
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;          // ソースとデスティネーションを加算する操作
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;       // ソースのアルファ値をそのまま使用
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;     // デスティネーションのアルファ値を無視
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;     // アルファ値に対して加算操作を行う
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // レンダーターゲットのカラーチャンネル書き込みマスク

	hr = m_Device->CreateBlendState(&BlendDesc, &m_BlendState[0]);
	if (FAILED(hr)) return;

	// ブレンド ステート生成 (アルファ ブレンド用)
	//BlendDesc.AlphaToCoverageEnable = TRUE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	hr = m_Device->CreateBlendState(&BlendDesc, &m_BlendState[1]);
	if (FAILED(hr)) return;

	// ブレンド ステート生成 (加算合成用)
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	hr = m_Device->CreateBlendState(&BlendDesc, &m_BlendState[2]);
	if (FAILED(hr)) return;

	// ブレンド ステート生成 (減算合成用)
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	hr = m_Device->CreateBlendState(&BlendDesc, &m_BlendState[3]);
	if (FAILED(hr)) return;

	SetBlendState(BS_ALPHABLEND);


	// デプスステンシルステート設定
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	hr = m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStateEnable); //深度有効ステート
	if (FAILED(hr)) return;

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStateDisable); //深度無効ステート
	if (FAILED(hr)) return;

	m_DeviceContext->OMSetDepthStencilState(m_DepthStateEnable, NULL);

	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState{};
	hr = m_Device->CreateSamplerState(&samplerDesc, &samplerState);
	if (FAILED(hr)) return;

	m_DeviceContext->PSSetSamplers(0, 1, &samplerState);

	// 定数バッファ生成
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(Matrix);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	hr = m_Device->CreateBuffer(&bufferDesc, NULL, &m_WorldBuffer);
	if (FAILED(hr)) return;
	m_DeviceContext->VSSetConstantBuffers(0, 1, &m_WorldBuffer);

	hr = m_Device->CreateBuffer(&bufferDesc, NULL, &m_ViewBuffer);
	if (FAILED(hr)) return;
	m_DeviceContext->VSSetConstantBuffers(1, 1, &m_ViewBuffer);

	hr = m_Device->CreateBuffer(&bufferDesc, NULL, &m_ProjectionBuffer);
	if (FAILED(hr)) return;
	m_DeviceContext->VSSetConstantBuffers(2, 1, &m_ProjectionBuffer);

	bufferDesc.ByteWidth = sizeof(Matrix);
	hr = m_Device->CreateBuffer(&bufferDesc, NULL, &m_TextureBuffer);
	if (FAILED(hr)) return;
	m_DeviceContext->VSSetConstantBuffers(5, 1, &m_TextureBuffer);



	bufferDesc.ByteWidth = sizeof(MATERIAL);
	hr = m_Device->CreateBuffer(&bufferDesc, NULL,  &m_MaterialBuffer);
	m_DeviceContext->VSSetConstantBuffers(4, 1,		&m_MaterialBuffer);
	m_DeviceContext->PSSetConstantBuffers(4, 1,		&m_MaterialBuffer);
	if (FAILED(hr)) return;

	bufferDesc.ByteWidth = sizeof(LightBuffer);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	hr = m_Device->CreateBuffer(&bufferDesc, nullptr, &m_LightBuffer);
	if (FAILED(hr)) return;

	bufferDesc.ByteWidth = sizeof(InstancePaked);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;


	// 既定値（下向き白色の平行光）
	LightBuffer lightbuffer{};
	lightbuffer.Direction = DirectX::SimpleMath::Vector4(0.0f, 0.0f, -1.0f, 0.0f); // w=0: 方向ベクトル
	lightbuffer.LightColor = DirectX::SimpleMath::Color( 1.0f, 1.0f,  1.0f, 1.0f);
	lightbuffer.AmbientIntensity = 0.5f;
	lightbuffer.DiffuseIntensity = 0.5f;
	lightbuffer.SpecularIntensity = 0.5f;

	m_DeviceContext->UpdateSubresource(m_LightBuffer, 0, NULL, &lightbuffer, 0, 0);

	// ピクセルシェーダにバインド（b3）
	m_DeviceContext->PSSetConstantBuffers(3, 1, &m_LightBuffer);

	//マテリアル初期化
	MATERIAL material{};
	material.Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = Color(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	SetUV(0.0f, 0.0f, 1.0f, 1.0f); // UV行列を初期化
}

void Renderer::Finalize()
{
	for (auto& bs : m_BlendState) { if (bs) { bs->Release(); bs = nullptr; } }
	if (m_BlendStateATC) { m_BlendStateATC->Release(); m_BlendStateATC = nullptr; }

	if (m_DepthStencilView)  { m_DepthStencilView ->Release(); m_DepthStencilView  = nullptr; }
	if (m_DepthStateEnable)  { m_DepthStateEnable ->Release(); m_DepthStateEnable  = nullptr; }
	if (m_DepthStateDisable) { m_DepthStateDisable->Release(); m_DepthStateDisable = nullptr; }

#ifdef _DEBUG
    m_DebugGameTarget.reset();
#endif

	if (m_WorldBuffer)		{ m_WorldBuffer->Release();       m_WorldBuffer		 = nullptr; }
	if (m_ViewBuffer)		{ m_ViewBuffer->Release();        m_ViewBuffer		 = nullptr; }
	if (m_ProjectionBuffer) { m_ProjectionBuffer->Release();  m_ProjectionBuffer = nullptr; }

	if (m_MaterialBuffer) { m_MaterialBuffer->Release();    m_MaterialBuffer = nullptr; }
	if (m_TextureBuffer)  { m_TextureBuffer->Release();     m_TextureBuffer  = nullptr; }
	if (m_LightBuffer)    { m_LightBuffer->Release();       m_LightBuffer    = nullptr; }

	if (m_DeviceContext)  { m_DeviceContext->ClearState(); }

	if (m_RenderTargetView) { m_RenderTargetView->Release();  m_RenderTargetView = nullptr; }
	if (m_SwapChain)		{ m_SwapChain->Release();         m_SwapChain		 = nullptr; }
	if (m_DeviceContext)	{ m_DeviceContext->Release();     m_DeviceContext	 = nullptr; }
	if (m_Device)			{ m_Device->Release();            m_Device			 = nullptr; }
}

void Renderer::Start()
{
	if (!m_DeviceContext || !m_RenderTargetView || !m_DepthStencilView) { return; }
    // バックバッファのクリア
	float clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

#ifdef _DEBUG
	if (m_DebugGameTarget) {
		m_DebugGameTarget->Begin(m_DeviceContext, clearColor);
		return;
	}
#endif
	ID3D11RenderTargetView* rtvs[] = { m_RenderTargetView };
	m_DeviceContext->OMSetRenderTargets(1, rtvs, m_DepthStencilView);
	m_DeviceContext->RSSetViewports(1, &m_BackBufferViewport);
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, clearColor);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Renderer::Finish()
{
	if (!m_SwapChain || !m_DeviceContext) { return; }
	// フレームの表示
	m_SwapChain->Present(1, 0);


#ifdef _DEBUG
	// デバッグレイヤーで未解放オブジェクトをレポート
	ID3D11Debug* debugDev = nullptr;
	if (m_Device && SUCCEEDED(m_Device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugDev))) {
		debugDev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		debugDev->Release();
	}
#endif
}

void Renderer::PresentDebugGameView()
{
#ifdef _DEBUG
	if (!m_DebugGameTarget) { return; }

	ID3D11RenderTargetView* rtvs[] = { m_RenderTargetView };
	m_DeviceContext->OMSetRenderTargets(1, rtvs, nullptr);
	m_DeviceContext->RSSetViewports(1, &m_BackBufferViewport);

	const float background[4] = { 0.0f, 0.0f, 0.5f, 1.0f };
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, background);

	m_DeviceContext->RSSetViewports(1, &m_DebugPresentViewport);
	BlitSRVToBackbuffer(m_DebugGameTarget->GetSRV());
	m_DeviceContext->RSSetViewports(1, &m_BackBufferViewport);
#endif
}

void Renderer::SetDepthEnable(bool Enable)
{
	if (Enable)
	{
		// 深度テストを有効にするステンシルステートをセット
		m_DeviceContext->OMSetDepthStencilState(m_DepthStateEnable, NULL);
	}
	else
	{
		// 深度テストを無効にするステンシルステートをセット
		m_DeviceContext->OMSetDepthStencilState(m_DepthStateDisable, NULL);
	}
}

void Renderer::SetATCEnable(bool Enable)
{
	// ブレンドファクター（透明度などの調整に使用）
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (Enable)
	{
		// アルファテストとカバレッジ (ATC) を有効にするブレンドステートをセット
		m_DeviceContext->OMSetBlendState(m_BlendStateATC, blendFactor, 0xffffffff);
	}
	else
	{
		// 通常のブレンドステートをセット
		m_DeviceContext->OMSetBlendState(m_BlendState[0], blendFactor, 0xffffffff);
	}
}

void Renderer::SetWorldViewProjection2D()
{
	Matrix world = Matrix::Identity;	// 単位行列にする
	world = world.Transpose();			// 転置
	m_DeviceContext->UpdateSubresource(m_WorldBuffer, 0, NULL, &world, 0, 0);

	Matrix view = Matrix::Identity;		// 単位行列にする
	view = view.Transpose();			// 転置
	m_DeviceContext->UpdateSubresource(m_ViewBuffer, 0, NULL, &view, 0, 0);

	// 2D描画を左上原点にする
	Matrix projection = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f,
		static_cast<float>(Application::GetGameWidth()),	// ビューボリュームの最小Ｘ
		static_cast<float>(Application::GetGameHeight()),	// ビューボリュームの最小Ｙ
		0.0f,												// ビューボリュームの最大Ｙ
		0.0f,
		1.0f);

	projection = projection.Transpose();

	m_DeviceContext->UpdateSubresource(m_ProjectionBuffer, 0, NULL, &projection, 0, 0);
}

void Renderer::SetWorldMatrix(DirectX::SimpleMath::Matrix* WorldMatrix)
{
	Matrix world;
	world = WorldMatrix->Transpose(); // 転置

	// ワールド行列をGPU側へ送る
 	m_DeviceContext->UpdateSubresource(m_WorldBuffer, 0, NULL, &world, 0, 0);
}

void Renderer::SetViewMatrix(DirectX::SimpleMath::Matrix* ViewMatrix)
{
	Matrix view;
	view = ViewMatrix->Transpose(); // 転置

	// ビュー行列をGPU側へ送る
	m_DeviceContext->UpdateSubresource(m_ViewBuffer, 0, NULL, &view, 0, 0);
}

void Renderer::SetProjectionMatrix(DirectX::SimpleMath::Matrix* ProjectionMatrix)
{
	Matrix projection;
	projection = ProjectionMatrix->Transpose(); // 転置

	// プロジェクション行列をGPU側へ送る
	m_DeviceContext->UpdateSubresource(m_ProjectionBuffer, 0, NULL, &projection, 0, 0);

}

void Renderer::SetMaterial(MATERIAL Material)
{
	m_DeviceContext->UpdateSubresource(m_MaterialBuffer, 0, NULL, &Material, 0, 0);
}

void Renderer::SetLightBuffer(LightBuffer* LightBuffer)
{
	if (LightBuffer == nullptr) return;
	// ライトバッファをGPU側へ送る
	m_DeviceContext->UpdateSubresource(m_LightBuffer, 0, NULL, LightBuffer, 0, 0);
	// ピクセルシェーダにバインド
	m_DeviceContext->PSSetConstantBuffers(3, 1, &m_LightBuffer);
}


void Renderer::SetUV(float u, float v, float uw, float vh)
{
	//UV行列作成
	Matrix mat = Matrix::CreateScale(uw, vh, 1.0f);
	mat *= Matrix::CreateTranslation(u, v, 0.0f).Transpose();

	m_DeviceContext->UpdateSubresource(m_TextureBuffer, 0, NULL, &mat, 0, 0);

}

void Renderer::SetBlendState(int nBlendState)
{
	if (nBlendState < 0 || nBlendState >= MAX_BLENDSTATE) return;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_DeviceContext->OMSetBlendState(m_BlendState[nBlendState], blendFactor, 0xffffffff);

}

void Renderer::SetDefaultInputLayout()
{
	
}

D3D11_VIEWPORT Renderer::GetRenderTargetView(void)
{
	D3D11_VIEWPORT viewport{};
	UINT count = 1;
	// ビューポートの情報を取得
	m_DeviceContext->RSGetViewports(&count, &viewport);

	// まだ設定されていない等で取得できなかった場合は、デフォルトのビューポートを設定
	if (count == 0) {
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<FLOAT> (Application::GetGameWidth());
		viewport.Height = static_cast<FLOAT>(Application::GetGameHeight());
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
	}

	return viewport;
}

D3D11_VIEWPORT Renderer::GetViewport()
{
	D3D11_VIEWPORT vp{};
	UINT count = 1;
	m_DeviceContext->RSGetViewports(&count, &vp);
	if (count == 0) {
		vp.TopLeftX = 0.0f; vp.TopLeftY = 0.0f;
		vp.Width =  (FLOAT)Application::GetGameWidth();
		vp.Height = (FLOAT)Application::GetGameHeight();
		vp.MinDepth = 0.0f; vp.MaxDepth = 1.0f;
	}
	return vp;
}

ID3D11RenderTargetView* Renderer::GetBackBufferRTV()
{
	return m_RenderTargetView;
}

static void CompileShaderFromSource(const char* src, const char* entry, const char* target, ID3DBlob** blobOut)
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompile(src, strlen(src), nullptr, nullptr, nullptr, entry, target, flags, 0, blobOut, &errorBlob);
	if (FAILED(hr) && errorBlob) { OutputDebugStringA((const char*)errorBlob->GetBufferPointer()); }
	if (errorBlob) errorBlob->Release();
}

// SRV をバックバッファへフルスクリーン合成（αブレンドON/深度OFF）
void Renderer::BlitSRVToBackbuffer(ID3D11ShaderResourceView* srv, float alpha)
{
	if (!srv) return;

	static Microsoft::WRL::ComPtr<ID3D11VertexShader> sVS;
	static Microsoft::WRL::ComPtr<ID3D11PixelShader>  sPS;
	static Microsoft::WRL::ComPtr<ID3D11Buffer>       sCB; // alpha
	static bool sReady = false;

	if (!sReady)
	{
		const char* vsSrc = R"(
		struct VSOut 
		{
			float4 pos:SV_POSITION;
			float2 uv:TEXCOORD0;
		};

		VSOut main(uint id:SV_VertexID)
		{
			float2 v[3] = 
			{
				float2(-1,-1),
				float2(-1,3),
				float2(3,-1)
			};
		
			VSOut o;
			o.pos=float4(v[id],0,1);
			o.uv=0.5f*(v[id]+float2(1,1));

			o.uv.y = 1.0f - o.uv.y;
		
			return o;
		})";

		const char* psSrc = R"(
		Texture2D    gTex : register(t0);
		SamplerState gSmp : register(s0);
		cbuffer CB : register(b7)
		{
			float gAlpha;
			float3 pad;
		};

		float4 main(float4 pos:SV_POSITION, float2 uv:TEXCOORD0) : SV_Target 
		{
			float4 c = gTex.Sample(gSmp, uv);
			c.a = saturate(gAlpha);
		
			return c;
		})";
		Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob;
		CompileShaderFromSource(vsSrc, "main", "vs_5_0", vsBlob.GetAddressOf());
		CompileShaderFromSource(psSrc, "main", "ps_5_0", psBlob.GetAddressOf());
		if (vsBlob) m_Device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, sVS.GetAddressOf());
		if (psBlob) m_Device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, sPS.GetAddressOf());

		D3D11_BUFFER_DESC cbd{}; cbd.ByteWidth = 16; cbd.Usage = D3D11_USAGE_DEFAULT; cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		m_Device->CreateBuffer(&cbd, nullptr, sCB.GetAddressOf());

		sReady = true;
	}

	// OM backbuffer
	ID3D11RenderTargetView* rtvs[] = { m_RenderTargetView };
	m_DeviceContext->OMSetRenderTargets(1, rtvs, nullptr);

	// viewport
	auto vp = GetViewport();
	m_DeviceContext->RSSetViewports(1, &vp);

	// alpha blend ON / depth OFF
	SetBlendState(BS_ALPHABLEND);
	SetDepthEnable(false);

	// bind shaders & SRV
	m_DeviceContext->VSSetShader(sVS.Get(), nullptr, 0);
	m_DeviceContext->PSSetShader(sPS.Get(), nullptr, 0);
	m_DeviceContext->PSSetShaderResources(0, 1, &srv);

	float data[4] = { alpha, 0, 0, 0 };
	m_DeviceContext->UpdateSubresource(sCB.Get(), 0, nullptr, data, 0, 0);
	m_DeviceContext->VSSetConstantBuffers(7, 1, sCB.GetAddressOf());
	m_DeviceContext->PSSetConstantBuffers(7, 1, sCB.GetAddressOf());

	m_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_DeviceContext->Draw(3, 0);

	// unbind SRV（ハザード回避）
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	m_DeviceContext->PSSetShaderResources(0, 1, nullSRV);
}


void Renderer::CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName)
{
	FILE* file; // ファイルを開くためのポインタ
	long int fsize; // ファイルサイズを格納する変数
	fopen_s(&file, FileName, "rb"); // シェーダーファイルをReadBinaryモードで開く
	assert(file); // ファイルが正常に開けたかどうかを確認

	fsize = _filelength(_fileno(file)); // ファイルのサイズを取得
	unsigned char* buffer = new unsigned char[fsize]; // ファイルサイズに基づいてバッファを確保
	fread(buffer, fsize, 1, file); // ファイルからバッファにデータを読み込む
	fclose(file); // 読み込み完了後、ファイルを閉じる

	// デバイスを使って頂点シェーダーを作成
	HRESULT hr = m_Device->CreateVertexShader(buffer, fsize, NULL, VertexShader);

	if (FAILED(hr))
	{
		return;
	}

	// 頂点レイアウト（入力レイアウト）の定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// 頂点の位置情報（3つのfloat値）
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// 頂点の法線ベクトル情報（3つのfloat値）
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	4 * 3,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// 頂点の色情報（4つのfloat値：RGBA）
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	4 * 6,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// 頂点のテクスチャ座標情報（2つのfloat値）
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0,	4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout); // レイアウトの要素数を取得

	// デバイスを使って頂点レイアウトを作成
	m_Device->CreateInputLayout(layout, numElements, buffer, fsize, VertexLayout);

	delete[] buffer; // バッファのメモリを解放
}

void Renderer::CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName)
{
	FILE* file; // ファイルを開くためのポインタ
	long int fsize; // ファイルサイズを格納する変数
	fopen_s(&file, FileName, "rb"); // シェーダーファイルをReadBinaryモードで開く
	assert(file); // ファイルが正常に開けたかどうかを確認

	fsize = _filelength(_fileno(file)); // ファイルのサイズを取得
	unsigned char* buffer = new unsigned char[fsize]; // ファイルサイズに基づいてバッファを確保
	fread(buffer, fsize, 1, file); // ファイルからバッファにデータを読み込む
	fclose(file); // 読み込み完了後、ファイルを閉じる

	// デバイスを使ってピクセルシェーダーを作成
	m_Device->CreatePixelShader(buffer, fsize, NULL, PixelShader);

	delete[] buffer; // バッファのメモリを解放
}

void Renderer::SetDrawState()
{

}
