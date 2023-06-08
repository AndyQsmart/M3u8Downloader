pragma Singleton

import QtQuick 2.15

Item {
    function exeFunc(name, arg, callback) {
        let post_arg = {
            jsonrpc:'2.0',
            id:'qt',
            method: name,
            params: arg,
        }
        if (arg && arg.length > 0) {
            post_arg.params = arg
        }
        // console.log("Aria2Util:exeFunc", JSON.stringify(post_arg))
        QHttp.postJSON("http://localhost:6800/jsonrpc", post_arg, callback)
    }

    function init() {
        DownloadM3u8.initAria2()
    }

    function shutdown(callback) {
        exeFunc("aria2.shutdown", [], callback)
    }

    function forceShutdown(callback) {
        exeFunc("aria2.forceShutdown", [], callback)
    }

    function getGlobalOption(callback) {
        exeFunc("aria2.getGlobalOption", [], callback)
    }

    function getGlobalStat(callback) {
        exeFunc("aria2.getGlobalStat", [], function(res) {
            console.log(res)
            let data = JSON.parse(res)
            const { result } = data
            if (callback) {
                callback(result)
            }
        })
    }

    function tellStatus(gid, callback) {
        exeFunc("aria2.tellStatus", [gid], function(res) {
            // console.log(res)
            let data = JSON.parse(res)
            const { result } = data
            if (callback) {
                callback(result)
            }
        })
    }

    function tellActive(callback) {
        exeFunc("aria2.tellActive", [], function(res) {
            // console.log(res)
            let data = JSON.parse(res)
            const { result } = data
            if (callback) {
                callback(result)
            }
        })
    }

    function getTempSaveFolder(file_path, file_name) {
        let save_folder = file_path[file_path.length-1] === "/" ? `${file_path}${file_name}_ts` : `${file_path}/${file_name}_ts`;
        return save_folder
    }

    function downloadUri(file_path, file_name, file_urls, callback) {
        let gid_list = []
        let rest = file_urls.length
        let save_folder = getTempSaveFolder(file_path, file_name)
        for (let i = 0; i < file_urls.length; i++) {
            let uri = file_urls[i]
            exeFunc("aria2.addUri", [
                [uri],
                {
                    dir: save_folder,
                    out: `${i+1}.ts`,
                }
            ], function(res) {
                let data = JSON.parse(res)
                const { result } = data
                gid_list[i] = result
                rest -= 1
                if (rest === 0) {
                    if (callback) {
                        callback(gid_list)
                    }
                }
            })
        }
    }
}
