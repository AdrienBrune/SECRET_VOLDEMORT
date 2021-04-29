#include "widget_gameboard_bot.h"
#include "ui_widget_gameboard_bot.h"

Widget_Gameboard_Bot::Widget_Gameboard_Bot(QWidget* parent, S_MESSAGE* MSG):
    QWidget(parent),
    mMSG(MSG),
    ui(new Ui::Widget_Gameboard_Bot)
{
    ui->setupUi(this);

    wGameState = new Widget_GameState(this, mMSG);
    ui->layout_game_state->addWidget(wGameState);

    ui->role->hide();
}

Widget_Gameboard_Bot::~Widget_Gameboard_Bot()
{
    delete ui;
}

void Widget_Gameboard_Bot::initWidget()
{
    ui->role->hide();

    if(mMSG->gameStatus.players.size() == 0)
        return;

    switch(mMSG->gameStatus.players[mMSG->identifier].electionRole)
    {
        case E_ELECTION_ROLE::minister:
            ui->role->setStyleSheet(QString("QLabel{border-image:url(:/images/%1);}").arg(RES_MINISTER));
            ui->role->show();
            break;

        case E_ELECTION_ROLE::director:
            ui->role->setStyleSheet(QString("QLabel{border-image:url(:/images/%1);}").arg(RES_DIRECTOR));
            ui->role->show();
            break;

        default:
            break;
    }
}

void Widget_Gameboard_Bot::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(":/images/border_bot.png")));
}

void Widget_Gameboard_Bot::hideRole()
{
    ui->role->hide();
}

void Widget_Gameboard_Bot::updateWidget()
{
    wGameState->update();
}

void Widget_Gameboard_Bot::showRole()
{
    ui->role->show();
}


