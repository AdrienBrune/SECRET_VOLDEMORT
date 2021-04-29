#ifndef WIDGET_GAMEBOARD_RIGHT_H
#define WIDGET_GAMEBOARD_RIGHT_H

#include <QWidget>
#include <QPainter>
#include <QtMath>
#include <QTimer>
#include "widget_card.h"
#include "data.h"

namespace Ui {
class Widget_Right;
}

class Widget_Gameboard_Right : public QWidget
{
    Q_OBJECT

    typedef enum
    {
       none,
       blinking,
       fading

    }E_ANIMATION;

    typedef struct
    {
        E_ANIMATION state;
        E_VOTE vote;
        int step;
        QTimer* timer;

    }S_ANIMATION;

public:
    explicit Widget_Gameboard_Right(QWidget *parent = nullptr, S_MESSAGE* MSG = nullptr);
    ~Widget_Gameboard_Right();

signals:
    void sig_voted(E_VOTE);
    void sig_menu();

private slots:
    void Event_cardClicked(int);
    void hoverIn(int);
    void hoverOut(int);
    void animate();
    void onCheckRole();
    void onHideRole();

public:
    void initWidget();
    void updateWidget();
    void startVote();
    void endVote();

protected:
    void paintEvent(QPaintEvent *);

private:
    S_MESSAGE* mMSG;

    S_ANIMATION mAnimation;
    bool mHoverTop;
    bool mHoverBot;
    bool mHoverRole;
    QList<Widget_Card*> wCards;

    Widget_Card* mRoleArea;

public:
    Ui::Widget_Right *ui;

};

#endif // WIDGET_GAMEBOARD_RIGHT_H
