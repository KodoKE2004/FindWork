#include "Square.h"
#include "Game.h"
#include <memory>

#include "Renderer.h"
#include <algorithm>
#include <stdexcept>

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
	m_Vertices.resize(4);

	m_Vertices[0].position = NVector3(-0.5f,  0.5f, 0.0f);
	m_Vertices[1].position = NVector3( 0.5f,  0.5f, 0.0f);
	m_Vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
	m_Vertices[3].position = NVector3( 0.5f, -0.5f, 0.0f);

	m_Vertices[0].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_Vertices[1].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_Vertices[2].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_Vertices[3].color = Color(1.0f, 1.0f, 1.0f, 1.0f);

	m_Vertices[0].uv = Vector2(0.0f, 0.0f);
	m_Vertices[1].uv = Vector2(1.0f, 0.0f);
	m_Vertices[2].uv = Vector2(0.0f, 1.0f);
	m_Vertices[3].uv = Vector2(1.0f, 1.0f);

	m_VertexBuffer.Create(m_Vertices);

	std::vector<unsigned int> indices;
	indices.resize(4);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	m_IndexBuffer.Create(indices);

    TextureManager* textureMgr = Game::GetInstance();
    SetTexture(textureMgr->GetTexture("Plane.png"));
	SetShader("VS_Alpha", "PS_Alpha");

	m_Material = std::make_unique<Material>();
	MATERIAL mtrl;
	mtrl.Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
	mtrl.Shiness = 1;
	mtrl.TextureEnable = true;
	m_Material->Create(mtrl);

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
	// SetGPU already binds the default samplers and constant buffers.
}

//======================
//		  描画処理
//======================
void Square::Draw()
{
    DrawPrepared(m_UseInstancing ? &m_Instances : nullptr, false);
}

void Square::DrawPrepared(const std::vector<InstanceData>* instances, bool ownAppearance)
{
    if (instances && instances->empty()) return;
	// 描画の処理

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

	const Color tint = ownAppearance ? Color(1, 1, 1, 1) : m_Color;
    m_Material->SetDiffuse(tint);
	m_Material->Update();
	m_Material->SetGPU();

	// UVの設定を指定
	float u0 = (m_NumU - 1.0f) / m_SplitX;
	float v0 = (m_NumV - 1.0f) / m_SplitY;
	float u1 = 1.0f / m_SplitX;
	float v1 = 1.0f / m_SplitY;

	if (ownAppearance) Renderer::SetUV(0, 0, 1, 1);
    else Renderer::SetUV(u0, v0, u1, v1);

	Camera::ScopedMode scopedMode(m_Camera, m_DrawCameraMode);

    if (instances)
    {
        auto* shader = dynamic_cast<VertexShader*>(m_VertexShader);
        if (!shader || !Renderer::DrawIndexedInstanced(4, 0, 0, *instances, *shader))
            throw std::runtime_error("Square instanced draw failed");
        return;
    }

	Renderer::DrawIndexed(
		4, // 描画するインデックス数（四角形なんで４）
		0, // 最初のインデックスバッファの位置
		0);
}

//==========================
//			終了処理
//==========================
void Square::Finalize()
{
    m_Instances.clear();
    m_UseInstancing = false;
}


void Square::UpdateInstanceBuffers()
{
    // Renderer owns a reusable dynamic stream, updated once per draw chunk.
}

void Square::SetInstancingEnabled(bool enabled)
{
    m_UseInstancing = enabled;
}

void Square::SetInstanceTransforms(const std::vector<InstanceTransform2D>& transforms)
{
    m_Instances.clear();
    m_Instances.reserve(transforms.size());
    for (const auto& transform : transforms)
    {
        InstanceData instance;
        instance.world = Matrix::CreateScale(transform.scale)
            * Matrix::CreateFromYawPitchRoll(transform.rotation.x, transform.rotation.y, transform.rotation.z)
            * Matrix::CreateTranslation(transform.position);
        m_Instances.push_back(instance);
    }
}

InstanceData Square::GetInstanceData() const
{
    InstanceData instance;
    instance.world = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z)
        * Matrix::CreateFromYawPitchRoll(m_Rotation.x, m_Rotation.y, m_Rotation.z)
        * Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
    instance.color = m_Color;
    instance.uvRect = Vector4((m_NumU - 1) / m_SplitX, (m_NumV - 1) / m_SplitY,
        1 / m_SplitX, 1 / m_SplitY);
    return instance;
}

bool Square::CanBatchWith(const Square& other) const
{
    if (&m_Camera != &other.m_Camera || m_DrawCameraMode != other.m_DrawCameraMode
        || m_Texture != other.m_Texture || m_VertexShader != other.m_VertexShader
        || m_PixelShader != other.m_PixelShader || m_GeometoryShader != other.m_GeometoryShader
        || m_ComputeShader != other.m_ComputeShader || m_Vertices.size() != other.m_Vertices.size()) return false;
    for (size_t i = 0; i < m_Vertices.size(); ++i)
    {
        const auto& a = m_Vertices[i];
        const auto& b = other.m_Vertices[i];
        if (a.position.x != b.position.x || a.position.y != b.position.y || a.position.z != b.position.z
            || a.normal.x != b.normal.x || a.normal.y != b.normal.y || a.normal.z != b.normal.z
            || a.color != b.color || a.uv != b.uv) return false;
    }
    return true;
}

void Square::DrawBatch(const std::vector<Square*>& squares)
{
    Square* representative = nullptr;
    std::vector<InstanceData> instances;
    auto flush = [&]() {
        if (representative) representative->DrawPrepared(&instances, true);
        instances.clear();
        representative = nullptr;
    };
    for (auto* square : squares)
    {
        if (!square) continue;
        auto* shader = dynamic_cast<VertexShader*>(square->m_VertexShader);
        // Preserve custom rendering and explicitly configured instance lists.
        if (!shader || !shader->SupportsInstancing() || square->m_UseInstancing)
        {
            flush();
            square->Draw();
            continue;
        }
        if (representative && !representative->CanBatchWith(*square)) flush();
        if (!representative) representative = square;
        instances.push_back(square->GetInstanceData());
    }
    flush();
}

// テクスチャを指定
void Square::SetTexture(const char* imgname)
{
	if (!m_Texture)
	{
        m_Texture = std::make_shared<Texture>();
	}

	bool sts = m_Texture->LoadFromFile(imgname);
	assert(sts == true);

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

void Square::SetUVRect(float u0, float v0, float u1, float v1)
{
	if (m_Vertices.size() < 4)
	{
		return;
	}

	u0 = std::clamp(u0, 0.0f, 1.0f);
	v0 = std::clamp(v0, 0.0f, 1.0f);
	u1 = std::clamp(u1, 0.0f, 1.0f);
	v1 = std::clamp(v1, 0.0f, 1.0f);

	// 頂点順:
	// [0] 左上, [1] 右上, [2] 左下, [3] 右下
	m_Vertices[0].uv = Vector2(u0, v0);
	m_Vertices[1].uv = Vector2(u1, v0);
	m_Vertices[2].uv = Vector2(u0, v1);
	m_Vertices[3].uv = Vector2(u1, v1);

	m_VertexBuffer.Modify(m_Vertices);
}
