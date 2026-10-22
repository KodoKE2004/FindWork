# Dodge / Shot 描画負荷の改善

検証日: 2026-10-22

## 調査結果と変更

既存のインスタンシング変更を維持したまま、以下を追加した。画面・レンダーターゲットの解像度、VSync、ゲーム進行速度、透明描画順は変更していない。

| 箇所 | 確認した処理 | 今回の変更 |
|---|---|---|
| Renderer::Finish | Debugで毎フレームID3D11Debugを取得し、取得成功時に全リソースの詳細レポートを出力 | フレームループから削除。診断インターフェースが利用できない環境での影響は小さい |
| Renderer::Finish | ReleaseでGame::Drawと同じ全画面Blitを再実行 | Game::Drawの1回に統一 |
| GameSceneShot::Draw | 生存弾を1個ずつ描画 | 隣接する互換弾をDrawBatchへ。非弾オブジェクトの前で必ずflushし、死んだ弾は除外 |
| 両シーンのDraw | 各SquareがOBJECT→UI→OBJECTと往復し、その都度View/Projectionを転送 | シーンの2D描画をUIスコープで囲む。Shotの空だけOBJECTに切り替えて復帰 |
| Square::SetPipeline | Object::SetGPUの直後に同じsampler/constant bufferを再バインド | 重複呼び出しを削除 |
| Material::Update | 内容が同じでも毎描画UpdateSubresource | 前回転送内容と一致する場合は転送省略。バインドは維持。SetMaterialによる直接上書きとCreate時はキャッシュを無効化 |
| TextureManager::GetTexture | キャッシュヒットでも色付きコンソール出力とstd::endlによるflush | キャッシュヒットのログを削除。ロードの成功・失敗ログは維持 |
| GameSceneDodgeの岩生成 | AddObject内部でInitializeした直後に再度Initialize | 2回目を削除。1個につき不要なVB/IB/Materialの再生成を除去 |
| GameSceneDodgeの岩生成 | 生成する岩1個ごとに全オブジェクトを走査 | 生成グループごとに1回取得し、新規配置した岩をリストに追加。重なり判定は維持 |

弾N個の連続した互換グループは、N回の描画から原則ceil(N/1024)回へ減る。これはコード上の削減量で、実機FPSの測定値ではない。

DebugのBlitSRVToTextureはalphaを1に補正してImGui表示に渡している。元のゲームRTは描画元のalphaを保存するため、単純なSRVの直渡しでは表示が変わる。このコピーは維持した。

## 検証

- tools/build_verification.py Debug / Release: ゲーム全体のコンパイルとリンク成功。既存のPDB問題を回避する一時プロジェクトを使用。
- tests/InstancingTests.vcxproj Release x64: ビルド成功。
- tests/x64/InstancingTests.exe: WARPで全テスト成功。
- 既存の4種類のシェーダー、0〜2049インスタンス、画素比較、subset、描画状態復帰、reload、Renderer再初期化を確認。
- 新規にMaterialのb4をGPUから読み戻し、初回、未変更、外部でバインド解除後、色変更、直接上書き後のUpdate、再Createを確認。
- git diff --check: 成功。

ログ: build-instancing-debug.log / build-instancing-release.log / build-performance-tests.log / test-performance-results.log。

## 未確認事項

実機GPUでのプレイ、両シーンのFPS・CPU/GPU時間の変更前後比較は未実施。上記はコードから確認した負荷の削減であり、著しいFPS低下の主因や改善率は断定していない。WARPテストは描画の正しさを検証するもので、ゲーム全体の性能測定ではない。

実機比較では同じビルド構成、ステージ難度、画面サイズ、デバッガ接続条件で比較し、Dodgeの岩生成時とShotの連射時を含める。Present(1,0)による待機時間と描画処理時間を分けて評価する。

別途、Dodgeの岩はシーンUpdate内とGame::Update内の2箇所からUpdateされている。変更すると落下速度や判定タイミングが変わるため、今回の描画最適化には含めていない。
