#ifndef WIDGET_GAMEBOARD_BOT_H
#define WIDGET_GAMEBOARD_BOT_H

#include <QWidget>
#include <QPainter>
#include <QLabel>
#include "data.h"
#include "widget_gamestate.h"

namespace Ui {
class Widget_Gameboard_Bot;
}

class Widget_Gameboard_Bot : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_Gameboard_Bot(QWidget *parent = nullptr, S_MESSAGE* MSG = nullptr);
    ~Widget_Gameboard_Bot();

public:
    void initWidget();
    void showRole();
    void hideRole();
    void updateWidget();

protected:
    void paintEvent(QPaintEvent *);

public:
    Widget_GameState * wGameState;

private:
    S_MESSAGE* mMSG;

public:
    Ui::Widget_Gameboard_Bot *ui;
};

#endif // WIDGET_GAMEBOARD_BOT_H
