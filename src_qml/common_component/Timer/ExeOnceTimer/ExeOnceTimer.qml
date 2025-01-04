import QtQuick 2.15
import "../OnceTimer"

// 在指定时间内只执行一次，如果反复调用会一直延时，直到最后一次被调用
OnceTimer {
    id: root

    function exeOnceAtTime(func, during_time) {
        // 在指定时间内只执行一次，如果反复调用会一直延时，直到最后一次被调用
        let lastFlag = {
            last_timer: null,
            need_exe: true,
        }

        function exeFunc() {
            let args = arguments
            root.clearTimeout(lastFlag.last_timer)
            lastFlag.need_exe = false

            let flag = {
                need_exe: true,
                last_timer: root.setTimeout(function() {
                    if (flag.need_exe === true) {
                        func.apply(null, args)
                    }
                }, during_time)
            }
            lastFlag = flag
        }
        return exeFunc
    }
}
