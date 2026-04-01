QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    forgotpassword.cpp \
    main.cpp \
    mainwindow.cpp \
    adminwindow.cpp

HEADERS += \
    forgotpassword.h \
    mainwindow.h \
    adminwindow.h

FORMS += \
    adminwindow.ui \
    forgotpassword.ui \
    mainwindow.ui

RESOURCES += \
    ressources.qrc

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
