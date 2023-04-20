# -*- mode: python ; coding: utf-8 -*-


block_cipher = None

add_files = [
    ('src_qml', 'src_qml'),
    ('./src/libs/math_type/MT6.dll', '.'),
]

a = Analysis(['main.py'],
             pathex=['C:\\Users\\AndyQsmart\\Documents\\Qt\\MathTypeToolsPy'],
             binaries=[],
             datas=add_files,
             hiddenimports=['PySide2.QtQuick'],
             hookspath=[],
             hooksconfig={},
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher,
             noarchive=False)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)

exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,  
          [],
          name='MathType工具',
          debug=False,
          bootloader_ignore_signals=False,
          strip=False,
          upx=True,
          upx_exclude=[],
          runtime_tmpdir=None,
          console=False,
          icon='C:\\Users\\AndyQsmart\\Documents\\Qt\\MathTypeToolsPy\\build_res\\logo.ico',
          disable_windowed_traceback=False,
          target_arch=None,
          codesign_identity=None,
          entitlements_file=None )
