import QtQuick 2.15
import "../../../common_component/SQL/QSQL"
import "../TableFactory"

Item {
    id: table
    property string table_name: ''
    property var table_field: ({})
    property var table_meta: null

    function getTable() {
        TableFactory.connect()
        TableFactory.migrateTable(table_name, table_field, function() {
            console.log(`(TableBase.qml)migrateTable success: ${table_name}`)
        })
        if (!table_meta) {
            table_meta = QSQL.db.define(table_name, table_field)
            QSQL.db.schemaSync()
        }
        return table_meta
    }

    function getModel() {
        return QSQL.db
    }

    // 查询得到的内容转js对象
    function toObject(query) {
        let ans = {
            id: query.id,
        }
        for (let key in table_field) {
            ans[key] = query[key]
        }

        return ans
    }

    function create(arg) {
        console.log("(TableBase.qml)create:", JSON.stringify(arg))
        let Table = table.getTable()
        let new_item = new Table(arg)
        QSQL.db.add(new_item)
        save()
        return new_item
    }

    function remove(item) {
        console.log("(TableBase.qml)remove:", JSON.stringify(item))
        QSQL.db.remove(item)
    }

    function save(callback) {
        QSQL.db.save(null, callback)
    }
}
