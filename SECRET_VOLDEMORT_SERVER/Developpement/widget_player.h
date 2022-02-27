#ifndef WIDGET_PLAYER_H
#define WIDGET_PLAYER_H

#include <QWidget>
#include <QPainter>

#include "types.h"

namespace Ui {
class Widget_Player;
}

class Widget_Player : public QWidget
{
    Q_OBJECT

public:
    explicit Widget_Player(QWidget *parent = nullptr,
                           S_PLAYER player = { E_PLAYER_STATUS::notPlaying,
                                              nullptr,
                                              E_IDENTIFIER::ID_none,
                                              "",
                                              E_ROLE::notAssigned,
                                              E_ROLE_NAME::noOne,
                                              E_ELECTION_ROLE::none,
                                              E_VOTE::blank,
                                              E_POWER::noPower },
                           S_GAME_STATUS* game = nullptr,
                           bool* antiSpoil = nullptr,
                           bool* expertMode = nullptr);
    ~Widget_Player();

public:
    void initWidget();
    void setPlayer(S_PLAYER player);
    S_PLAYER getPlayer();
    void applyPlayerInputs();

protected:
    void paintEvent(QPaintEvent*);

private:
    S_PLAYER mPlayer;
    S_GAME_STATUS* mGame;
    bool* mAntiSpoil;
    bool* mExpertMode;

private:
    Ui::Widget_Player *ui;
};

#endif // WIDGET_PLAYER_H
