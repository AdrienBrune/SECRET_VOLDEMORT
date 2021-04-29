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

/*
    if(mMSG->gameStatus.players[mIdentifier].status == E_PLAYER_STATUS::notPlaying)
        return;
*/

    // Black pen.
    QPen penBlack(QBrush(QColor(0x2E,0x2E,0x2E)), 1);

    // Grey brush.
    QBrush brushGrey(QColor(0x57,0x57,0x57));

    // White brush.
    QBrush brushWhite(QColor(0xFF,0xFF,0xFF));

    // Black brush.
    QBrush brushBlack(QColor(0x00,0x00,0x00));

    // Dark red brush.
    QBrush brushDarkRed(QColor(0x88,0x00,0x00));

    // Dark green brush.
    QBrush brushDarkGreen(QColor(0x00,0x88,0x66));

    // Get box size.
    QRect box;
    // Dead player is not focusable
    if(mMSG->gameStatus.players[mIdentifier].status == E_PLAYER_STATUS::alive)
    {
        // Identifie Chancelor vote stage or other selection stage.
        if(mMSG->command == CMD_TO_PLAYER_ELECT_CHANCELOR)
        {
            // Check to grow the box while hover.
            if(mHover && mPlayerClickable && mMSG->identifier != mIdentifier)
            {
                // Do not grow the box if the player focus is a former President/Chancelor.
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
        QBrush brushFaciste(QColor(0xCA,0x00,0x00));
        QBrush brushLiberal(QColor(0x25,0x5B,0xCA));
        QBrush brushHitler(QColor(0x00,0x00,0x00));
        QBrush brushNotAssigned(QColor(0xB6,0xB6,0xB6));

        // draw box and texts.
        painter.setOpacity(0.8);
        painter.setPen(penBlack);
        switch(mMSG->gameStatus.players[mIdentifier].role)
        {
            case E_ROLE::deathEaters:
                painter.setBrush(brushFaciste);
                painter.drawRect(box);
                // draw text.
                painter.setOpacity(1);
                painter.setFont(QFont("Germania", 16));
                painter.drawText(labelName, Qt::AlignCenter, mMSG->gameStatus.players[mIdentifier].name);
                painter.setFont(QFont("Times New Roman", 8));
                painter.drawText(labelRole, Qt::AlignCenter, "Faciste");
                break;

            case E_ROLE::phenixOrder:
                painter.setBrush(brushLiberal);
                painter.drawRect(box);
                // draw text.
                painter.setOpacity(1);
                painter.setFont(QFont("Germania", 16));
                painter.drawText(labelName, Qt::AlignCenter, mMSG->gameStatus.players[mIdentifier].name);
                painter.setFont(QFont("Times New Roman", 8));
                painter.drawText(labelRole, Qt::AlignCenter, "Libéral");
                break;

            case E_ROLE::voldemort:
                painter.setBrush(brushHitler);
                painter.drawRect(box);
                // draw text.
                painter.setOpacity(1);
                painter.setFont(QFont("Germania", 16));
                painter.drawText(labelName, Qt::AlignCenter, mMSG->gameStatus.players[mIdentifier].name);
                painter.setFont(QFont("Times New Roman", 8));
                painter.drawText(labelRole, Qt::AlignCenter, "Hitler");
                break;

            case E_ROLE::notAssigned:
                painter.setBrush(brushDarkGreen);
                painter.drawRect(box);
                // draw texts.
                painter.setOpacity(1);
                painter.setFont(QFont("Germania", 16));
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
                    painter.setBrush(brushWhite);
                    painter.drawRect(box);
                    break;

                case E_ELECTION_ROLE::director:
                    painter.setBrush(brushBlack);
                    painter.drawRect(box);
                    break;

                default:
                    painter.setBrush(brushGrey);
                    painter.drawRect(box);
                    break;
            }
            break;

        case E_PLAYER_STATUS::dead:
            painter.setBrush(brushDarkRed);
            painter.drawRect(box);
            break;

        case E_PLAYER_STATUS::notPlaying:
            painter.setBrush(brushDarkGreen);
            painter.drawRect(box);
            break;
    }

    // Draw border to show focus on player.
    if(mMSG->gameStatus.playerFocus != E_IDENTIFIER::ID_none)
    {
        if(mMSG->gameStatus.playerFocus == mIdentifier)
        {
            painter.setPen(QPen(QBrush(QColor(0xFF,0xB8,0x3C)), 4));
            painter.setBrush(QBrush());
            painter.drawRect(box);
            painter.setPen(penBlack);
        }
    }

    // draw texts.
    painter.setOpacity(1);
    painter.setFont(QFont("Germania", 16));
    painter.drawText(labelName, Qt::AlignCenter, mMSG->gameStatus.players[mIdentifier].name);
    painter.setFont(QFont("Times New Roman", 8));
    switch(mMSG->gameStatus.players[mIdentifier].status)
    {
        case E_PLAYER_STATUS::alive:
            switch(mMSG->gameStatus.players[mIdentifier].electionRole)
            {
                case E_ELECTION_ROLE::minister:
                    painter.drawText(labelRole, Qt::AlignCenter, "Président");
                    break;

                case E_ELECTION_ROLE::director:
                    painter.drawText(labelRole, Qt::AlignCenter, "Chancelier");
                    break;

                case E_ELECTION_ROLE::viceMinister:
                    painter.drawText(labelRole, Qt::AlignCenter, "Vice Président");
                    break;

                case E_ELECTION_ROLE::viceDirector:
                    painter.drawText(labelRole, Qt::AlignCenter, "Vice Chancelier");
                    break;

                default:
                    break;
            }
            break;

        case E_PLAYER_STATUS::dead:
            painter.drawText(labelRole, Qt::AlignCenter, "Mort");
            break;

        case E_PLAYER_STATUS::notPlaying:
            painter.drawText(labelRole, Qt::AlignCenter, "En attente");
            break;
    }

    // draw vote.

    if(mVotesReveal)
    {
        // Player needs to be alive.
        if(mMSG->gameStatus.players[mIdentifier].status == E_PLAYER_STATUS::alive)
        {
            QBrush brushGreen(QColor(0x7A,0xCB,0x8A));
            QBrush brushRed(QColor(0xC2,0x33,0x33));

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

    // The Président still have the power while voting.
    if(mMSG->command == CMD_TO_PLAYER_VOTE_TO_KILL || mMSG->command == CMD_TO_PLAYER_PLAYER_VOTED_TO_KILL)
        return;

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
            case E_POWER::voteToKill:
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
        if(mMSG->command == CMD_TO_PLAYER_ELECT_CHANCELOR
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
