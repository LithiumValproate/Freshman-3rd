QT       += core gui webenginewidgets sql

CONFIG += c++17

TARGET = QtWebSchoolSys
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

# Ensure build directories exist
QMAKE_PRE_LINK += "mkdir -p $OBJECTS_DIR"
QMAKE_PRE_LINK += "mkdir -p $MOC_DIR"
QMAKE_PRE_LINK += "mkdir -p $RCC_DIR"

# Add the SQL driver plugin to the deployment
macx: {
    SQL_PLUGINS_PATH = $$[QT_INSTALL_PLUGINS]/sqldrivers
    SQL_PLUGINS_DEST = $$DESTDIR/$TARGET.app/Contents/PlugIns/sqldrivers
    deployment.files += $$SQL_PLUGINS_PATH/libqsqlmysql.dylib
    deployment.path = $$SQL_PLUGINS_DEST
    QMAKE_POST_LINK += \
        "mkdir -p $$SQL_PLUGINS_DEST && " \
        "cp $$SQL_PLUGINS_PATH/libqsqlmysql.dylib $$SQL_PLUGINS_DEST/"
    INSTALLS += deployment
}