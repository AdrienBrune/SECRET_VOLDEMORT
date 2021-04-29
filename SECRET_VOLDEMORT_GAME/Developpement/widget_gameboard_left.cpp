#include "widget_gameboard_left.h"
#include "ui_widget_gameboard_left.h"

Widget_Gameboard_Left::Widget_Gameboard_Left(QWidget* parent, S_MESSAGE* MSG, E_VOTE* playerVotes):
    QWidget(parent),
    mMSG(MSG),
    mPlayerVotes(playerVotes),
    ui(new Ui::Widget_Gameboard_Left)
{
    ui->setupUi(this);

    for(int i = 0; i < 10; i++)
    {
        Widget_Player* player = new Widget_Player(this, mMSG, mPlayerVotes);
        ui->layout_players->addWidget(player, 0, Qt::AlignTop);
        player->hide();

        wPlayers.append(player);

        connect(player, SIGNAL(sig_clickOnPlayer(E_IDENTIFIER)), this , SIGNAL(sig_clickOnPlayer(E_IDENTIFIER)));
    }
}

Widget_Gameboard_Left::~Widget_Gameboard_Left()
{
    delete ui;
}

void Widget_Gameboard_Left::attachPlayersToWidgets(S_MESSAGE* MSG)
{
    // Initialise all widgets.
    for(Widget_Player * widget : wPlayers)
    {
        widget->setPlayer(E_IDENTIFIER::ID_none);
        widget->hide();
    }

    // Attach identifier to widgets.
    for(int i = 0; i < MSG->gameStatus.players.size(); i++)
    {
        wPlayers[i]->setPlayer(static_cast<E_IDENTIFIER>(i));
        wPlayers[i]->show();
    }

    // Update.
    for(Widget_Player * widget : wPlayers)
    {
        widget->update();
    }
}

void Widget_Gameboard_Left::setRevealVotes(bool toggle)
{
    for(Widget_Player* wPlayer : wPlayers)
    {
        wPlayer->setVotesReveal(toggle);
    }
}

void Widget_Gameboard_Left::setPlayersClickable(bool toggle)
{
    for(Widget_Player* wPlayer : wPlayers)
    {
        wPlayer->setClickable(toggle);
    }
}

void Widget_Gameboard_Left::updateWidget()
{
    for(Widget_Player * player : wPlayers)
    {
        player->update();
    }
}

void Widget_Gameboard_Left::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(":/images/border_left.png")));
}


