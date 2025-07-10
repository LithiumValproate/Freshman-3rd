QT += core widgets webenginewidgets webchannel

CONFIG += c++17

TARGET = WebStudentSys
TEMPLATE = app

VERSION = 1.0.0
QMAKE_TARGET_COMPANY = "Qt学习项目"
QMAKE_TARGET_PRODUCT = "Qt Web学生管理系统"

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    webbridge.cpp \
    im/user.cpp \
    im/room.cpp \
    im/im_go_bridge/im_bridge.cpp

HEADERS += \
    mainwindow.h \
    webbridge.h \
    struct/student.h \
    struct/stu_with_score.h \
    struct/other_users.h \
    struct/course.h \
    im/user.h \
    im/room.h \
    im/message.h \
    im/im_go_bridge/im_bridge.h

RESOURCES += \
    resources.qrc

DESTDIR = $$PWD/bin
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
RCC_DIR = $$PWD/build/rcc