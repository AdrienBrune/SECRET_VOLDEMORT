#ifndef WIDGET_GAMEBOARD_TOP_H
#define WIDGET_GAMEBOARD_TOP_H

#include <QWidget>
#include <QPainter>
#include "data.h"

namespace Ui {
class Widget_Gameboard_Top;
}

class Widget_Gameboard_Top : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_Gameboard_Top(QWidget* parent = nullptr, S_MESSAGE* MSG = nullptr);
    ~Widget_Gameboard_Top();

private:
    quint8 getIndexMessageCorrespondingToSituation();
    quint8 getIndexMessagePresident();
    quint8 getIndexMessageChancelor();
    quint8 getIndexMessageNoRole();

protected:
    void paintEvent(QPaintEvent*);

public:
    void updateWidget();
    void setGameMessage(quint8);

private:
    S_MESSAGE* mMSG;
    quint8 mState;
    QString mMessages[20];

public:
    Ui::Widget_Gameboard_Top *ui;
};

#endif // WIDGET_GAMEBOARD_TOP_H
