#pragma once
#include	<vector>
#include	"Renderer.h"

class Mesh {
protected:
	vector<VERTEX_3D>		m_Vertices;		// 頂点座標群
	vector<unsigned int>	m_Indices;		// インデックスデータ群
public:
	// 頂点データ取得
	const vector<VERTEX_3D>& GetVertices() {
		return m_Vertices;
	}

	// インデックスデータ取得
	const vector<unsigned int>& GetIndices() {
		return m_Indices;
	}
};

