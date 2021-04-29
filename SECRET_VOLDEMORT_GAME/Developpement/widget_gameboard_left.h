#ifndef WIDGET_GAMEBOARD_LEFT_H
#define WIDGET_GAMEBOARD_LEFT_H

#include <QWidget>
#include <QPainter>
#include "widget_player.h"

namespace Ui {
class Widget_Gameboard_Left;
}

class Widget_Gameboard_Left : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_Gameboard_Left(QWidget* parent = nullptr, S_MESSAGE* MSG = nullptr, E_VOTE* playerVotes = nullptr);
    ~Widget_Gameboard_Left();

signals:
    void sig_clickOnPlayer(E_IDENTIFIER);

public:
    void attachPlayersToWidgets(S_MESSAGE* MSG);
    void setRevealVotes(bool toggle);
    void setPlayersClickable(bool toggle);
    void updateWidget();

protected:
    void paintEvent(QPaintEvent*);

private:
    S_MESSAGE* mMSG;
    E_VOTE* mPlayerVotes;
    QList<Widget_Player*> wPlayers;

public:
    Ui::Widget_Gameboard_Left *ui;
};

#endif // WIDGET_GAMEBOARD_LEFT_H
