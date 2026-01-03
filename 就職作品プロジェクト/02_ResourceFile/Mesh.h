#pragma once
#include	<vector>
#include	"Renderer.h"

class Mesh {
protected:
	std::vector<VERTEX_3D>		m_Vertices;		// 頂点座標群
	std::vector<unsigned int>	m_Indices;		// インデックスデータ群
public:
	// 頂点データ取得
	const std::vector<VERTEX_3D>& GetVertices() {
		return m_Vertices;
	}

	// インデックスデータ取得
	const std::vector<unsigned int>& GetIndices() {
		return m_Indices;
	}
};

