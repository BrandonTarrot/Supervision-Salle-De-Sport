QT += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    interface_utilisateur.cpp

HEADERS += \
    mainwindow.h \
    interface_utilisateur.h

FORMS += \
    mainwindow.ui \
    Interface_Utilisateur.ui \
    Historique.ui
