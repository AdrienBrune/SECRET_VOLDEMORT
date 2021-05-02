#include "widget_screenendgame.h"

Widget_ScreenEndGame::Widget_ScreenEndGame(QWidget* parent, S_MESSAGE* MSG) :
    QWidget(parent),
    mMSG(MSG)
{

}

Widget_ScreenEndGame::~Widget_ScreenEndGame()
{

}

void Widget_ScreenEndGame::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    switch(mMSG->gameStatus.endGame)
    {
        case E_END_GAME::deathEatersWon:
            painter.drawPixmap(QRect(0, 0, width(), height()), QPixmap(":/images/end_screen_deatheaters_won.png"));
            break;

        case E_END_GAME::voldemortElected:
            painter.drawPixmap(QRect(0, 0, width(), height()), QPixmap(":/images/end_screen_voldemort_elu.png"));
            break;

        case E_END_GAME::voldemortKilled:
            painter.drawPixmap(QRect(0, 0, width(), height()), QPixmap(":/images/end_screen_voldemort_killed.png"));
            break;

        case E_END_GAME::phenixOrderWon:
            painter.drawPixmap(QRect(0, 0, width(), height()), QPixmap(":/images/end_screen_phenixorder_won.png"));
            break;

        default:
            break;
    }
}
