# 描画の利用箇所一覧

対象: 自作コード `02_ResourceFile` の全 `.cpp` / `.h` / `.hpp`。宣言・定義と呼び出しを含む静的一覧。
再生成: `python tools/rendering_inventory.py`。外部ライブラリ ImGui の描画は本体の `DebugUI::Render` から呼ばれる。

## GPU送信・描画ディスパッチ

| ファイル | 行 | コード |
|---|---:|---|
| Application.cpp | 235 | `instance.Draw();` |
| DebugGridLine.cpp | 91 | `void DebugGridLine::DrawAxis(VertexBuffer<VERTEX_3D>& buffer, float axisFlag)` |
| DebugGridLine.cpp | 98 | `Renderer::GetDeviceContext()->DrawInstanced(2, m_InstanceCount, 0, 0);` |
| DebugGridLine.cpp | 101 | `void DebugGridLine::Draw()` |
| DebugGridLine.cpp | 124 | `DrawAxis(m_LineX, 0.0f);` |
| DebugGridLine.cpp | 125 | `DrawAxis(m_LineZ, 1.0f);` |
| DebugGridLine.h | 15 | `void Draw();` |
| DebugGridLine.h | 31 | `void DrawAxis(VertexBuffer<VERTEX_3D>& buffer, float axisFlag);` |
| Game.cpp | 99 | `DrawInGameStateGUI();` |
| Game.cpp | 180 | `void Game::Draw()` |
| Game.cpp | 192 | `currentScene->Draw();` |
| Game.cpp | 198 | `it->Draw();` |
| Game.cpp | 203 | `instance.m_Theme->Draw();` |
| Game.h | 79 | `static void Draw();` |
| JsonLoder.hpp | 139 | `void DrawConfigEditor(LiveEditSession& s, const std::filesystem::path& path) {` |
| MeshRenderer.h | 41 | `virtual void DrawSubset(unsigned int indexnum,unsigned int baseindex,unsigned int basevertexindex )` |
| MeshRenderer.h | 43 | `Renderer::DrawIndexed(` |
| MeshRenderer.h | 50 | `virtual void Draw()` |
| MeshRenderer.h | 54 | `Renderer::DrawIndexed(` |
| MeshRenderer.h | 59 | `bool DrawSubsetInstanced(UINT count, UINT startIndex, INT baseVertex,` |
| MeshRenderer.h | 62 | `return Renderer::DrawIndexedInstanced(count, startIndex, baseVertex, instances, shader);` |
| MeshRenderer.h | 64 | `bool DrawInstanced(const std::vector<InstanceData>& instances, VertexShader& shader)` |
| MeshRenderer.h | 67 | `return DrawSubsetInstanced(m_IndexNum, 0, 0, instances, shader);` |
| ObjectFile/Bird.cpp | 94 | `void Bird::Draw()` |
| ObjectFile/Bird.cpp | 96 | `Square::Draw();` |
| ObjectFile/Bird.h | 18 | `void Draw()       override;` |
| ObjectFile/Bomber.cpp | 64 | `void Bomber::Draw()` |
| ObjectFile/Bomber.cpp | 66 | `Square::Draw();` |
| ObjectFile/Bomber.h | 29 | `void Draw()       override;` |
| ObjectFile/Bullet.cpp | 36 | `void Bullet::Draw() {` |
| ObjectFile/Bullet.cpp | 39 | `Square::Draw();` |
| ObjectFile/Bullet.h | 15 | `void Draw() override;` |
| ObjectFile/Button.cpp | 24 | `void Button::Draw()` |
| ObjectFile/Button.cpp | 26 | `MouseObject::Draw();` |
| ObjectFile/Button.h | 12 | `void Draw()       override;` |
| ObjectFile/Car.cpp | 108 | `void Cart::Draw()` |
| ObjectFile/Car.cpp | 110 | `Square::Draw();` |
| ObjectFile/Car.h | 100 | `void Draw()       override;` |
| ObjectFile/CartWarning.cpp | 30 | `void CartWarning::Draw()` |
| ObjectFile/CartWarning.cpp | 34 | `Square::Draw();` |
| ObjectFile/CartWarning.h | 20 | `void Draw()       override;` |
| ObjectFile/Circle.cpp | 87 | `void Circle::Draw()` |
| ObjectFile/Circle.cpp | 120 | `Renderer::DrawIndexed(m_DrawIndexCount, 0, 0);` |
| ObjectFile/Circle.h | 26 | `virtual void Draw()       ;` |
| ObjectFile/Cube.cpp | 203 | `void Cube::Draw()` |
| ObjectFile/Cube.cpp | 228 | `Renderer::DrawIndexed(` |
| ObjectFile/Cube.h | 30 | `void Draw();` |
| ObjectFile/DragController.cpp | 70 | `void DragController::Draw()` |
| ObjectFile/DragController.cpp | 72 | `Square::Draw();` |
| ObjectFile/DragController.h | 48 | `void Draw()       override;` |
| ObjectFile/Enemy.cpp | 33 | `void Enemy::Draw()` |
| ObjectFile/Enemy.cpp | 35 | `Square::Draw();` |
| ObjectFile/Enemy.h | 18 | `void Draw()       override;` |
| ObjectFile/Light.cpp | 53 | `void Light::Draw()` |
| ObjectFile/Light.cpp | 80 | `Renderer::DrawIndexed(` |
| ObjectFile/Light.h | 18 | `void Draw() override ;` |
| ObjectFile/Model.cpp | 31 | `void Model::Draw()` |
| ObjectFile/Model.cpp | 34 | `DrawSky();` |
| ObjectFile/Model.cpp | 59 | `m_MeshRenderer.DrawSubset(` |
| ObjectFile/Model.cpp | 130 | `void Model::DrawSky()` |
| ObjectFile/Model.cpp | 162 | `m_MeshRenderer.DrawSubset(` |
| ObjectFile/Model.h | 41 | `void Draw()       override;` |
| ObjectFile/Model.h | 50 | `void DrawSky();` |
| ObjectFile/MouseObject.cpp | 39 | `void MouseObject::Draw()` |
| ObjectFile/MouseObject.cpp | 41 | `Square::Draw();` |
| ObjectFile/MouseObject.h | 16 | `void Draw()       override;` |
| ObjectFile/Object.cpp | 77 | `void Object::DrawInstances(const std::vector<InstanceData>& instances)` |
| ObjectFile/Object.cpp | 84 | `Draw();` |
| ObjectFile/Object.h | 45 | `virtual void Draw()       = 0;` |
| ObjectFile/Object.h | 56 | `void DrawInstances(const std::vector<InstanceData>& instances);` |
| ObjectFile/Player.cpp | 39 | `void Player::Draw()` |
| ObjectFile/Player.cpp | 41 | `Square::Draw();` |
| ObjectFile/Player.h | 19 | `void Draw()       override;` |
| ObjectFile/Sphere.cpp | 90 | `void Sphere::Draw()` |
| ObjectFile/Sphere.cpp | 94 | `DrawAsSky();` |
| ObjectFile/Sphere.cpp | 98 | `DrawAsMesh();` |
| ObjectFile/Sphere.cpp | 116 | `void Sphere::DrawAsSky()` |
| ObjectFile/Sphere.cpp | 149 | `Renderer::DrawIndexed(m_IndexCount, 0, 0);` |
| ObjectFile/Sphere.cpp | 158 | `void Sphere::DrawAsMesh()` |
| ObjectFile/Sphere.cpp | 179 | `Renderer::DrawIndexed(m_IndexCount, 0, 0);` |
| ObjectFile/Sphere.h | 30 | `void DrawAsSky();` |
| ObjectFile/Sphere.h | 31 | `void DrawAsMesh();` |
| ObjectFile/Sphere.h | 45 | `void Draw()		  override;` |
| ObjectFile/Stone.cpp | 85 | `void Stone::Draw()` |
| ObjectFile/Stone.cpp | 90 | `Square::Draw();` |
| ObjectFile/Stone.cpp | 93 | `void Stone::DrawInstanced(const vector<pShared<Stone>>& stones)` |
| ObjectFile/Stone.cpp | 99 | `Square::DrawBatch(active);` |
| ObjectFile/Stone.h | 15 | `void Draw()       override;` |
| ObjectFile/Stone.h | 16 | `void DrawInstanced(const vector<pShared<Stone>>& stones);` |
| ObjectFile/Theme.cpp | 61 | `void Theme::Draw()` |
| ObjectFile/Theme.cpp | 73 | `Square::Draw();` |
| ObjectFile/Theme.h | 23 | `void Draw()		  override;` |
| ObjectFile/Timer.cpp | 33 | `void Timer::Draw()` |
| ObjectFile/Timer.cpp | 35 | `Square::Draw();` |
| ObjectFile/Timer.cpp | 38 | `m_Meter->Draw();` |
| ObjectFile/Timer.h | 17 | `void Draw()       override;` |
| Particle.cpp | 44 | `void ParticleEmitter::Draw()` |
| Particle.cpp | 49 | `Square::DrawBatch(quads);` |
| Particle.h | 51 | `void Draw();` |
| Renderer.cpp | 754 | `void DrawFullscreenBlit(ID3D11DeviceContext* ctx, ID3D11ShaderResourceView* srv, float alpha)` |
| Renderer.cpp | 771 | `ctx->Draw(3, 0);` |
| Renderer.cpp | 801 | `DrawFullscreenBlit(m_DeviceContext, srv, 1.0f);` |
| Renderer.cpp | 828 | `DrawFullscreenBlit(m_DeviceContext, srv, alpha);` |
| Renderer.cpp | 927 | `void Renderer::DrawIndexed(UINT count, UINT startIndex, INT baseVertex)` |
| Renderer.cpp | 932 | `if (!m_InstanceShader &#124;&#124; !DrawIndexedInstanced(count, startIndex, baseVertex, *m_Instances, *m_InstanceShader))` |
| Renderer.cpp | 936 | `m_DeviceContext->DrawIndexed(count, startIndex, baseVertex);` |
| Renderer.cpp | 939 | `bool Renderer::DrawIndexedInstanced(UINT count, UINT startIndex, INT baseVertex,` |
| Renderer.cpp | 979 | `m_DeviceContext->DrawIndexedInstanced(count, static_cast<UINT>(batch), startIndex, baseVertex, 0);` |
| Renderer.h | 164 | `static void DrawIndexed(UINT count, UINT startIndex = 0, INT baseVertex = 0);` |
| Renderer.h | 165 | `static bool DrawIndexedInstanced(UINT count, UINT startIndex, INT baseVertex,` |
| SceneFile/GameSceneDodge.cpp | 271 | `void GameSceneDodge::Draw()` |
| SceneFile/GameSceneDodge.cpp | 275 | `auto flush = [&]() { Square::DrawBatch(stones); stones.clear(); };` |
| SceneFile/GameSceneDodge.cpp | 285 | `if (object) object->Draw();` |
| SceneFile/GameSceneDodge.h | 24 | `void Draw() 			override;` |
| SceneFile/GameSceneExe.h | 70 | `virtual void Draw() = 0;` |
| SceneFile/GameSceneGunman.cpp | 375 | `void GameSceneGunman::Draw()` |
| SceneFile/GameSceneGunman.cpp | 379 | `object->Draw();` |
| SceneFile/GameSceneGunman.h | 41 | `void Draw() 			override;` |
| SceneFile/GameSceneRocket.cpp | 58 | `void GameSceneRocket::Draw()` |
| SceneFile/GameSceneRocket.cpp | 62 | `object->Draw();` |
| SceneFile/GameSceneRocket.h | 17 | `void Draw() 			override;` |
| SceneFile/GameSceneShot.cpp | 272 | `void GameSceneShot::Draw()` |
| SceneFile/GameSceneShot.cpp | 274 | `Scene::Draw();` |
| SceneFile/GameSceneShot.h | 28 | `void Draw() 			override;` |
| SceneFile/GameSceneText.cpp | 250 | `void GameSceneText::Draw()` |
| SceneFile/GameSceneText.cpp | 252 | `Scene::Draw();` |
| SceneFile/GameSceneText.h | 48 | `void Draw() 			override;` |
| SceneFile/GameSceneWait.cpp | 421 | `void GameSceneWait::Draw()` |
| SceneFile/GameSceneWait.cpp | 423 | `Scene::Draw();` |
| SceneFile/GameSceneWait.h | 94 | `void Draw() override;` |
| SceneFile/ResultScene.cpp | 135 | `void ResultScene::Draw()` |
| SceneFile/ResultScene.cpp | 137 | `Scene::Draw();` |
| SceneFile/ResultScene.h | 26 | `void Draw() 			override;` |
| SceneFile/Scene.cpp | 27 | `void Scene::Draw()` |
| SceneFile/Scene.cpp | 32 | `object->Draw();` |
| SceneFile/Scene.h | 129 | `virtual void Draw();` |
| SceneFile/TitleScene.cpp | 156 | `void TitleScene::Draw()` |
| SceneFile/TitleScene.cpp | 158 | `Scene::Draw();` |
| SceneFile/TitleScene.h | 54 | `void Draw() 			override;` |
| SceneFile/Transition/Fade.cpp | 79 | `void Fade::Draw()` |
| SceneFile/Transition/Fade.cpp | 91 | `DrawDebugFullscreenSolid();` |
| SceneFile/Transition/Fade.cpp | 116 | `Renderer::DrawIndexed(4, 0, 0);` |
| SceneFile/Transition/Fade.h | 14 | `void Draw()             override;` |
| SceneFile/Transition/SnapshotOverlay.h | 21 | `void Draw() override` |
| SceneFile/Transition/TransitionBase.cpp | 74 | `void DrawTransitionStateUI(const char* label, SceneTransitionParam& param)` |
| SceneFile/Transition/TransitionBase.cpp | 358 | `void TransitionBase::DrawDebugFullscreenSolid()` |
| SceneFile/Transition/TransitionBase.cpp | 405 | `Renderer::DrawIndexed(4, 0, 0);` |
| SceneFile/Transition/TransitionBase.cpp | 446 | `void DrawInGameStateGUI()` |
| SceneFile/Transition/TransitionBase.cpp | 520 | `TransGui::DrawTransitionStateUI("Title  -> Wait",   TitleToWait);` |
| SceneFile/Transition/TransitionBase.cpp | 521 | `TransGui::DrawTransitionStateUI("Wait   -> Game",   WaitToGame);` |
| SceneFile/Transition/TransitionBase.cpp | 522 | `TransGui::DrawTransitionStateUI("Game   -> Wait",   GameToWait);` |
| SceneFile/Transition/TransitionBase.cpp | 523 | `TransGui::DrawTransitionStateUI("Wait   -> Result", WaitToResult);` |
| SceneFile/Transition/TransitionBase.cpp | 524 | `TransGui::DrawTransitionStateUI("Result -> Title",  ResultToTitle);` |
| SceneFile/Transition/TransitionBase.cpp | 525 | `TransGui::DrawTransitionStateUI("Result -> Game",   ResultToGame);` |
| SceneFile/Transition/TransitionBase.h | 62 | `void DrawInGameStateGUI();` |
| SceneFile/Transition/TransitionBase.h | 106 | `void DrawDebugFullscreenSolid();` |
| SceneFile/Transition/TransitionBase.h | 117 | `virtual void Draw()       = 0;` |
| SceneFile/Transition/Wipe.cpp | 70 | `void Wipe::Draw()` |
| SceneFile/Transition/Wipe.cpp | 82 | `DrawDebugFullscreenSolid();` |
| SceneFile/Transition/Wipe.cpp | 126 | `Renderer::DrawIndexed(4, 0, 0);` |
| SceneFile/Transition/Wipe.h | 34 | `void Draw()             override;` |
| SceneFile/Transition/Zoom.cpp | 74 | `void Zoom::Draw()` |
| SceneFile/Transition/Zoom.cpp | 86 | `DrawDebugFullscreenSolid();` |
| SceneFile/Transition/Zoom.cpp | 111 | `Renderer::DrawIndexed(4, 0, 0);` |
| SceneFile/Transition/Zoom.h | 21 | `void Draw()             override;` |
| SceneFile/TransScene.cpp | 163 | `void TransScene::Draw()` |
| SceneFile/TransScene.cpp | 167 | `m_SceneNext->Draw();` |
| SceneFile/TransScene.cpp | 173 | `m_SceneOld->Draw();` |
| SceneFile/TransScene.h | 66 | `void Draw() 			override;` |
| Square.cpp | 108 | `void Square::Draw()` |
| Square.cpp | 110 | `DrawPrepared(m_UseInstancing ? &m_Instances : nullptr, false);` |
| Square.cpp | 113 | `void Square::DrawPrepared(const std::vector<InstanceData>* instances, bool ownAppearance)` |
| Square.cpp | 152 | `if (!shader &#124;&#124; !Renderer::DrawIndexedInstanced(4, 0, 0, *instances, *shader))` |
| Square.cpp | 157 | `Renderer::DrawIndexed(` |
| Square.cpp | 226 | `void Square::DrawBatch(const std::vector<Square*>& squares)` |
| Square.cpp | 231 | `if (representative) representative->DrawPrepared(&instances, true);` |
| Square.cpp | 243 | `square->Draw();` |
| Square.h | 79 | `void DrawPrepared(const std::vector<InstanceData>* instances, bool ownAppearance);` |
| Square.h | 92 | `void Draw()		  override;` |
| Square.h | 96 | `static void DrawBatch(const std::vector<Square*>& squares);` |
| Square.h | 98 | `void UpdateInstanceBuffers();` |

## Renderer利用箇所

| ファイル | 行 | コード |
|---|---:|---|
| Camera.cpp | 132 | `Renderer::SetViewMatrix(&m_ViewMatrix);` |
| Camera.cpp | 144 | `Renderer::SetProjectionMatrix(&m_ProjectionMatrix);` |
| Camera.cpp | 157 | `Renderer::SetViewMatrix(&m_ViewMatrix);` |
| Camera.cpp | 170 | `Renderer::SetProjectionMatrix(&m_ProjectionMatrix);` |
| DebugGridLine.cpp | 53 | `if (!CreateConstantBuffer(Renderer::GetDevice(), sizeof(GridParams), m_GridParamBuffer.GetAddressOf()))` |
| DebugGridLine.cpp | 85 | `auto* context = Renderer::GetDeviceContext();` |
| DebugGridLine.cpp | 98 | `Renderer::GetDeviceContext()->DrawInstanced(2, m_InstanceCount, 0, 0);` |
| DebugGridLine.cpp | 106 | `auto* context = Renderer::GetDeviceContext();` |
| DebugGridLine.cpp | 108 | `if (!context &#124;&#124; !Renderer::GetDevice())` |
| DebugGridLine.cpp | 113 | `Renderer::BindDefaultConstantBuffers();` |
| DebugGridLine.cpp | 120 | `Renderer::SetWorldMatrix(&identity);` |
| Game.cpp | 78 | `Renderer::Initialize();` |
| Game.cpp | 79 | `DebugUI::Init(Renderer::GetDevice(), Renderer::GetDeviceContext());` |
| Game.cpp | 185 | `Renderer::Start();` |
| Game.cpp | 206 | `Renderer::EndGameRender();` |
| Game.cpp | 207 | `Renderer::ClearBackBuffer();` |
| Game.cpp | 209 | `auto* gameSrv = Renderer::GetGameRenderSRV();` |
| Game.cpp | 216 | `auto srvTex = Renderer::BlitSRVToTexture(gameSrv, 1.0f);` |
| Game.cpp | 219 | `Renderer::BlitSRVToBackbuffer(gameSrv);` |
| Game.cpp | 226 | `Renderer::Finish();` |
| Game.cpp | 253 | `Renderer::Finalize();` |
| IndexBuffer.h | 24 | `device = Renderer::GetDevice();` |
| IndexBuffer.h | 40 | `deviceContext = Renderer::GetDeviceContext();` |
| Material.h | 30 | `dev = Renderer::GetDevice();` |
| Material.h | 60 | `devcontext = Renderer::GetDeviceContext();` |
| Material.h | 80 | `devcontext = Renderer::GetDeviceContext();` |
| Material.h | 95 | `devcontext = Renderer::GetDeviceContext();` |
| MeshRenderer.h | 31 | `ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();` |
| MeshRenderer.h | 43 | `Renderer::DrawIndexed(` |
| MeshRenderer.h | 54 | `Renderer::DrawIndexed(` |
| MeshRenderer.h | 62 | `return Renderer::DrawIndexedInstanced(count, startIndex, baseVertex, instances, shader);` |
| ObjectFile/Circle.cpp | 96 | `Renderer::SetWorldMatrix(&world);` |
| ObjectFile/Circle.cpp | 98 | `ID3D11DeviceContext* dc = Renderer::GetDeviceContext();` |
| ObjectFile/Circle.cpp | 120 | `Renderer::DrawIndexed(m_DrawIndexCount, 0, 0);` |
| ObjectFile/Cube.cpp | 212 | `Renderer::SetWorldMatrix(&worldmtx);` |
| ObjectFile/Cube.cpp | 216 | `devicecontext = Renderer::GetDeviceContext();` |
| ObjectFile/Cube.cpp | 228 | `Renderer::DrawIndexed(` |
| ObjectFile/Light.cpp | 62 | `Renderer::SetWorldMatrix(&worldmtx);` |
| ObjectFile/Light.cpp | 66 | `devicecontext = Renderer::GetDeviceContext();` |
| ObjectFile/Light.cpp | 71 | `Renderer::SetLightBuffer(&m_LightBuffer);` |
| ObjectFile/Light.cpp | 80 | `Renderer::DrawIndexed(` |
| ObjectFile/Model.cpp | 44 | `Renderer::SetWorldMatrix(&world);` |
| ObjectFile/Model.cpp | 114 | `auto dev = Renderer::GetDevice();` |
| ObjectFile/Model.cpp | 134 | `auto ctx = Renderer::GetDeviceContext();` |
| ObjectFile/Model.cpp | 148 | `Renderer::SetWorldMatrix(&world);` |
| ObjectFile/Object.cpp | 48 | `Renderer::BindDefaultConstantBuffers();` |
| ObjectFile/Object.cpp | 49 | `Renderer::BindDefaultSamplers();` |
| ObjectFile/Sphere.cpp | 118 | `ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();` |
| ObjectFile/Sphere.cpp | 135 | `Renderer::SetWorldMatrix(&world);` |
| ObjectFile/Sphere.cpp | 149 | `Renderer::DrawIndexed(m_IndexCount, 0, 0);` |
| ObjectFile/Sphere.cpp | 164 | `Renderer::SetWorldMatrix(&world);` |
| ObjectFile/Sphere.cpp | 166 | `ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();` |
| ObjectFile/Sphere.cpp | 179 | `Renderer::DrawIndexed(m_IndexCount, 0, 0);` |
| Renderer.cpp | 49 | `void Renderer::Initialize(D3D_DRIVER_TYPE driverType)` |
| Renderer.cpp | 275 | `void Renderer::EnsureGameRenderTarget()` |
| Renderer.cpp | 302 | `void Renderer::EnsureBlitRenderTarget()` |
| Renderer.cpp | 330 | `void Renderer::Finalize()` |
| Renderer.cpp | 367 | `void Renderer::Start()` |
| Renderer.cpp | 389 | `void Renderer::EndGameRender()` |
| Renderer.cpp | 401 | `void Renderer::ClearBackBuffer(const float clearColor[4])` |
| Renderer.cpp | 420 | `void Renderer::Finish()` |
| Renderer.cpp | 443 | `void Renderer::SetDepthEnable(bool Enable)` |
| Renderer.cpp | 457 | `void Renderer::SetATCEnable(bool Enable)` |
| Renderer.cpp | 474 | `void Renderer::ResetForFullscreenPass(ID3D11DeviceContext* ctx, UINT w, UINT h)` |
| Renderer.cpp | 502 | `void Renderer::SetWorldViewProjection2D()` |
| Renderer.cpp | 526 | `void Renderer::SetWorldMatrix(DirectX::SimpleMath::Matrix* WorldMatrix)` |
| Renderer.cpp | 535 | `void Renderer::SetViewMatrix(DirectX::SimpleMath::Matrix* ViewMatrix)` |
| Renderer.cpp | 544 | `void Renderer::SetProjectionMatrix(DirectX::SimpleMath::Matrix* ProjectionMatrix)` |
| Renderer.cpp | 554 | `void Renderer::SetMaterial(MATERIAL Material)` |
| Renderer.cpp | 559 | `void Renderer::SetLightBuffer(LightBuffer* LightBuffer)` |
| Renderer.cpp | 569 | `void Renderer::SetUV(float u, float v, float uw, float vh)` |
| Renderer.cpp | 581 | `void Renderer::SetBlendState(int nBlendState)` |
| Renderer.cpp | 590 | `void Renderer::SetDefaultInputLayout()` |
| Renderer.cpp | 595 | `void Renderer::BindDefaultSamplers()` |
| Renderer.cpp | 603 | `void Renderer::BindDefaultConstantBuffers()` |
| Renderer.cpp | 618 | `D3D11_VIEWPORT Renderer::GetRenderTargetView(void)` |
| Renderer.cpp | 638 | `D3D11_VIEWPORT Renderer::GetViewport()` |
| Renderer.cpp | 652 | `ID3D11RenderTargetView* Renderer::GetBackBufferRTV()` |
| Renderer.cpp | 657 | `ID3D11ShaderResourceView* Renderer::GetGameRenderSRV()` |
| Renderer.cpp | 780 | `void Renderer::BlitSRVToBackbuffer(ID3D11ShaderResourceView* srv)` |
| Renderer.cpp | 805 | `ID3D11ShaderResourceView* Renderer::BlitSRVToTexture(ID3D11ShaderResourceView* srv, float alpha)` |
| Renderer.cpp | 834 | `void Renderer::CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName)` |
| Renderer.cpp | 874 | `void Renderer::CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName)` |
| Renderer.cpp | 892 | `void Renderer::SetDrawState()` |
| Renderer.cpp | 902 | `bool Renderer::CreateInstanceBuffer()` |
| Renderer.cpp | 927 | `void Renderer::DrawIndexed(UINT count, UINT startIndex, INT baseVertex)` |
| Renderer.cpp | 939 | `bool Renderer::DrawIndexedInstanced(UINT count, UINT startIndex, INT baseVertex,` |
| Renderer.h | 229 | `inline float Renderer::GetScreenWidth()` |
| Renderer.h | 234 | `inline float Renderer::GetScreenHeight()` |
| SceneFile/Transition/Fade.cpp | 37 | `SetScale(static_cast<float>(Renderer::GetScreenWidth()),` |
| SceneFile/Transition/Fade.cpp | 38 | `static_cast<float>(Renderer::GetScreenHeight()),` |
| SceneFile/Transition/Fade.cpp | 95 | `Renderer::SetDepthEnable(false);` |
| SceneFile/Transition/Fade.cpp | 96 | `Renderer::SetBlendState(BS_ALPHABLEND);` |
| SceneFile/Transition/Fade.cpp | 102 | `Renderer::SetWorldMatrix(&world);` |
| SceneFile/Transition/Fade.cpp | 114 | `Renderer::SetUV(u, v, uw, vh);` |
| SceneFile/Transition/Fade.cpp | 116 | `Renderer::DrawIndexed(4, 0, 0);` |
| SceneFile/Transition/Fade.cpp | 117 | `Renderer::SetDepthEnable(true);` |
| SceneFile/Transition/SnapshotOverlay.h | 24 | `Renderer::BlitSRVToBackbuffer(m_SRV.Get());` |
| SceneFile/Transition/TransitionBase.cpp | 344 | `ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();` |
| SceneFile/Transition/TransitionBase.cpp | 354 | `Renderer::BindDefaultSamplers();` |
| SceneFile/Transition/TransitionBase.cpp | 355 | `Renderer::BindDefaultConstantBuffers();` |
| SceneFile/Transition/TransitionBase.cpp | 368 | `ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();` |
| SceneFile/Transition/TransitionBase.cpp | 372 | `Renderer::SetDepthEnable(false);` |
| SceneFile/Transition/TransitionBase.cpp | 373 | `Renderer::SetBlendState(BS_ALPHABLEND);` |
| SceneFile/Transition/TransitionBase.cpp | 387 | `Renderer::SetWorldMatrix(&world);` |
| SceneFile/Transition/TransitionBase.cpp | 402 | `Renderer::SetUV(u, v, uw, vh);` |
| SceneFile/Transition/TransitionBase.cpp | 405 | `Renderer::DrawIndexed(4, 0, 0);` |
| SceneFile/Transition/TransitionBase.cpp | 409 | `Renderer::SetDepthEnable(true);` |
| SceneFile/Transition/Wipe.cpp | 21 | `SetScale(static_cast<float>(Renderer::GetScreenWidth()),` |
| SceneFile/Transition/Wipe.cpp | 22 | `static_cast<float>(Renderer::GetScreenHeight()),` |
| SceneFile/Transition/Wipe.cpp | 90 | `Renderer::SetDepthEnable(false);` |
| SceneFile/Transition/Wipe.cpp | 91 | `Renderer::SetBlendState(BS_ALPHABLEND);` |
| SceneFile/Transition/Wipe.cpp | 92 | `Renderer::BlitSRVToBackbuffer(srv);` |
| SceneFile/Transition/Wipe.cpp | 93 | `Renderer::SetDepthEnable(true);` |
| SceneFile/Transition/Wipe.cpp | 98 | `Renderer::SetDepthEnable(false);` |
| SceneFile/Transition/Wipe.cpp | 99 | `Renderer::SetBlendState(BS_ALPHABLEND);` |
| SceneFile/Transition/Wipe.cpp | 105 | `Renderer::SetWorldMatrix(&world);` |
| SceneFile/Transition/Wipe.cpp | 123 | `Renderer::SetUV(u, v, uw, vh);` |
| SceneFile/Transition/Wipe.cpp | 126 | `Renderer::DrawIndexed(4, 0, 0);` |
| SceneFile/Transition/Wipe.cpp | 127 | `Renderer::SetDepthEnable(true);` |
| SceneFile/Transition/Zoom.cpp | 90 | `Renderer::SetDepthEnable(false);` |
| SceneFile/Transition/Zoom.cpp | 91 | `Renderer::SetBlendState(BS_ALPHABLEND);` |
| SceneFile/Transition/Zoom.cpp | 97 | `Renderer::SetWorldMatrix(&world);` |
| SceneFile/Transition/Zoom.cpp | 109 | `Renderer::SetUV(u, v, uw, vh);` |
| SceneFile/Transition/Zoom.cpp | 111 | `Renderer::DrawIndexed(4, 0, 0);` |
| SceneFile/Transition/Zoom.cpp | 112 | `Renderer::SetDepthEnable(true);` |
| Shader.cpp | 26 | `auto* device = Renderer::GetDevice();` |
| Shader.cpp | 94 | `auto* context = Renderer::GetDeviceContext();` |
| Shader.cpp | 101 | `auto* ctx = Renderer::GetDeviceContext();` |
| Shader.cpp | 117 | `auto* device = Renderer::GetDevice();` |
| Shader.cpp | 131 | `auto* ctx = Renderer::GetDeviceContext();` |
| Shader.cpp | 145 | `auto* device = Renderer::GetDevice();` |
| Shader.cpp | 158 | `auto* ctx = Renderer::GetDeviceContext();` |
| Shader.cpp | 171 | `auto* device = Renderer::GetDevice();` |
| Shader.cpp | 185 | `auto* ctx = Renderer::GetDeviceContext();` |
| Square.cpp | 91 | `ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();` |
| Square.cpp | 101 | `Renderer::BindDefaultSamplers();` |
| Square.cpp | 102 | `Renderer::BindDefaultConstantBuffers();` |
| Square.cpp | 120 | `Renderer::SetBlendState(BS_ALPHABLEND);` |
| Square.cpp | 129 | `Renderer::SetWorldMatrix(&worldmtx);` |
| Square.cpp | 144 | `if (ownAppearance) Renderer::SetUV(0, 0, 1, 1);` |
| Square.cpp | 145 | `else Renderer::SetUV(u0, v0, u1, v1);` |
| Square.cpp | 152 | `if (!shader &#124;&#124; !Renderer::DrawIndexedInstanced(4, 0, 0, *instances, *shader))` |
| Square.cpp | 157 | `Renderer::DrawIndexed(` |
| Texture.cpp | 27 | `hr = CreateShaderResourceView(Renderer::GetDevice(), img.GetImages(), img.GetImageCount(), meta, m_srv.GetAddressOf());` |
| Texture.cpp | 50 | `hr = CreateShaderResourceView(Renderer::GetDevice(), img.GetImages(), img.GetImageCount(), meta, &m_srv);` |
| Texture.cpp | 60 | `ID3D11DeviceContext* ctx = Renderer::GetDeviceContext();` |
| VertexBuffer.h | 25 | `ID3D11Device* device = Renderer::GetDevice();` |
| VertexBuffer.h | 50 | `ID3D11DeviceContext* deveiceContext = Renderer::GetDeviceContext();` |
| VertexBuffer.h | 65 | `HRESULT hr = Renderer::GetDeviceContext()->Map(` |
| VertexBuffer.h | 72 | `Renderer::GetDeviceContext()->Unmap(m_Buffer.Get(), 0);` |
