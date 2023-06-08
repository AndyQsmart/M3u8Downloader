pragma Singleton

import QtQuick 2.15

Item {
    property var download_list: []
    property var history_list: []
    property var trash_list: []

    function addDownloadItem(item) {
        // const { task_id, file_name, file_path, m3u8, file_urls, gid_list } = item
        download_list.push(item)
    }

    function getDownloadList() {
        return download_list
    }
}
