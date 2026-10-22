"""Build a temporary project with embedded debug info, avoiding this host's broken PDB server."""
from pathlib import Path
import subprocess, sys
root=Path(__file__).resolve().parents[1]
project=root/'就職作品プロジェクト'
original=project/'就職作品プロジェクト.vcxproj'
temp=project/'InstancingVerification.vcxproj'
configuration=sys.argv[1] if len(sys.argv)>1 else 'Debug'
text=original.read_text(encoding='utf-8-sig')
text=text.replace('<PrecompiledHeader>Use</PrecompiledHeader>','<PrecompiledHeader>NotUsing</PrecompiledHeader>')
text=text.replace('>Create</PrecompiledHeader>','>NotUsing</PrecompiledHeader>')
text=text.replace('<AdditionalIncludeDirectories>','<AdditionalIncludeDirectories>$(ProjectDir);')
text=text.replace('<ClCompile>','<ClCompile><DebugInformationFormat>OldStyle</DebugInformationFormat>')
import re
text=re.sub(r'<DebugInformationFormat>(?!OldStyle).*?</DebugInformationFormat>','',text)
text=re.sub(r'<GenerateDebugInformation>.*?</GenerateDebugInformation>','',text)
text=text.replace('<Link>','<Link><GenerateDebugInformation>false</GenerateDebugInformation>')
temp.write_text(text,encoding='utf-8-sig')
vswhere=Path('C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe')
vs=subprocess.check_output([str(vswhere),'-latest','-property','installationPath'],text=True).strip()
try:
 with (root/f'build-instancing-{configuration.lower()}.log').open('w',encoding='utf-8') as log:
  result=subprocess.run([str(Path(vs)/'MSBuild/Current/Bin/MSBuild.exe'),str(temp),
   f'/p:Configuration={configuration}','/p:Platform=x64',f'/p:IntDir=x64/Instancing{configuration}/',
   f'/p:OutDir=x64/Instancing{configuration}/','/p:LinkIncremental=false','/v:minimal','/nologo'],stdout=log,stderr=subprocess.STDOUT)
 sys.exit(result.returncode)
finally:
 temp.unlink()
