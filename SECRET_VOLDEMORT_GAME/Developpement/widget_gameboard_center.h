#ifndef WIDGET_GAMEBOARD_CENTER_H
#define WIDGET_GAMEBOARD_CENTER_H

#include <QWidget>
#include <QPainter>
#include <QTimer>
#include "widget_boardphenixorder.h"
#include "widget_boarddeatheaters.h"
#include "widget_card.h"
#include "data.h"

namespace Ui {
class Widget_Gameboard_Center;
}

class Widget_Gameboard_Center : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_Gameboard_Center(QWidget *parent = nullptr, S_MESSAGE* MSG = nullptr);
    ~Widget_Gameboard_Center();

signals:
    void sig_draw();

private slots:
    void hoverIn(int);
    void hoverOut(int);
    void animate();
    void onDraw();

public:
    void initWidget();
    void updateWidget();
    void startPileAnimation();
    void stopPileAnimation();

protected:
    void paintEvent(QPaintEvent *);

public:
    Widget_BoardDeathEaters* wFacisteBoard;
    Widget_BoardPhenixOrder* wLiberalBoard;
    Widget_Card* wPile;

private:
    S_MESSAGE* mMSG;

private:
    bool mIsPileAnimated;
    bool mHover;
    int mPileAnimationStep;
    QTimer * mTimer;

public:
    Ui::Widget_Gameboard_Center *ui;
};

#endif // WIDGET_GAMEBOARD_CENTER_H
