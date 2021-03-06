QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    server_ctr.cpp \
    tcp_api.cpp \
    util_functions.cpp \
    widget_board.cpp \
    widget_card.cpp \
    widget_player.cpp

HEADERS += \
    server_ctr.h \
    tcp_api.h \
    types.h \
    util_functions.h \
    widget_board.h \
    widget_card.h \
    widget_player.h

FORMS += \
    server_ctr.ui \
    widget_board.ui \
    widget_card.ui \
    widget_player.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ressources/res.qrc

RC_ICONS += ressources/icone.ico
