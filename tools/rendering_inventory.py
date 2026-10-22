"""List all engine draw dispatches and Renderer call sites (exclude vendor code)."""
from pathlib import Path
import re
root=Path(__file__).resolve().parents[1]
source=root/'就職作品プロジェクト'/'02_ResourceFile'
sections=[('GPU送信・描画ディスパッチ',r'\b(?:Draw\w*\s*\(|DrawSubset\w*\s*\()'),
          ('Renderer利用箇所',r'\bRenderer::\w+\s*\(')]
text=['# 描画の利用箇所一覧','', '対象: 自作コード `02_ResourceFile` の全 `.cpp` / `.h` / `.hpp`。宣言・定義と呼び出しを含む静的一覧。',
      '再生成: `python tools/rendering_inventory.py`。外部ライブラリ ImGui の描画は本体の `DebugUI::Render` から呼ばれる。','']
for heading,pattern in sections:
 text += [f'## {heading}','','| ファイル | 行 | コード |','|---|---:|---|']
 for path in sorted(source.rglob('*')):
  if path.suffix not in ('.cpp','.h','.hpp'):continue
  raw=path.read_bytes()
  try:s=raw.decode('utf-8-sig')
  except UnicodeDecodeError:s=raw.decode('cp932',errors='replace')
  for number,line in enumerate(s.splitlines(),1):
   code=line.strip()
   if code.startswith('//'):continue
   if re.search(pattern,code):
    code=code.split('//')[0].strip().replace('|','&#124;')
    text.append(f'| {path.relative_to(source).as_posix()} | {number} | `{code}` |')
 text.append('')
out=root/'docs'/'RenderingCallSites.md';out.parent.mkdir(exist_ok=True)
out.write_text('\n'.join(text),encoding='utf-8')
