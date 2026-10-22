# 描画処理の分析とインスタンシング対応

## 描画経路

`Application → Game::Initialize/Draw → Scene/Object → Renderer → D3D11` の構成。自作ソース全体の描画ディスパッチとRenderer利用箇所は [RenderingCallSites.md](RenderingCallSites.md) に記録。

| 担当 | 処理と利用元 | 今回の対応 |
|---|---|---|
| Renderer | デバイス・swap chain・定数バッファ・blend/depth/sampler、描画先、Present | 共通動的インスタンスVBと送信API、生成・解放・再初期化、行列の初期値を追加。SM5.0に合わせFL11.0を要求 |
| Shader / ShaderManager | Game初期化でHLSL登録、ObjectのSetGPU、デバッグ時の再読込 | 通常版とインスタンス版のVS・入力レイアウトを同時生成。両方の成功後に置換するため再読込失敗時も旧版を保持 |
| VertexBuffer / IndexBuffer / Mesh / StaticMesh | slot 0の頂点、index、モデル読込・subset構築 | 頂点形式は48バイトのまま維持。slot 1に160バイトのインスタンス属性を追加 |
| MeshRenderer / Model | Model::Draw / DrawSkyの各subsetを材質・textureごとに描画 | subsetのindex数・開始index・base vertexを維持し共通APIに送信。直接利用できるDrawInstanced/DrawSubsetInstancedも追加 |
| Square | スプライト、背景、UI、Stone、Bird、Bullet等の基礎 | 旧b6配列方式を廃止し共通ストリームへ移行。公開したInstanceTransform2Dに回転も追加 |
| Cube / Light | triangle listによる36 indexの描画 | Object::DrawInstancesから同一geometryを複数描画可能。Lightの照明設定は共通値 |
| Sphere / Skydome | 球、空。空は専用rasterizer/depth状態を保存・復帰 | indexed送信を共通化。自動で空を複製する処理は追加しない |
| Circle | 扇形geometry、表示割合に応じたindex数 | 共通indexed送信を使用。インスタンス間で表示割合・geometryは共有 |
| DebugGridLine | 2本の基準線をSV_InstanceIDで複製 | 既存DrawInstancedを維持。未使用のID用VBと二重の入力レイアウトを削除。b6は引き続きグリッド専用 |
| ParticleEmitter | 各particleのSquareを描画 | 隣接する同一条件のSquareをDrawBatchにまとめる。従来vcxprojに未登録だったParticle.cpp/hを登録 |
| GameSceneDodge / Stone | シーンのオブジェクト列、落下する石 | 隣接した有効な石だけをまとめる。非Stoneに到達したら送信し、透明物とUIの順序を維持 |
| Scene / Rocket / Gunman | オブジェクトの仮想Drawを順次呼ぶ | 通常の順序を維持。各Objectは明示的にDrawInstancesを呼べる |
| Timer / Theme / Button等 | 子Object描画、Square継承、色・UV等の設定 | 通常経路を継続。複合Objectの自動バッチ化はしない |
| TransScene / RenderTarget | 新旧シーンのオフスクリーン描画、RTV/SRV切替と状態復帰 | 通常経路を継続。共有バッファをb6に置かないため干渉しない |
| Fade / Wipe / Zoom / TransitionBase | triangle stripの全画面四角形 | indexed送信を共通化。通常描画ではインスタンス入力を要求しない |
| RendererのBlit | SV_VertexIDから全画面三角形、Draw(3) | 専用シェーダー経路を維持 |
| DebugUI / ImGuiUtil | ImGuiによるオフスクリーン画像・操作UI | ImGuiの専用backendを維持。Object::SetGPUで次の描画の既定CB/samplerを再設定 |

## 修正した問題

- 旧Squareは512個で切り捨て、b6に位置とscaleを書き込むだけでVS_Alphaから専用VSに切り替えていなかった。
- 旧Stoneは各石のworld相当の座標・scaleに代表StoneのWorldをさらに掛け、回転と個別色・UVも反映できなかった。
- 旧インスタンスCBは設定のたびに再生成し、空の配列は単体描画に戻ってしまっていた。
- 全VSの入力レイアウトに、通常描画では供給されないTEXCOORD1のIDが含まれていた。グリッドも実際にはSV_InstanceIDを使用していた。
- VS_Skydomeのb0にworld/view/projectionをまとめた宣言は、エンジンのb0/b1/b2の分割方式と不一致だった。
- Renderer初期化でworld/view/projectionが未設定だった。単位行列で初期化し、共有インスタンスVBも初期化時に確保する。

## データ・状態の契約

`InstanceData` は絶対world行列、色の乗数、UV矩形（offset.xy、scale.zw）。共有Objectのworldはインスタンス描画では重ねて掛けない。C++側は通常のSimpleMath行列を渡し、転置不要。GPUのslot 1にはworld、法線用inverse-transpose、color、UVを順に配置する。slot 0は従来の頂点形式。

1024件分の動的VBを再利用し、WRITE_DISCARDで順次更新する。1025件以上も複数回のDrawIndexedInstancedで全件を送る。空配列は0 draw。逆行列を作れないゼロscale等の行列では法線行列に単位行列を使い、NaNを避ける。

DrawIndexedInstancedはVS・input layout・slot 1を保存／復帰する。材質のb4、texture、sampler、blend/depth、描画先、cameraは呼び出し元の設定を使用する。特にModelのsubset用MaterialバッファをRendererの既定b4で上書きしない。割当・Map失敗はfalseを返す。Object/Squareの高水準APIでは描画失敗を例外として通知する。

対応VS: `VS_Alpha`, `VS_Default`, `VS_Unlit`, `VS_Instansing2D`（互換名）, `VS_Skydome`。`VS_DebugGrid`は自身の専用方式。通常版と`USE_INSTANCING=1`版をShader::Createで生成する。独自VSを増やす場合はCommonのインスタンス属性とGetWorld/GetColor/GetUV/GetNormalを使用し、Shader.cppの対応判定にも登録する。

## 利用例

同じモデル・材質・cameraの3Dオブジェクトを複数描画する場合:

```cpp
std::vector<InstanceData> instances(2000);
for (size_t i = 0; i < instances.size(); ++i) {
    instances[i].world = Matrix::CreateScale(2.0f)
        * Matrix::CreateRotationY(0.3f)
        * Matrix::CreateTranslation(static_cast<float>(i % 50) * 4, 0,
                                    static_cast<float>(i / 50) * 4);
    instances[i].color = Color(1, 1, 1, 1);
}
model->DrawInstances(instances); // Modelの全subsetを材質ごとに送信
```

Squareに位置・拡縮・回転リストを設定する場合:

```cpp
std::vector<Square::InstanceTransform2D> transforms;
transforms.push_back({ Vector3(100, 100, 0), Vector3(50, 50, 1), Vector3(0, 0, .3f) });
square->SetInstanceTransforms(transforms);
square->SetInstancingEnabled(true);
square->Draw();
square->SetInstancingEnabled(false); // 次回は元の単体描画
```

既存のSquare群の色・UV・transformをそのまままとめる場合:

```cpp
Square::DrawBatch({ squareA.get(), squareB.get(), squareC.get() });
```

DrawBatchは入力順を保持し、texture、VS/PS/GS/CS、camera、camera mode、頂点geometryが変わる境界で分割する。独自Drawの付加処理や個別Materialの意味を持つ派生クラスは自動的に対象とせず、呼び出し側で適格なSquareだけを渡す。現在の接続先はStoneとParticleの標準Square描画。

`Object::DrawInstances`は共有geometryを持つ単体Object向け。子Objectも描く複合Objectや、透明な複数subsetモデルを順序依存で重ねる用途では、必要な順序ごとに分割して呼ぶ。異なるtextureやgeometryが無条件に1 drawになるAPIではない。空のcamera追従変換なども絶対world指定時は呼び出し側で用意する。

## 検証

`tests/InstancingTests.vcxproj` は製品のRenderer.cpp/Shader.cpp/RenderTarget.cpp/dx11helper.cppを直接コンパイルして使用。非表示ウィンドウとWARPで実行し、通常描画とのfloat render targetの画素比較、GPU pipeline statisticsによるprimitive数、subset offset、1024件境界、通常描画への復帰、slot 1復元、材質バッファ、shader reload、Renderer再初期化を検証する。別途非均等scaleの法線を数値検証。

```powershell
MSBuild tests/InstancingTests.vcxproj /p:Configuration=Release /p:Platform=x64
Push-Location 就職作品プロジェクト
../tests/x64/InstancingTests.exe
Pop-Location
```

この端末の通常ビルドはMSVCのPDB管理エラーC1902/LNK1101が発生したため、`tools/build_verification.py Debug` / `Release` は一時vcxprojで/Z7・PCH無効・リンクPDB無効にして検証する。メインvcxprojのデバッグ設定を変更せず、終了時に一時projectを削除する。出力はプロジェクトの`x64/InstancingDebug` / `x64/InstancingRelease`。

ゲーム全体の実機での画面遷移・全ステージの目視確認とGPU性能計測は、このオフスクリーンテストには含まれない。
