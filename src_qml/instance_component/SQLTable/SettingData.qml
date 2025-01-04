pragma Singleton

import QtQuick 2.15
import "../../common_component/SQL/TableBase"

TableBase {
    table_name: 'SettingData'
    table_field: ({
        name: 'TEXT',
        value: 'TEXT',
    })

    function getValue(name, callback) {
        let query = getTable().all().filter('name', '=', name)
        query.list(null, function (data) {
            if (data.length === 0) {
                callback(null)
            }
            else {
                let res_data = null
                try {
                    res_data = JSON.parse(data[0].value)
                }
                catch (e) {
                }
                callback(res_data)
            }
        })
    }

    function setValue(name, value) {
        let query = getTable().all().filter('name', '=', name)
        query.list(null, function (data) {
            if (data.length === 0) {
                create({
                    name,
                    value: JSON.stringify(value),
                })
            }
            else {
                let the_data = data[0]
                the_data.value = JSON.stringify(value)
                save()
            }
        })
    }
}
