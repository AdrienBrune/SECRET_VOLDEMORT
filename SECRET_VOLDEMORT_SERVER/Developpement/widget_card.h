#ifndef WIDGET_CARD_H
#define WIDGET_CARD_H

#include <QWidget>
#include <QPainter>
#include <QEvent>
#include "types.h"

namespace Ui {
class Widget_Card;
}

class Widget_Card : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_Card(QWidget *parent = nullptr, E_CARD card = E_CARD::deathEatersLaw, bool* antispoil = nullptr);
    ~Widget_Card();

public:
    void setCard(E_CARD card);
    E_CARD getCard();

protected:
    void paintEvent(QPaintEvent*);
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    E_CARD mCard;
    bool* mAntiSpoil;
    bool mHover;

private:
    Ui::Widget_Card *ui;
};

#endif // WIDGET_CARD_H
