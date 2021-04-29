#ifndef WIDGET_GAMEBOARD_H
#define WIDGET_GAMEBOARD_H

#include <QWidget>
#include <QPainter>
#include "widget_deatheatersteam.h"
#include "widget_gameboard_bot.h"
#include "widget_gameboard_center.h"
#include "widget_gameboard_right.h"
#include "widget_gameboard_top.h"
#include "widget_gameboard_left.h"
#include "widget_screencheckrole.h"
#include "widget_screenlawcards.h"
#include "widget_screenendgame.h"
#include "widget_screenpowerunlocked.h"
#include "soundmanager.h"

namespace Ui {
class Widget_Gameboard;
}

class Widget_Gameboard : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_Gameboard(QWidget *parent = nullptr, S_MESSAGE* MSG = nullptr, E_VOTE* playerVotes = nullptr);
    ~Widget_Gameboard();

signals:
    void sig_voted(E_VOTE);
    void sig_clickOnPlayer(E_IDENTIFIER);
    void sig_draw();
    void sig_menu();
    void sig_playSound(SoundManager::E_SOUND);

public slots:
    void onHideFacistTeam();

public:
    void updateBoard();
    void switchCurrentScreenWith(QWidget* screen);
    void displayFacistTeam();
    void revealVotes(bool toggle);
    void enableClickOnPlayers(bool toggle);
    void enableVote(bool toggle);
    void enableDraw(bool toggle);

public:
    Widget_FacistTeam* wFacistTeam;
    Widget_Gameboard_Top* wWidgetTop;
    Widget_Gameboard_Bot* wWidgetBot;
    Widget_Gameboard_Right* wWidgetRight;
    Widget_Gameboard_Center* wWidgetCenter;
    Widget_Gameboard_Left* wWidgetLeft;

private:
    S_MESSAGE* mMSG;
    E_VOTE* mPlayerVotes;

public:
    Ui::Widget_Gameboard *ui;
};

#endif // WIDGET_GAMEBOARD_H
