import os

QT_CLANG_PATH = "/Applications/Qt/5.15.2/clang_64"
ARIA2_DLL_PATH = "lib/aria2/lib/mac"
ARIA2_DLL_CHANGE_MAP = {
    "/usr/local/lib/libaria2.0.dylib": "libaria2.0.dylib",
}

if __name__=="__main__":
    print(">>>>>>process aria2>>>>>>>")
    for dll_path in ARIA2_DLL_CHANGE_MAP:
        dll_dir = ARIA2_DLL_PATH
        dll_name = ARIA2_DLL_CHANGE_MAP[dll_path]
        cp_cmd = "cp %s/%s M3u8Downloader.app/Contents/Frameworks/" % (dll_dir, dll_name)
        print(cp_cmd)
        os.system(cp_cmd)
        change_name_cmd = 'install_name_tool -change "%s" "@rpath/%s" M3u8Downloader.app/Contents/MacOS/M3u8Downloader' % (dll_path, dll_name)
        print(change_name_cmd)
        os.system(change_name_cmd)

    macdeployqt_path = QT_CLANG_PATH+"/bin/macdeployqt"
    qml_dir = QT_CLANG_PATH+"/qml"
    deployqt_cmd = "%s M3u8Downloader.app -qmldir=%s" %(macdeployqt_path, qml_dir)
    print(deployqt_cmd)
    result = os.system(deployqt_cmd)