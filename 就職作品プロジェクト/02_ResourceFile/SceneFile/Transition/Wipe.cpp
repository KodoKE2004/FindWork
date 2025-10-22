#include "Application.h"
#include "Wipe.h"


Wipe::Wipe(Camera* cam) : TransitionBase(cam), m_Overlay(cam)
{

}

void Wipe::Initialize()
{
    // フェード用の画像追加
    auto textureMgr = Game::GetInstance().GetTextureManager();
    m_Texture = textureMgr->GetTexture("Black.png");
    SetScale(SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f);
    SetColor(0.0f, 0.0f, 0.0f, 0.0f);

    // 頂点データ
    std::vector<VERTEX_3D> vertices;

    vertices.resize(4);

    vertices[0].position = NVector3(-0.5f, 0.5f, 0.0f);
    vertices[1].position = NVector3(0.5f, 0.5f, 0.0f);
    vertices[2].position = NVector3(-0.5f, -0.5f, 0.0f);
    vertices[3].position = NVector3(0.5f, -0.5f, 0.0f);

    vertices[0].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    vertices[1].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    vertices[2].color = Color(1.0f, 1.0f, 1.0f, 1.0f);
    vertices[3].color = Color(1.0f, 1.0f, 1.0f, 1.0f);

    vertices[0].uv = Vector2(0.0f, 0.0f);
    vertices[1].uv = Vector2(1.0f, 0.0f);
    vertices[2].uv = Vector2(0.0f, 1.0f);
    vertices[3].uv = Vector2(1.0f, 1.0f);

    // 頂点バッファ生成
    m_VertexBuffer.Create(vertices);

    // インデックスバッファ生成
    std::vector<unsigned int> indices;
    indices.resize(4);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;

    // インデックスバッファ生成
    m_IndexBuffer.Create(indices);

    // シェーダオブジェクト生成
    SetShader("VS_Alpha", "PS_Alpha");

    // マテリアル情報取得
    m_Materiale = std::make_unique<Material>();
    MATERIAL mtrl;
    mtrl.Diffuse = Color(1.0f, 1.0f, 1.0f, 1.0f);
    mtrl.Shiness = 1;
    mtrl.TextureEnable = true; // テクスチャを使うか否かのフラグ
    m_Materiale->Create(mtrl);
}

void Wipe::Update()
{
    m_Overlay.Update();
}

void Wipe::Draw()
{
    ID3D11DeviceContext* ctx = Renderer::GetDeviceContext();

    // ビューポートサイズを取得
    UINT W=0, H=0;
    GetViewportSize(ctx, W, H);
    if (W == 0 || H == 0) return;

    // 進行方向とprogressからシザー矩形を計算
    D3D11_RECT rc{};
    float p = m_Invert ? (1.0f - m_Progress) : m_Progress;
    p = (p < 0 ? 0 : (p > 1 ? 1 : p));

    switch (m_Dir)
    {
    case DIRECTION::LEFT_TO_RIGHT:
        rc.left = 0; rc.top = 0;
        rc.right = static_cast<LONG>(W * p);
        rc.bottom = static_cast<LONG>(H);
        break;

    case DIRECTION::RIGHT_TO_LEFT:
        rc.left = static_cast<LONG>(W * (1.0f - p)); rc.top = 0;
        rc.right = static_cast<LONG>(W);
        rc.bottom = static_cast<LONG>(H);
        break;

    case DIRECTION::TOP_TO_BOTTOM:
        rc.left = 0; rc.top = 0;
        rc.right = static_cast<LONG>(W);
        rc.bottom = static_cast<LONG>(H * p);
        break;

    case DIRECTION::BOTTOM_TO_TOP:
        rc.left = 0; rc.top = static_cast<LONG>(H * (1.0f - p));
        rc.right = static_cast<LONG>(W);
        rc.bottom = static_cast<LONG>(H);
        break;
    }

    // RS差し替え & シザー設定
    ComPtr<ID3D11RasterizerState> rsOrig;
    ctx->RSGetState(rsOrig.ReleaseAndGetAddressOf());
    ctx->RSSetState(m_RSScissor.Get());
    ctx->RSSetScissorRects(1, &rc);

    // この呼び出しは「全画面ブリット」。シザーで必要領域だけ描かれる
    m_Overlay.Draw(); // ← m_SRV を使って alpha合成（あなたの既存の動作）:contentReference[oaicite:1]{index=1}

    // 復帰
    ctx->RSSetScissorRects(0, nullptr);
    ctx->RSSetState(rsOrig.Get());
}

void Wipe::Finalize()
{
    m_Overlay.Finalize(); 
}

void Wipe::GetViewportSize(ID3D11DeviceContext* ctx, UINT& w, UINT& h)
{
    D3D11_VIEWPORT vp{};
    UINT count = 1;
    ctx->RSGetViewports(&count, &vp);
    w = static_cast<UINT>(vp.Width);
    h = static_cast<UINT>(vp.Height);
}
