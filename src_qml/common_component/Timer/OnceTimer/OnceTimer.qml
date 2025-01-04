import QtQuick 2.15

// 单次运行Timer，模拟JS端的setTimeout
Timer {
    id: timer
    interval: 0
    repeat: false
    running: false
    triggeredOnStart: false

    property int current_task_id: 0
    property var task_list: ([])
    property int next_trigger_time: -1

    function setTimeout(func, mtimes) {
        let current_time = Date.now()
        let trigger_time = current_time+mtimes
        task_list.push({
            id: current_task_id,
            func,
            trigger_time,
        })
        current_task_id += 1
        if (timer.running) {
            if (trigger_time < next_trigger_time) {
                next_trigger_time = trigger_time
                timer.interval = mtimes
                timer.restart()
            }
        }
        else {
            next_trigger_time = trigger_time
            timer.interval = mtimes
            timer.start()
        }
        return current_task_id-1
    }

    function clearTimeout(task_id) {
        let index = -1
        for (let i = 0; i < task_list.length; i++) {
            let the_task = task_list[i]
            if (the_task.id === task_id) {
                index = i
                break
            }
        }
        if (index != -1) {
            task_list.splice(index, 1)
        }
    }

    onTriggered: {
        let current_time = Date.now()
        let new_task_list = []
        let new_trigger_time = -1

        for (let i = 0; i < timer.task_list.length; i++) {
            let the_task = timer.task_list[i]

            if (the_task.trigger_time <= current_time) {
                the_task.func()
            }
            else {
                new_task_list.push(the_task)
                if (new_trigger_time === -1 || new_trigger_time > the_task.trigger_time) {
                    new_trigger_time = the_task.trigger_time
                }
            }
        }

        if (new_task_list.length > 0) {
            timer.task_list = new_task_list
            timer.next_trigger_time = new_trigger_time
            timer.interval = parseInt(new_trigger_time-current_time)
            timer.restart()
        }
        else {
            timer.task_list = []
        }
    }
}
