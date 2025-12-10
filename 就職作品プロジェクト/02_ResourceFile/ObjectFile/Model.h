#pragma once
#include "Object.h"
#include "StaticMesh.h"
#include "MeshRenderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Material.h"
#include <memory>
#include <vector>
#include <d3d11.h>
#include <wrl/client.h>

struct MeshInfo {
    std::shared_ptr<StaticMesh> mesh;
    std::string filePath;
    std::string texDirectory;
};

class Model : public Object
{
protected:
    std::string  m_MeshName;
    MeshInfo     m_MeshModel;
    MeshRenderer m_MeshRenderer;

    VertexBuffer<VERTEX_3D> m_VertexBuffer;
    IndexBuffer             m_IndexBuffer;
    std::vector<SUBSET>     m_Subsets;
    std::vector<std::shared_ptr<Texture>> m_Textures;
    std::vector<std::unique_ptr<Material>> m_Materials;
    bool m_IsSky = false;
    float m_SkyRadius = 1.0f;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RS_CullFront;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DSS_NoWrite_Lequal;

public:
    Model(Camera& cam);

    void Initialize() override;
    void Update()     override;
    void Draw()       override;
    void Finalize()   override;

    void GetMeshModel(std::string modelName);
    void EnableSkyDome(float radius);
    void DisableSkyDome();

private:
    void CreateSkyStates();
    void DrawSky();
};
