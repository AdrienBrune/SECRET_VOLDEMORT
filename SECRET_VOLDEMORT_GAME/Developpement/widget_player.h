#ifndef WIDGET_PLAYER_H
#define WIDGET_PLAYER_H

#include <QWidget>
#include <QEvent>
#include <QHoverEvent>
#include <QPainter>
#include <QPushButton>
#include "data.h"

namespace Ui {
class Widget_Player;
}

class Widget_Player : public QPushButton
{
    Q_OBJECT

public:
    explicit Widget_Player(QWidget *parent = nullptr, S_MESSAGE* MSG = nullptr, E_VOTE* playerVotes = nullptr);
    ~Widget_Player();

signals:
    void sig_clickOnPlayer(E_IDENTIFIER);

private slots:
    void clickOnPlayer();

public:
    void setPlayer(E_IDENTIFIER identifier);
    void setClickable(bool toggle);
    void setVotesReveal(bool toggle);

protected:
    void paintEvent(QPaintEvent*);
    void leaveEvent(QEvent * event);
    void enterEvent(QEvent * event);
    void hoverMove(QHoverEvent* event);

private:
    S_MESSAGE* mMSG;
    E_VOTE* mPlayerVotes;
    E_IDENTIFIER mIdentifier;
    bool mPlayerClickable;
    bool mVotesReveal;
    bool mHover;

public:
    Ui::Widget_Player *ui;
};

#endif // WIDGET_PLAYER_H
