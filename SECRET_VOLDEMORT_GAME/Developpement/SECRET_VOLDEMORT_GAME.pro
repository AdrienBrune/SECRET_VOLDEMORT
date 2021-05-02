QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialog_menu.cpp \
    dialog_question.cpp \
    main.cpp \
    controller.cpp \
    soundmanager.cpp \
    tcp_api.cpp \
    widget_boarddeatheaters.cpp \
    widget_boardphenixorder.cpp \
    widget_card.cpp \
    widget_deatheatersteam.cpp \
    widget_gameboard.cpp \
    widget_gameboard_bot.cpp \
    widget_gameboard_center.cpp \
    widget_gameboard_left.cpp \
    widget_gameboard_right.cpp \
    widget_gameboard_top.cpp \
    widget_gamestate.cpp \
    widget_player.cpp \
    widget_popupmessage.cpp \
    widget_screencheckrole.cpp \
    widget_screenendgame.cpp \
    widget_screenlawcards.cpp \
    widget_screenpowerunlocked.cpp

HEADERS += \
    controller.h \
    data.h \
    dialog_menu.h \
    dialog_question.h \
    soundmanager.h \
    tcp_api.h \
    widget_boarddeatheaters.h \
    widget_boardphenixorder.h \
    widget_card.h \
    widget_deatheatersteam.h \
    widget_gameboard.h \
    widget_gameboard_bot.h \
    widget_gameboard_center.h \
    widget_gameboard_left.h \
    widget_gameboard_right.h \
    widget_gameboard_top.h \
    widget_gamestate.h \
    widget_player.h \
    widget_popupmessage.h \
    widget_screencheckrole.h \
    widget_screenendgame.h \
    widget_screenlawcards.h \
    widget_screenpowerunlocked.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
    controller.ui \
    dialog_menu.ui \
    dialog_question.ui \
    widget_boarddeatheaters.ui \
    widget_boardphenixorder.ui \
    widget_gameboard.ui \
    widget_gameboard_bot.ui \
    widget_gameboard_center.ui \
    widget_gameboard_left.ui \
    widget_gameboard_right.ui \
    widget_gameboard_top.ui \
    widget_gamestate.ui \
    widget_popupmessage.ui \
    widget_screencheckrole.ui \
    widget_screenlawcards.ui \
    widget_screenpowerunlocked.ui

RESOURCES += \
    ressources/res.qrc

ICON += ressources/icone.ico
RC_ICONS += ressources/icone.ico
