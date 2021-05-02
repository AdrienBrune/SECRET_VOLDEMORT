#include "widget_gameboard_right.h"
#include "ui_widget_gameboard_right.h"
#include "data.h"

Widget_Gameboard_Right::Widget_Gameboard_Right(QWidget* parent, S_MESSAGE* MSG) :
    QWidget(parent),
    mMSG(MSG),
    mAnimation({E_ANIMATION::none, E_VOTE::blank, 0, nullptr}),
    mHoverTop(false),
    mHoverBot(false),
    mHoverRole(false),
    ui(new Ui::Widget_Right)
{
    ui->setupUi(this);

    // Setup vote cards.

    mAnimation.timer = new QTimer(this);
    connect(mAnimation.timer, SIGNAL(timeout()), this, SLOT(animate()));

    for(int i = 0; i < 2; i++)
    {
        Widget_Card* card = new Widget_Card(this, i);
        card->setStyleSheet("Widget_Card{background-color:rgba(0,0,0,0)}");
        connect(card, &Widget_Card::clicked, this, [i, this](bool){ this->Event_cardClicked(i); });
        connect(card, SIGNAL(sig_hoverIn(int)), this, SLOT(hoverIn(int)));
        connect(card, SIGNAL(sig_hoverOut(int)), this, SLOT(hoverOut(int)));

        wCards.append(card);
        ui->layout_vote->addWidget(card);
    }

    // Setup role card.

    mRoleArea = new Widget_Card(this);
    mRoleArea->setStyleSheet("Widget_Card{background-color:rgba(0,0,0,0);}");
    connect(mRoleArea, SIGNAL(sig_hoverIn(int)), this, SLOT(onCheckRole()));
    connect(mRoleArea, SIGNAL(sig_hoverOut(int)), this, SLOT(onHideRole()));
    ui->layoutRole->addWidget(mRoleArea);

    connect(ui->buttonMenu, SIGNAL(clicked()), this, SIGNAL(sig_menu()));
}

void Widget_Gameboard_Right::hoverIn(int id)
{
    if(id == 0)
    {
        mHoverTop = true;
    }
    else
    {
        mHoverBot = true;
    }

    update();
}

void Widget_Gameboard_Right::hoverOut(int id)
{
    if(id == 0)
    {
        mHoverTop = false;
    }
    else
    {
        mHoverBot = false;
    }

    update();
}

void Widget_Gameboard_Right::Event_cardClicked(int card)
{
    if(mAnimation.state != E_ANIMATION::blinking)
        return;

    if(card == 0)
    {
        mAnimation.vote = E_VOTE::lumos;
        emit sig_voted(E_VOTE::lumos);
    }
    else
    {
        mAnimation.vote = E_VOTE::nox;
        emit sig_voted(E_VOTE::nox);
    }

    mAnimation.state = E_ANIMATION::fading;
    mAnimation.step = 10;
    mAnimation.timer->stop();
    mAnimation.timer->start(120);

    update();
}

void Widget_Gameboard_Right::startVote()
{
    mAnimation.state = E_ANIMATION::blinking;
    mAnimation.step = 1;
    mAnimation.vote = E_VOTE::blank;

    mAnimation.timer->start(1000);
}

void Widget_Gameboard_Right::animate()
{
    switch(mAnimation.state)
    {
        case E_ANIMATION::blinking:
            if(mAnimation.step-- == 0)
                mAnimation.step = 1;
            break;

        case E_ANIMATION::fading:
            if(mAnimation.step-- == 0)
                endVote();
            break;

        default:
            break;
    }

    update();
}

void Widget_Gameboard_Right::onCheckRole()
{
    mHoverRole = true;
    update();
}

void Widget_Gameboard_Right::onHideRole()
{
    mHoverRole = false;
    update();
}

void Widget_Gameboard_Right::initWidget()
{

}

void Widget_Gameboard_Right::updateWidget()
{
    for(Widget_Card * card : wCards)
    {
        card->update();
    }
}

void Widget_Gameboard_Right::endVote()
{
    mAnimation.state = E_ANIMATION::none;
    mAnimation.step = 1;
    mAnimation.vote = E_VOTE::blank;
    mAnimation.timer->stop();

    update();
}

void Widget_Gameboard_Right::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(":/images/border_right.png")));

    if(mMSG->identifier == E_IDENTIFIER::ID_none)
        return;

    if(mMSG->gameStatus.endGame != E_END_GAME::notFinished)
        return;

    if(mMSG->gameStatus.players[mMSG->identifier].status == E_PLAYER_STATUS::notPlaying)
        return;

    // Draw Role card (ratio 1.5).

    QRect roleArea, roleAreaReduced;
    roleAreaReduced.setRect(mRoleArea->x() + 20, mRoleArea->y() + 30, mRoleArea->width() - 40, mRoleArea->height() - 60);
    if( mRoleArea->height() / mRoleArea->width() < 1.5)
    {
        // The size will scale on height.
        roleArea.setRect(roleAreaReduced.x() + (roleAreaReduced.width()-static_cast<int>(roleAreaReduced.height()/1.5))/2,
                         roleAreaReduced.y(),
                         static_cast<int>(roleAreaReduced.height()/1.5),
                         roleAreaReduced.height());
    }
    else
    {
        // The size will scale on width.
        roleArea.setRect(roleAreaReduced.x(),
                         roleAreaReduced.y() + (roleAreaReduced.height()-static_cast<int>(roleAreaReduced.width()*1.5))/2,
                         roleAreaReduced.width(),
                         static_cast<int>(roleAreaReduced.width()*1.5));
    }

    if(mHoverRole)
    {
        painter.drawPixmap(roleArea, QPixmap(QString(":/images/Role_%1.png").arg(g_RoleNames[mMSG->gameStatus.players[mMSG->identifier].roleName])));
    }
    else
    {
        roleArea.setRect(mRoleArea->x() + mRoleArea->width()/6,
                         mRoleArea->y() + mRoleArea->height() - (mRoleArea->width()*2/3)*0.43,
                         mRoleArea->width()*2/3,
                         (mRoleArea->width()*2/3)*0.43);
        painter.setOpacity(0.8);
        painter.drawPixmap(roleArea, QPixmap(QString(":/images/%1").arg(RES_SECRET_ROLE)));
        painter.setOpacity(1.0);
    }

    // Draw vote animation.

    if(mAnimation.state == E_ANIMATION::none)
        return;

    QRect area(wCards[0]->x() + 20, wCards[0]->y(), wCards[0]->width() - 40, wCards[0]->height() + wCards[1]->height());
    QRect areaTop(area.x(), area.y(), area.width(), area.height()/2);
    QRect areaBot(area.x(), area.y()+area.height()/2, area.width(), area.height()/2);
    QRect cardBigTop(areaTop.x() + areaTop.width()/10, areaTop.y() + areaTop.height()/3 - 20, areaTop.width()*8/10, areaTop.height()*2/3);
    QRect cardLittleTop(cardBigTop.x() + cardBigTop.width()/10, cardBigTop.y() + cardBigTop.height()/10, cardBigTop.width()*8/10, cardBigTop.height()*8/10);
    QRect cardBigBot(areaBot.x() + areaBot.width()/10, areaBot.y() + 20, areaBot.width()*8/10, areaBot.height()*2/3);
    QRect cardLittleBot(cardBigBot.x() + cardBigBot.width()/10, cardBigBot.y() + cardBigBot.height()/10, cardBigBot.width()*8/10, cardBigBot.height()*8/10);
    QRect cardTopShifted(cardBigTop.x() + cardBigTop.width()*9/10, cardBigTop.y(), cardBigTop.width(), cardBigTop.height());
    QRect cardBotShifted(cardBigBot.x() + cardBigBot.width()*9/10, cardBigBot.y(), cardBigBot.width(), cardBigBot.height());

    if(mAnimation.state == E_ANIMATION::blinking)
    {
        // Display pre-choice Yah.
        if(mHoverTop)
        {
            painter.drawPixmap(cardBigTop, QPixmap(QString(":/images/%1").arg(RES_VOTE_LUMOS)));
            painter.setOpacity(0.7);
            painter.drawPixmap(cardBotShifted, QPixmap(QString(":/images/%1").arg(RES_VOTE_NOX)));
        }

        // Display pre-choice Nein.
        if(mHoverBot)
        {
            painter.drawPixmap(cardBigBot, QPixmap(QString(":/images/%1").arg(RES_VOTE_NOX)));
            painter.setOpacity(0.7);
            painter.drawPixmap(cardTopShifted, QPixmap(QString(":/images/%1").arg(RES_VOTE_LUMOS)));
        }

        // Display blinking cards.
        if(!mHoverTop && !mHoverBot)
        {
            if(mAnimation.step == 1)
            {
                painter.drawPixmap(cardBigTop, QPixmap(QString(":/images/%1").arg(RES_VOTE_LUMOS)));
                painter.drawPixmap(cardBigBot, QPixmap(QString(":/images/%1").arg(RES_VOTE_NOX)));
            }
            else
            {
                painter.setOpacity(0.9);
                painter.drawPixmap(cardLittleTop, QPixmap(QString(":/images/%1").arg(RES_VOTE_LUMOS)));
                painter.drawPixmap(cardLittleBot, QPixmap(QString(":/images/%1").arg(RES_VOTE_NOX)));
            }
        }
    }
    else
    {
        double opacity = mAnimation.step/10.0;
        painter.setOpacity(opacity);
        switch(mAnimation.vote)
        {
            // Display Yah card fading.
            case E_VOTE::lumos:
                painter.drawPixmap(cardBigTop, QPixmap(QString(":/images/%1").arg(RES_VOTE_LUMOS)));
                break;

            // Display Nein card fading.
            case E_VOTE::nox:
                painter.drawPixmap(cardBigBot, QPixmap(QString(":/images/%1").arg(RES_VOTE_NOX)));
                break;

            default:
                break;
        }
    }
}

Widget_Gameboard_Right::~Widget_Gameboard_Right()
{
    delete ui;
}
