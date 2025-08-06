#pragma once
#include	<wrl/client.h>
#include	<string>
#include	<d3d11.h>

using Microsoft::WRL::ComPtr;

//-----------------------------------------------------------------------------
//Shaderクラス
//-----------------------------------------------------------------------------
class Shader {
public:
	void Create(std::string vs, std::string ps);
	void CreateVS(std::string vs);
	void CreatePS(std::string  ps);
	void SetGPU();
private:
	ID3D11VertexShader* m_pVertexShader;		// 頂点シェーダー
	ID3D11PixelShader*  m_pPixelShader;			// ピクセルシェーダー
	ID3D11InputLayout*  m_pVertexLayout;		// 頂点レイアウト
};