#pragma once

#include	"dx11helper.h"
#include	"Renderer.h"
#include	<vector>
#include	<wrl/client.h>

using Microsoft::WRL::ComPtr;

//-----------------------------
//IndexBufferクラス
//-----------------------------

class IndexBuffer {
	ComPtr<ID3D11Buffer> m_IndexBuffer;

public:
	IndexBuffer() = default;
	~IndexBuffer() = default;

	void Create(const std::vector<unsigned int>& indices) {
		// デバイスの取得
		ID3D11Device* device = nullptr;
		device = Renderer::GetDevice();
		assert(device);

		// インデックスバッファ作成
		bool sts = CreateIndexBuffer(
			device,										// デバイス
			(unsigned int)(indices.size()),				// インデックス数
			(void*)indices.data(),						// インデックスデータ先頭アドレス
			&m_IndexBuffer);							// インデックスバッファ

		assert(sts == true);
	}

	void SetGPU() {
		// デバイスコンテキスト取得
		ID3D11DeviceContext* deviceContext = nullptr;
		deviceContext = Renderer::GetDeviceContext();

		// インデックスバッファをセット
		deviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	}
};