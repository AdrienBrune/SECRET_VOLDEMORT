#include "widget_gameboard.h"
#include "ui_widget_gameboard.h"

Widget_Gameboard::Widget_Gameboard(QWidget* parent, S_MESSAGE* MSG, E_VOTE* playerVotes):
    QWidget(parent),
    mMSG(MSG),
    mPlayerVotes(playerVotes),
    ui(new Ui::Widget_Gameboard)
{
    ui->setupUi(this);

    QCursor cursor(QPixmap(":/images/custom_cursor.png"), 20, 20);
    setCursor(cursor);

    wWidgetTop = new Widget_Gameboard_Top(this, mMSG);
    wWidgetBot = new Widget_Gameboard_Bot(this, mMSG);
    wWidgetRight = new Widget_Gameboard_Right(this, mMSG);
    wWidgetCenter = new Widget_Gameboard_Center(this, mMSG);
    wWidgetLeft = new Widget_Gameboard_Left(this, mMSG, mPlayerVotes);

    wFacistTeam = new Widget_FacistTeam(this, mMSG);
    wFacistTeam->hide();

    ui->layout_top->addWidget(wWidgetTop);
    ui->layout_right->addWidget(wWidgetRight);
    ui->layout_bot->addWidget(wWidgetBot);
    ui->layout_center->addWidget(wWidgetCenter);
    ui->layout_left->addWidget(wWidgetLeft);

    connect(wWidgetRight, SIGNAL(sig_voted(E_VOTE)), this, SIGNAL(sig_voted(E_VOTE)));
    connect(wWidgetRight, SIGNAL(sig_menu()), this, SIGNAL(sig_menu()));
    connect(wWidgetLeft, SIGNAL(sig_clickOnPlayer(E_IDENTIFIER)), this , SIGNAL(sig_clickOnPlayer(E_IDENTIFIER)));
    connect(wWidgetCenter, SIGNAL(sig_draw()), this, SIGNAL(sig_draw()));
    connect(wFacistTeam, SIGNAL(sig_hideFacistTeam()), this, SLOT(onHideFacistTeam()));

    wWidgetBot->initWidget();
}

Widget_Gameboard::~Widget_Gameboard()
{
    delete ui;
}

void Widget_Gameboard::onHideFacistTeam()
{
    // Remove old screen.
    QLayoutItem * old = ui->layout_bot->itemAt(0);
    ui->layout_bot->removeItem(old);
    old->widget()->hide();

    // Display new screen.
    ui->layout_bot->addWidget(wWidgetBot);
    wWidgetBot->show();
}

void Widget_Gameboard::switchCurrentScreenWith(QWidget * screen)
{
    // Remove old screen.
    QLayoutItem * old = ui->layout_center->itemAt(0);
    ui->layout_center->removeItem(old);
    old->widget()->hide();

    // Display new screen.
    ui->layout_center->addWidget(screen);
    screen->show();
}

void Widget_Gameboard::displayFacistTeam()
{
    // Do not show the message if player is liberal.
    if(mMSG->gameStatus.players[mMSG->identifier].role == E_ROLE::phenixOrder)
    {
        // Remove old screen.
        QLayoutItem * old = ui->layout_bot->itemAt(0);
        ui->layout_bot->removeItem(old);
        old->widget()->hide();

        // Display widget bot.
        ui->layout_bot->addWidget(wWidgetBot);
        wWidgetBot->show();
        return;
    }

    // Do not show the message if player is Hitler and number of player is upper than 6.
    if(mMSG->gameStatus.players[mMSG->identifier].role == E_ROLE::voldemort)
    {
        if(mMSG->gameStatus.players.size() > 6)
        {
            // Remove old screen.
            QLayoutItem * old = ui->layout_bot->itemAt(0);
            ui->layout_bot->removeItem(old);
            old->widget()->hide();

            // Display widget bot.
            ui->layout_bot->addWidget(wWidgetBot);
            wWidgetBot->show();
            return;
        }
    }

    // Remove old screen.
    QLayoutItem * old = ui->layout_bot->itemAt(0);
    ui->layout_bot->removeItem(old);
    old->widget()->hide();

    // Display new screen.
    ui->layout_bot->addWidget(wFacistTeam);
    wFacistTeam->show();
}

void Widget_Gameboard::revealVotes(bool toggle)
{
    wWidgetLeft->setRevealVotes(toggle);
}

void Widget_Gameboard::enableClickOnPlayers(bool toggle)
{
    wWidgetLeft->setPlayersClickable(toggle);
}

void Widget_Gameboard::enableVote(bool toggle)
{
    if(toggle)
    {
        if(mMSG->gameStatus.players[mMSG->identifier].status != E_PLAYER_STATUS::alive)
            return;

        wWidgetRight->startVote();
    }
    else
    {
        wWidgetRight->endVote();
    }
}

void Widget_Gameboard::enableDraw(bool toggle)
{
    if(toggle)
    {
        wWidgetCenter->startPileAnimation();
    }
    else
    {
        wWidgetCenter->stopPileAnimation();
    }
}

void Widget_Gameboard::updateBoard()
{
    wWidgetRight->initWidget();
    wWidgetLeft->attachPlayersToWidgets(mMSG);
    wWidgetBot->initWidget();

    wWidgetCenter->updateWidget();
    wWidgetLeft->updateWidget();
    wWidgetBot->updateWidget();
    wWidgetRight->updateWidget();
    wWidgetTop->updateWidget();
}
