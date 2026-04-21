QT += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    interface_utilisateur.cpp \
    mot_de_passe_oublie.cpp \
    smtp.cpp

HEADERS += \
    mainwindow.h \
    interface_utilisateur.h \
    mot_de_passe_oublie.h \
    smtp.h

FORMS += \
    mainwindow.ui \
    Interface_Utilisateur.ui \
    mot_de_passe_oublie.ui
