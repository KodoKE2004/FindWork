#include "Square.h"
#include "Game.h"
#include <memory>

#include "Renderer.h"
#include <algorithm>

namespace
{
	constexpr UINT kMaxInstanceTransforms = 512u;
}

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
Square::Square(Camera& cam) :Object(cam)
{
    m_Texture = std::make_shared<Texture>();
	m_HitResult.m_Now = false;
	m_HitResult.m_Old = false;

    TextureManager* textureMgr = Game::GetInstance();
    SetTexture(textureMgr->GetTexture("Plane.png"));
}

// デストラクタ
Square::~Square()
{

}

//======
// 初期化処理
//======
void Square::Initialize()
{

	// 頂点データ
	std::vector<VERTEX_3D> vertices;

	vertices.resize(4);

	vertices[0].position = NVector3(-0.5f,  0.5f, 0.0f);
	vertices[1].position = NVector3( 0.5f,  0.5f, 0.0f);
	vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
	vertices[3].position = NVector3( 0.5f, -0.5f, 0.0f);

	vertices[0].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[1].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[2].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	vertices[3].color = Color(1.0f, 1.0f, 1.0f, 1.0f);

	vertices[0].uv = Vector2(0.0f, 0.0f);
	vertices[1].uv = Vector2(1.0f, 0.0f);
	vertices[2].uv = Vector2(0.0f, 1.0f);
	vertices[3].uv = Vector2(1.0f, 1.0f);

	m_VertexBuffer.Create(vertices);

	std::vector<unsigned int> indices;
	indices.resize(4);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	m_IndexBuffer.Create(indices);

	SetShader("VS_Alpha", "PS_Alpha");

	m_Materiale = std::make_unique<Material>();
	MATERIAL mtrl;
	mtrl.Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Shiness = 1;
	mtrl.TextureEnable = true;
	m_Materiale->Create(mtrl);

	SetPos(0.0f,0.0f,0.0f);
    SetScale(100.0f, 100.0f, 1.0f);

}

//======================
//		  更新処理
//======================
void Square::Update()
{
	
}

void Square::SetPipeline()
{
	ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();
	if (!devicecontext) {
		return;
	}

	// Sprite/FullscreenQuad用: 毎回確実にパイプラインを設定する
	SetGPU();
	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Renderer::BindDefaultSamplers();
	Renderer::BindDefaultConstantBuffers();
}

//======================
//		  描画処理
//======================
void Square::Draw()
{
	// 描画の処理
	ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();

	// State破壊の影響を受けないよう、パイプラインを先頭で再設定する
	SetPipeline();
	Renderer::SetBlendState(BS_ALPHABLEND);

	// SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	m_Texture->SetGPU();

	m_Materiale->SetDiffuse(DirectX::XMFLOAT4(m_Color.x, m_Color.y, m_Color.z, m_Color.w));
	m_Materiale->Update();
	m_Materiale->SetGPU();

	// UVの設定を指定
	float u0 = (m_NumU - 1.0f) / m_SplitX;
	float v0 = (m_NumV - 1.0f) / m_SplitY;
	float u1 = 1.0f / m_SplitX;
	float v1 = 1.0f / m_SplitY;

	Renderer::SetUV(u0, v0, u1, v1);

	Camera::ScopedMode scepedMode(m_Camera, CAMERA_2D);


	if (m_UseInstancing && m_InstanceCount > 0 && m_InstanceTransformCB) 
	{
		ID3D11Buffer* instanceCB = m_InstanceTransformCB.Get();
		devicecontext->VSSetConstantBuffers(6, 1, &instanceCB);
		devicecontext->DrawIndexedInstanced(4, m_InstanceCount, 0, 0, 0);
		return;
	}


	devicecontext->DrawIndexed(
		4, // 描画するインデックス数（四角形なんで４）
		0, // 最初のインデックスバッファの位置
		0);
}

//==========================
//			終了処理
//==========================
void Square::Finalize()
{
	
}


void Square::UpdateInstanceBuffers()
{
	if (m_InstanceCount == 0)
	{
		m_InstanceTransformCB.Reset();
		return;
	}

	D3D11_BUFFER_DESC desc{};
	desc.ByteWidth = static_cast<UINT>(sizeof(Vector4) * m_InstancePositionScale.size());
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA init{};
	init.pSysMem = m_InstancePositionScale.data();

	if (FAILED(Renderer::GetDevice()->CreateBuffer(&desc, &init, m_InstanceTransformCB.ReleaseAndGetAddressOf())))
	{
		m_InstanceTransformCB.Reset();
	}
}


void Square::SetInstancingEnabled(bool enabled)
{
	m_UseInstancing = enabled;
}

void Square::SetInstanceTransforms(const std::vector<InstanceTransform2D>& transforms)
{
	m_InstancePositionScale.clear();
	m_InstanceCount = min(static_cast<UINT>(transforms.size()), kMaxInstanceTransforms);
	m_InstancePositionScale.reserve(static_cast<size_t>(m_InstanceCount) * 2);

	for (UINT i = 0; i < m_InstanceCount; ++i)
	{
		const auto& transform = transforms[i];
		m_InstancePositionScale.emplace_back(transform.position.x, transform.position.y, transform.position.z, 1.0f);
		m_InstancePositionScale.emplace_back(transform.scale.x, transform.scale.y, transform.scale.z, 1.0f);
	}

	UpdateInstanceBuffers();
}

// テクスチャを指定
void Square::SetTexture(const char* imgname)
{
	if (!m_Texture)
	{
        m_Texture = std::make_shared<Texture>();
	}

	bool sts = m_Texture->LoadFromFile(imgname);
	assert(sts != true);

    Object::SetTexture(m_Texture);
}

void Square::SetTexture(pShared<Texture> texture)
{
	m_Texture = texture;

	Object::SetTexture(m_Texture);

}

// UV座標を指定
// nu = U座標の位置
// nv = V座標の位置
// sx = U座標の分割数
// sy = V座標の分割数
void Square::SetUV(const float& nu = 1.0f, const float& nv = 1.0f, const float& sx = 1.0f, const float& sy = 1.0f)
{
	m_NumU = nu;
	m_NumV = nv;
	m_SplitX = sx;
	m_SplitY = sy;
}
