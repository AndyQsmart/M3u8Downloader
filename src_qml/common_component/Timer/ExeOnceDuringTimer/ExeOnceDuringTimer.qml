import QtQuick 2.15
import "../OnceTimer"

// 在规定时间内只执行一次，如果反复调用，会间隔执行，并且保证最后一次被执行
OnceTimer {
    id: root

    function exeOnceDuringTime(func, during_time) {
        // 在规定时间内只执行一次，如果反复调用，会间隔执行，并且保证最后一次被执行
        let lastFlag = {
            need_exe: true,
            exe_arg: null,
        }
        function exeFunc(){
            lastFlag.exe_arg = arguments

            if (!lastFlag.need_exe) {
                return
            }

            lastFlag.need_exe = false

            root.setTimeout(function() {
                func.apply(null, lastFlag.exe_arg)
                lastFlag.need_exe = true
            }, during_time)
        }
        return exeFunc
    }
}
