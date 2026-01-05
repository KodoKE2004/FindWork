#pragma once
#include	<vector>
#include	<wrl/client.h>
#include	"Dx11helper.h"
#include	"Renderer.h"

using Microsoft::WRL::ComPtr;

template <typename T>
class VertexBuffer
{
private:
	UINT m_Stride = 0;
	UINT m_Count  = 0;
	ComPtr<ID3D11Buffer> m_Buffer;  // 頂点バッファ
public:
	VertexBuffer() = default;
	~VertexBuffer() = default;

	//=========================
	// VertexBufferの作成
	//=========================
	void Create(const std::vector<T>& vertices)
	{
		ID3D11Device* device = Renderer::GetDevice();
		assert(device != nullptr);											  // デバイスが有効であることを確認
		unsigned int stride    = sizeof(T);									  // １頂点当たりのバイト数
		unsigned int vertexnum = static_cast<unsigned int>(vertices.size());  // 頂点数
		void* vertexdata       = (void*)vertices.data();					  // 頂点データのポインタ

		// 頂点バッファを作成
		if (!CreateVertexBufferWrite(
			device,
			stride,
			vertexnum,
			(void*)vertices.data(),
			m_Buffer.ReleaseAndGetAddressOf())) 
			{
				MessageBox(nullptr, "CreateVertexBuffer error", "error", MB_OK);
				return;
			}
	}

	//=========================
	// GPUにデータを送る
	// ここでは、頂点バッファをセットするだけ
	//=========================
	void SetGPU()
	{
		// デバイスコンテキストを取得
		ID3D11DeviceContext* deveiceContext = Renderer::GetDeviceContext();
		
		// 頂点バッファをセット
		unsigned int stride = sizeof(T);  // １頂点当たりのバイト数
		unsigned int offset = 0;          // オフセットは0
		deveiceContext->IASetVertexBuffers(0, 1, m_Buffer.GetAddressOf(), &stride, &offset);
	}

	//=========================
	// VertexBufferの変更
	//=========================
	void Modify(const std::vector<T>& vertices)
	{
		// 頂点データの書き換え
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = Renderer::GetDeviceContext()->Map(
			m_Buffer.Get(),
			0, D3D11_MAP_WRITE_DISCARD,
			0, &mappedResource);

		if(SUCCEEDED(hr)) {
			memcpy(mappedResource.pData, vertices.data(), vertices.size() * sizeof(T));
			Renderer::GetDeviceContext()->Unmap(m_Buffer.Get(), 0);
		}
	}

    ID3D11Buffer* GetBuffer() const { return m_Buffer.Get(); }
};

