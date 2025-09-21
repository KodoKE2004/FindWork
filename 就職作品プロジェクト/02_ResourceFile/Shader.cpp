#include "Shader.h"
#include "dx11helper.h"
#include "Renderer.h"
#include <d3d11.h>

////=======================================
////Shader作成
////=======================================
//void Shader::Create(std::string vs, std::string ps)
//{
//	// 頂点データの定義
//	D3D11_INPUT_ELEMENT_DESC layout[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0,	D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0 }
//	};
//
//	unsigned int numElements = ARRAYSIZE(layout);
//
//	ID3D11Device* device = Renderer::GetDevice();
//
//	// 頂点シェーダーオブジェクトを生成、同時に頂点レイアウトも生成
//	bool sts = CreateVertexShader(device,
//		vs.c_str(),
//		"main",
//		"vs_5_0",
//		layout,
//		numElements,
//		&m_pVertexShader,
//		&m_pVertexLayout);
//
//	if (!sts) {
//		MessageBox(nullptr, "CreateVertexShader error", "error", MB_OK);
//		return;
//	}
//
//	// ピクセルシェーダーを生成
//	sts = CreatePixelShader(			// ピクセルシェーダーオブジェクトを生成
//		device,							// デバイスオブジェクト
//		ps.c_str(),
//		"main",
//		"ps_5_0",
//		&m_pPixelShader);
//
//	if (!sts) {
//		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
//		return;
//	}
//
//	return;
//}
//
//void Shader::CreateVS(std::string vs)
//{
//	// 頂点データの定義
//	D3D11_INPUT_ELEMENT_DESC layout[] =
//	{
//		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	D3D11_APPEND_ALIGNED_ELEMENT,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0,	D3D11_APPEND_ALIGNED_ELEMENT,   D3D11_INPUT_PER_VERTEX_DATA, 0 }
//	};
//
//	unsigned int numElements = ARRAYSIZE(layout);
//
//	ID3D11Device* device = Renderer::GetDevice();
//
//	// 頂点シェーダーオブジェクトを生成、同時に頂点レイアウトも生成
//	bool sts = CreateVertexShader(device,
//		vs.c_str(),
//		"main",
//		"vs_5_0",
//		layout,
//		numElements,
//		&m_pVertexShader,
//		&m_pVertexLayout);
//
//	if (!sts) {
//		MessageBox(nullptr, "CreateVertexShader error", "error", MB_OK);
//		return;
//	}
//}
//
//void Shader::CreatePS(std::string ps)
//{
//	auto device = Renderer::GetDevice();
//
//	// ピクセルシェーダーを生成
//	bool sts = CreatePixelShader(			// ピクセルシェーダーオブジェクトを生成
//		device,							// デバイスオブジェクト
//		ps.c_str(),
//		"main",
//		"ps_5_0",
//		&m_pPixelShader);
//
//	if (!sts) {
//		MessageBox(nullptr, "CreatePixelShader error", "error", MB_OK);
//		return;
//	}
//}
//
////=======================================
////GPUにデータを送る
////=======================================
//void Shader::SetGPU()
//{
//	ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();
//
//	devicecontext->VSSetShader(m_pVertexShader, nullptr, 0);		// 頂点シェーダーをセット
//	devicecontext->PSSetShader(m_pPixelShader, nullptr, 0);		// ピクセルシェーダーをセット
//	devicecontext->IASetInputLayout(m_pVertexLayout);				// 頂点レイアウトセット
//}

BaseShader::~BaseShader() = default;

BaseShader* BaseShader::TryCreateShaderFromName()
{

	BaseShader* shader = nullptr;

	// 命名規則を遵守していたら、対応するシェーダークラスを生成
    switch (m_ShaderStage) { //  |              if               |  |     true         |  | false |
    case ShaderStage::VS: return (m_HlslName.rfind("VS_", 0) == 0) ? new VertexShader()  : nullptr;
    case ShaderStage::PS: return (m_HlslName.rfind("PS_", 0) == 0) ? new PixelShader()   : nullptr;
    case ShaderStage::GS: return (m_HlslName.rfind("GS_", 0) == 0) ? new GeometryShader(): nullptr;
    case ShaderStage::CS: return (m_HlslName.rfind("CS_", 0) == 0) ? new ComputeShader() : nullptr;
    default:              return nullptr;
    }
}

//======================================
//			VertexShaderクラス
//======================================
bool VertexShader::Create(std::string hlslName)
{
    m_HlslName = std::move(hlslName);
    m_ShaderStage = ShaderStage::VS;

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,     0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR",       0, DXGI_FORMAT_R32G32B32A32_FLOAT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT,        0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT,     1, 0 ,D3D11_INPUT_PER_INSTANCE_DATA, 1 }, // 1 = step rate (インスタンスごとに進める)
        { "INSTANCEMAT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,  1, 0 ,D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEMAT", 1, DXGI_FORMAT_R32G32B32A32_FLOAT,  1, 16,D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEMAT", 2, DXGI_FORMAT_R32G32B32A32_FLOAT,  1, 32,D3D11_INPUT_PER_INSTANCE_DATA, 1 },
        { "INSTANCEMAT", 3, DXGI_FORMAT_R32G32B32A32_FLOAT,  1, 48,D3D11_INPUT_PER_INSTANCE_DATA, 1 },

    };

    auto* device = Renderer::GetDevice();
    bool res = CreateVertexShader(
        device,
        m_HlslName.c_str(),
        "main",
        "vs_5_0",
        layout,
        static_cast<UINT>(std::size(layout)),
        m_VertexShader.GetAddressOf(),
        m_InputLayout.GetAddressOf()
    );  

    return res;
}

void VertexShader::SetGPU()
{
    auto* ctx = Renderer::GetDeviceContext();
    ctx->VSSetShader(m_VertexShader.Get(), nullptr, 0);
    ctx->IASetInputLayout(m_InputLayout.Get());
}



//======================================
//			PixelShaderクラス
//======================================
bool
PixelShader::Create(std::string hlslName)
{
    m_HlslName = std::move(hlslName);
    m_ShaderStage = ShaderStage::PS;

    auto* device = Renderer::GetDevice();
    bool res = CreatePixelShader(
        device,
        m_HlslName.c_str(),
        "main",
        "ps_5_0",
        m_PixelShader.GetAddressOf()
    );

    return res;
}

void PixelShader::SetGPU()
{
    auto* ctx = Renderer::GetDeviceContext();
    ctx->PSSetShader(m_PixelShader.Get(), nullptr, 0);
}



//======================================
//			ComputeShaderクラス
//======================================
bool GeometryShader::Create(std::string hlslName)
{
    m_HlslName = std::move(hlslName);
    m_ShaderStage = ShaderStage::GS;

    auto* device = Renderer::GetDevice();
    bool res =  CreateGeometryShader(
        device,
        m_HlslName.c_str(),
        "main",
        "gs_5_0",
        m_GeometryShader.GetAddressOf()
    );
    return res;
}

void GeometryShader::SetGPU()
{
    auto* ctx = Renderer::GetDeviceContext();
    ctx->GSSetShader(m_GeometryShader.Get(), nullptr, 0);
}


//======================================
//			GeometoryShaderクラス
//======================================
bool ComputeShader::Create(std::string hlslName)
{
    m_HlslName = std::move(hlslName);
    m_ShaderStage = ShaderStage::CS;

    auto* device = Renderer::GetDevice();
    bool res = CreateComputeShader(
        device,
        m_HlslName.c_str(),
        "main",
        "cs_5_0",
        m_ComputeShader.GetAddressOf()
    );

    return res;
}

void ComputeShader::SetGPU()
{
    auto* ctx = Renderer::GetDeviceContext();
    ctx->CSSetShader(m_ComputeShader.Get(), nullptr, 0);
}

