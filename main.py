# This Python file uses the following encoding: utf-8
import sys
import os

from PySide2.QtCore import QCoreApplication, Qt
from PySide2.QtGui import QGuiApplication, QFont
from PySide2.QtQml import QQmlApplicationEngine
from PySide2.QtQuickControls2 import QQuickStyle

# from src.file_util.file_tools import FileTools

if __name__ == "__main__":
    # 高分辨率适配
    QCoreApplication.setAttribute(Qt.AA_EnableHighDpiScaling)
    QGuiApplication.setHighDpiScaleFactorRoundingPolicy(Qt.HighDpiScaleFactorRoundingPolicy.PassThrough)

    app = QGuiApplication(sys.argv)
    # QQuickStyle.setStyle('Material')
    defualt_font = QFont()
    defualt_font.setFamily("Arial")
    app.setFont(defualt_font)
    app.setOrganizationName('Zhibing')
    app.setOrganizationDomain('www.jiuzhangzaixian.com')

    engine = QQmlApplicationEngine()

    # # 文件相关处理
    # file_tools = FileTools()
    # engine.rootContext().setContextProperty('FileTools', file_tools)

    engine.load(os.path.join(os.path.dirname(__file__), "src_qml/main.qml"))
    # print(engine.offlineStoragePath())

    if not engine.rootObjects():
        sys.exit(-1)

    # # 连接信号
    # rootObject = engine.rootObjects()[0]
    # QMLSignal.instance().connectQMLCallback(rootObject.onPythonSignal)

    sys.exit(app.exec_())
