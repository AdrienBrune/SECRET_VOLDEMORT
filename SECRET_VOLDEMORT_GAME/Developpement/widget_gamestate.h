#ifndef WIDGET_GAME_STATE_H
#define WIDGET_GAME_STATE_H

#include <QWidget>
#include <QPainter>
#include <QFontMetrics>
#include "data.h"

namespace Ui {
class Widget_GameState;
}

class Widget_GameState : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_GameState(QWidget *parent = nullptr, S_MESSAGE* MSG = nullptr);
    ~Widget_GameState();

private:
    int getIndexTextFromCommand(int);

public:
    S_MESSAGE* mMSG;
    QStringList mStatesLabel;

protected:
    void paintEvent(QPaintEvent *);

private:
    Ui::Widget_GameState *ui;
};

#endif // WIDGET_GAME_STATE_H
