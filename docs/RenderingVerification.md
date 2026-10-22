# インスタンシング検証結果

検証日: 2026-10-22

| 検証 | 結果 |
|---|---|
| x64 Debug、ゲーム全体のコンパイル・リンク | 成功（端末のPDB問題を検証用プロジェクトで回避） |
| x64 Release、ゲーム全体のコンパイル・リンク | 成功（同上） |
| HLSLの通常版とインスタンス版のコンパイル・InputLayout生成 | 成功 |
| WARPでの通常／インスタンス画素比較 | VS_Alpha / VS_Default / VS_Unlit / VS_Instansing2Dで成功 |
| 件数 | 0 / 1 / 512 / 513 / 1024 / 1025 / 2049 の全ケース成功 |
| GPU pipeline statistics | 2三角形 × 件数と一致。1024件境界で切り捨て・重複なし |
| StartIndex=3、BaseVertex=1のsubset描画 | 成功 |
| 非単位の代表Worldからの独立、回転、拡縮、移動、色、UV | 成功 |
| 独自Materialのb4を使った描画 | 成功 |
| slot 1の既存VB・stride・offsetの復帰 | 成功 |
| ScopedInstancesによる送信とスコープ終了後の単体描画 | 成功 |
| 非均等scaleの法線 | 成功。scale=(2,1,1), normal=(1,1,0) → (0.447214,0.894427,0) |
| シェーダー再読込・再読込失敗時の旧版保持 | 成功 |
| グリッド／空のシェーダー生成 | 成功 |
| Renderer終了・再初期化 | 成功 |

実行したテストは `tests/InstancingTests.cpp`。ゲーム全体の目視プレイ、全ステージ・画面遷移の実機操作、ハードウェアGPUでの性能計測は未実施。

通常のMSBuildでは、この端末のMSVCデバッグ情報管理でC1902/LNK1101が発生した。`tools/build_verification.py` はデバッグ情報を/Z7に変更し、PCHとリンクPDBを無効にした一時プロジェクトで同じゲームソースをビルドする。メインプロジェクトの通常のデバッグ設定は維持している。
