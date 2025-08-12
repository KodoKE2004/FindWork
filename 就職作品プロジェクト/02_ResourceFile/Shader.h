#pragma once
#include	<wrl/client.h>
#include	<string>
#include	<d3d11.h>

using Microsoft::WRL::ComPtr;

enum class ShaderStage {
	VS,
	PS,
	GS,
	CS
};


//======================================
//				Shaderクラス
//======================================
class BaseShader {
protected:
	std::string m_HlslName;
	ShaderStage m_ShaderStage;
public:
	BaseShader() = default;
	BaseShader(std::string hlslName, ShaderStage stage) : m_HlslName(hlslName), m_ShaderStage(stage) {}
	BaseShader* TryCreateShaderFromName();

	// シェーダーの名前とステージを設定
	void SetHlslName(std::string hlslName) { m_HlslName = std::move(hlslName); }
	void SetShaderStage(ShaderStage stage) { m_ShaderStage = stage; }

	// シェーダーの名前とステージを取得
	std::string GetHlslName()	 const { return m_HlslName; }
	ShaderStage GetShaderStage() const { return m_ShaderStage; }

	virtual ~BaseShader() = 0;
	virtual bool Create(std::string hlslName) = 0;
	virtual void SetGPU() = 0;
};

//======================================
//			VertexShaderクラス
//======================================
class VertexShader : public BaseShader 
{
private:
	ComPtr<ID3D11VertexShader> m_VertexShader;
	ComPtr<ID3D11InputLayout> m_InputLayout;
public:
	bool Create(std::string hlslName) override;
	void SetGPU();
};

//======================================
//			PixelShaderクラス
//======================================
class PixelShader : public BaseShader 
{
private:
	ComPtr<ID3D11PixelShader> m_PixelShader;

public:
	bool Create(std::string hlslName) override;
	void SetGPU();
};

//======================================
//			ComputeShaderクラス
//======================================
class ComputeShader : public BaseShader 
{
private:
	ComPtr<ID3D11ComputeShader> m_ComputeShader;
public:
	bool Create(std::string hlslName) override;
	void SetGPU();
};

//======================================
//			GeometoryShaderクラス
//======================================
class GeometryShader : public BaseShader 
{
private:
	ComPtr<ID3D11GeometryShader> m_GeometryShader;
public:
	bool Create(std::string hlslName) override;
	void SetGPU();
};