#include "widget_player.h"
#include <QDebug>

Widget_Player::Widget_Player(QWidget *parent, S_MESSAGE* MSG, E_VOTE* playerVotes):
    QPushButton(parent),
    mMSG(MSG),
    mPlayerVotes(playerVotes),
    mIdentifier(E_IDENTIFIER::ID_none),
    mPlayerClickable(false),
    mVotesReveal(false),
    mHover(false)
{
    this->setAttribute(Qt::WA_Hover, true);
    connect(this, SIGNAL(clicked()), this, SLOT(clickOnPlayer()));

    setMinimumHeight(80);
}

Widget_Player::~Widget_Player()
{

}

void Widget_Player::setPlayer(E_IDENTIFIER identifier)
{
    mIdentifier = identifier;
}

void Widget_Player::setClickable(bool toggle)
{
    mPlayerClickable = toggle;
}

void Widget_Player::setVotesReveal(bool toggle)
{
    mVotesReveal = toggle;
}

void Widget_Player::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

#define PEN_WIDTH   4

    // Focus pen.
    QPen penFocus(QColor(255, 222, 92), PEN_WIDTH);

    // Minister brush.
    QBrush brushMinister(QColor(220, 220, 220));
    // Minister Pen.
    QPen penMinister(QColor(160, 120, 70), PEN_WIDTH);

    // Director brush.
    QBrush brushDirector(QColor(115, 115, 115));
    // Director pen.
    QPen penDirector(QColor(50, 50, 50), PEN_WIDTH);

    // Player brush.
    QBrush brushPlayer(QColor(0, 80, 60));
    // Player pen.
    QPen penPlayer(QColor(10, 120, 80), PEN_WIDTH);


    // Voldemort brush.
    QBrush brushVoldemort(QColor(28, 63, 56));
    // Voldemort pen.
    QPen penVoldemort(QColor(70, 130, 115), PEN_WIDTH);

    // Death eater brush.
    QBrush brushDeathEater(QColor(89, 53, 119));
    // Death eater pen.
    QPen penDeathEater(QColor(112, 74, 137), PEN_WIDTH);

    // Phenix order brush.
    QBrush brushPhenixOrder(QColor(143, 20, 31));
    // Phenix order pen.
    QPen penPhenixOrder(QColor(153, 64, 55), PEN_WIDTH);


    // Unconnected player brush.
    QBrush brushUnconnected(QColor(71, 48, 45));
    // Unconnected player pen.
    QPen penUnconnected(QColor(98, 68, 65), PEN_WIDTH);

    // Minister brush.
    QBrush brushDead(QColor(30, 30, 30));
    // Minister Pen.
    QPen penDead(QColor(60, 60, 60), PEN_WIDTH);


    // White text pen.
    QPen penWhite(QColor(220, 220, 220), PEN_WIDTH);
    // Black text pen.
    QPen penBlack(QColor(40, 40, 40), PEN_WIDTH);


    // Get box size.
    QRect box;
    // Dead player is not focusable
    if(mMSG->gameStatus.players[mIdentifier].status == E_PLAYER_STATUS::alive)
    {
        // Identify Director vote stage or other selection stage.
        if(mMSG->command == CMD_TO_PLAYER_ELECT_DIRECTOR)
        {
            // Check to grow the box while hover.
            if(mHover && mPlayerClickable && mMSG->identifier != mIdentifier)
            {
                // Do not grow the box if the player focus is a former Minister/Director.
                if(mMSG->gameStatus.players[mIdentifier].electionRole == E_ELECTION_ROLE::viceDirector
                    || mMSG->gameStatus.players[mIdentifier].electionRole == E_ELECTION_ROLE::viceMinister)
                {
                    box = QRect(width()/20, height()/20, width()*9/10, height()*9/10);
                }
                else
                {
                    box = QRect(0, 0, width(), height());
                }
            }
            else
            {
                box = QRect(width()/20, height()/20, width()*9/10, height()*9/10);
            }
        }
        else
        {
            if(mHover && mPlayerClickable && mMSG->identifier != mIdentifier)
            {
                box = QRect(0, 0, width(), height());
            }
            else
            {
                box = QRect(width()/20, height()/20, width()*9/10, height()*9/10);
            }
        }
    }
    else
    {
        box = QRect(width()/20, height()/20, width()*9/10, height()*9/10);
    }

    // Get label areas.
    QRect labelName, labelRole;
    labelName = QRect(box.x(), 0, box.width(), box.height()*2/3);
    labelRole = QRect(box.x(), box.height()*2/3, box.width(), box.height()/3);

    // Out of game boxes behaviour.
    if(mMSG->gameStatus.endGame != E_END_GAME::notFinished)
    {
        // draw box and texts.
        painter.setOpacity(0.8);
        painter.setPen(penWhite);
        switch(mMSG->gameStatus.players[mIdentifier].role)
        {
            case E_ROLE::deathEaters:
                painter.setBrush(brushDeathEater);
                painter.setPen(penDeathEater);
                painter.drawRect(box);
                // draw text.
                painter.setOpacity(1);
                painter.setFont(QFont("Harry Potter", 20));
                painter.drawText(labelName, Qt::AlignCenter, mMSG->gameStatus.players[mIdentifier].name);
                painter.setFont(QFont("Times New Roman", 8));
                painter.drawText(labelRole, Qt::AlignCenter, "Mangemort");
                break;

            case E_ROLE::phenixOrder:
                painter.setBrush(brushPhenixOrder);
                painter.setPen(penPhenixOrder);
                painter.drawRect(box);
                // draw text.
                painter.setOpacity(1);
                painter.setFont(QFont("Harry Potter", 20));
                painter.drawText(labelName, Qt::AlignCenter, mMSG->gameStatus.players[mIdentifier].name);
                painter.setFont(QFont("Times New Roman", 8));
                painter.drawText(labelRole, Qt::AlignCenter, "Ordre du Phénix");
                break;

            case E_ROLE::voldemort:
                painter.setBrush(brushVoldemort);
                painter.setPen(penVoldemort);
                painter.drawRect(box);
                // draw text.
                painter.setOpacity(1);
                painter.setFont(QFont("Harry Potter", 20));
                painter.drawText(labelName, Qt::AlignCenter, mMSG->gameStatus.players[mIdentifier].name);
                painter.setFont(QFont("Times New Roman", 8));
                painter.drawText(labelRole, Qt::AlignCenter, "Voldemort");
                break;

            case E_ROLE::notAssigned:
                painter.setBrush(brushUnconnected);
                painter.setPen(penUnconnected);
                painter.drawRect(box);
                // draw texts.
                painter.setOpacity(1);
                painter.setFont(QFont("Harry Potter", 20));
                painter.drawText(labelName, Qt::AlignCenter, mMSG->gameStatus.players[mIdentifier].name);
                painter.setFont(QFont("Times New Roman", 8));
                painter.drawText(labelRole, Qt::AlignCenter, "En attente");
                break;
        }

        return;
    }

    // Game box behaviour.

    // draw box
    painter.setOpacity(0.8);
    painter.setPen(penBlack);
    switch(mMSG->gameStatus.players[mIdentifier].status)
    {
        case E_PLAYER_STATUS::alive:
            switch(mMSG->gameStatus.players[mIdentifier].electionRole)
            {
                case E_ELECTION_ROLE::minister:
                    painter.setBrush(brushMinister);
                    painter.setPen(penMinister);
                    painter.drawRect(box);
                    break;

                case E_ELECTION_ROLE::director:
                    painter.setBrush(brushDirector);
                    painter.setPen(penDirector);
                    painter.drawRect(box);
                    break;

                default:
                    painter.setBrush(brushPlayer);
                    painter.setPen(penPlayer);
                    painter.drawRect(box);
                    break;
            }
            break;

        case E_PLAYER_STATUS::dead:
            painter.setBrush(brushDead);
            painter.setPen(penDead);
            painter.drawRect(box);
            break;

        case E_PLAYER_STATUS::notPlaying:
            painter.setBrush(brushUnconnected);
            painter.setPen(penUnconnected);
            painter.drawRect(box);
            break;
    }

    // Draw border to show focus on player.
    if(mMSG->gameStatus.playerFocus != E_IDENTIFIER::ID_none)
    {
        if(mMSG->gameStatus.playerFocus == mIdentifier)
        {
            painter.setOpacity(1);
            painter.setBrush(QBrush());
            painter.setPen(penFocus);
            painter.drawRect(box);
        }
    }

    // draw texts.
    painter.setOpacity(1);
    painter.setFont(QFont("Times New Roman", 9));
    switch(mMSG->gameStatus.players[mIdentifier].status)
    {
        case E_PLAYER_STATUS::alive:
            switch(mMSG->gameStatus.players[mIdentifier].electionRole)
            {
                case E_ELECTION_ROLE::minister:
                    painter.setPen(penMinister);
                    painter.drawText(labelRole, Qt::AlignCenter, "Ministre de la magie");
                    break;

                case E_ELECTION_ROLE::director:
                    painter.setPen(penDirector);
                    painter.drawText(labelRole, Qt::AlignCenter, "Directeur de Poudlard");
                    break;

                case E_ELECTION_ROLE::viceMinister:
                    painter.setPen(penPlayer);
                    painter.drawText(labelRole, Qt::AlignCenter, "Vice Ministre");
                    break;

                case E_ELECTION_ROLE::viceDirector:
                    painter.setPen(penPlayer);
                    painter.drawText(labelRole, Qt::AlignCenter, "Vice Directeur");
                    break;

                default:
                    painter.setPen(penPlayer);
                    break;
            }
            break;

        case E_PLAYER_STATUS::dead:
            painter.setPen(penDead);
            painter.drawText(labelRole, Qt::AlignCenter, "Mort");
            break;

        case E_PLAYER_STATUS::notPlaying:
            painter.setPen(penUnconnected);
            painter.drawText(labelRole, Qt::AlignCenter, "En attente");
            break;
    }
    painter.setFont(QFont("Harry Potter", 20));
    painter.drawText(labelName, Qt::AlignCenter, mMSG->gameStatus.players[mIdentifier].name);

    // draw vote.

    if(mVotesReveal)
    {
        // Player needs to be alive.
        if(mMSG->gameStatus.players[mIdentifier].status == E_PLAYER_STATUS::alive)
        {
            QBrush brushGreen(QColor(0x7A, 0xCB, 0x8A));
            QBrush brushRed(QColor(0xA0, 0x33, 0x33));
            painter.setPen(QPen());

            if(mPlayerVotes[mIdentifier] == E_VOTE::lumos)
                painter.setBrush(brushGreen);

            if(mPlayerVotes[mIdentifier] == E_VOTE::nox)
                painter.setBrush(brushRed);

            painter.drawRect(box.x() + box.width()*9/10, box.y(), box.width()/10, box.height());
        }
    }
    else
    {
        if(mMSG->gameStatus.players[mIdentifier].vote == E_VOTE::nox || mMSG->gameStatus.players[mIdentifier].vote == E_VOTE::lumos)
        {
            painter.drawPixmap(QRect(box.x() + box.width() - box.height()/2 - 4, box.y() + box.height()/2 - 4,
                                     box.height()/2, box.height()/2),
                               QPixmap(":/images/vote_ok.png"));
        }
    }

    // draw power.
    if(mMSG->gameStatus.players[mMSG->identifier].electionRole == E_ELECTION_ROLE::minister
       && mMSG->gameStatus.players[mIdentifier].status == E_PLAYER_STATUS::alive
       && mMSG->identifier != mIdentifier
       && mHover)
    {
        painter.setOpacity(1);
        switch(mMSG->gameStatus.players[mMSG->identifier].power)
        {
            case E_POWER::chooseMinister:
                painter.drawPixmap(QRect(box.x(), box.y(), box.height(), box.height()),
                                   QPixmap(":/images/player_select.png"));
                break;

            case E_POWER::kill:
                painter.drawPixmap(QRect(box.x(), box.y(), box.height(), box.height()),
                                   QPixmap(":/images/player_kill.png"));
                break;

            case E_POWER::checkRole:
                painter.drawPixmap(QRect(box.x(), box.y(), box.height(), box.height()),
                                   QPixmap(":/images/player_see.png"));
                break;

            default:
                break;
        }
    }
}

void Widget_Player::clickOnPlayer()
{
    // Check if players are clickable.
    if(mPlayerClickable)
    {
        // Do not allow self click.
        if(mMSG->identifier == mIdentifier)
        {
            return;
        }

        // Do not allow player not alive or not playing.
        if(mMSG->gameStatus.players[mIdentifier].status != E_PLAYER_STATUS::alive)
        {
            return;
        }

        // In vote stage, some nomination are not allowed.
        if(mMSG->command == CMD_TO_PLAYER_ELECT_DIRECTOR
            && (mMSG->gameStatus.players[mIdentifier].electionRole == E_ELECTION_ROLE::viceMinister
                || mMSG->gameStatus.players[mIdentifier].electionRole == E_ELECTION_ROLE::viceDirector))
        {
            return;
        }

        emit sig_clickOnPlayer(mIdentifier);
    }
}

void Widget_Player::leaveEvent(QEvent*)
{
    mHover = false;
}

void Widget_Player::enterEvent(QEvent*)
{
    mHover = true;
}
