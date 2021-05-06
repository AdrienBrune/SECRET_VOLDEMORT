#include "widget_gameboard_top.h"
#include "ui_widget_gameboard_top.h"

Widget_Gameboard_Top::Widget_Gameboard_Top(QWidget* parent, S_MESSAGE* MSG):
    QWidget(parent),
    mMSG(MSG),
    mState(STATE_GAME_NOT_STARTED),
    ui(new Ui::Widget_Gameboard_Top)
{
    ui->setupUi(this);

    const QString messages[20] = {
        "LA PARTIE N'A PAS ENCORE COMMENCÉ",        ///< STATE_GAME_NOT_STARTED          0
        "LE MINISTRE CHOISIT LE DIRECTEUR",         ///< STATE_WAIT_DIRECTOR_NOMINATION  1
        "VEUILLEZ NOMMER UN DIRECTEUR",             ///< STATE_DIRECTOR_SELECTION        2
        "VOTER POUR L'ELECTION DU DIRECTEUR",       ///< STATE_VOTE_DIRECTOR             3
        "LE DIRECTEUR A ÉTÉ ÉLU SUITE AU VOTE",     ///< STATE_GET_VOTE_RESULT           4
        "LE MINISTRE ÉTUDIE LA LOI À VOTER",        ///< STATE_LAW_IS_VOTING_MINISTER    5
        "VEUILLEZ PIOCHER TROIS CARTES DE LOI",     ///< STATE_MINISTER_DRAW             6
        "VEUILLEZ CHOISIR LA LOI À DÉFAUSSER",      ///< STATE_MINISTER_DISCARD          7
        "LE DIRECTEUR ÉTUDIE LA LOI À VOTER",       ///< STATE_LAW_IS_VOTING_DIRECTOR    8
        "VEUILLEZ CHOSIR LA LOI À DÉFAUSSER",       ///< STATE_DIRECTOR_DISCARD          9
        "UN POUVOIR A ÉTÉ DÉBLOQUÉ",                ///< STATE_POWER_IN_PROGRESS         10
        "UNE NOUVELLE LOI A ÉTÉ VOTÉ",              ///< STATE_PUT_LAW                   11
        "REGARDEZ TROIS CARTES DE LA PILE",         ///< STATE_POWER_CHECK_PILE          12
        "REGARDEZ LE RÔLE D'UN DES JOUEURS",        ///< STATE_POWER_CHECK_ROLE          13
        "CHOISISSEZ LE PROCHAIN MINISTRE",          ///< STATE_POWER_NEW_MINISTER        14
        "CHOISISSEZ UN JOUEUR À ASSASSINER",        ///< STATE_POWER_KILL                15
        "UNE DEMANDE DE VETO EST EN NÉGOCIATION",   ///< STATE_VETO_NEGOCIATION          16
        "",                                         ///< EMPTY                           17
        "PARTIE TERMINÉE"                           ///< STATE_END_GAME                  18
    };

    for(int i = 0; i < 20; i++)
    {
        mMessages[i] = messages[i];
    }
}

Widget_Gameboard_Top::~Widget_Gameboard_Top()
{
    delete ui;
}

quint8 Widget_Gameboard_Top::getIndexMessageCorrespondingToSituation()
{
    switch(mMSG->gameStatus.players[mMSG->identifier].electionRole)
    {
        case E_ELECTION_ROLE::minister:
            return getIndexMessageMinister();

        case E_ELECTION_ROLE::director:
            return getIndexMessageDirector();

        default:
            return getIndexMessageNoRole();
    }
}

quint8 Widget_Gameboard_Top::getIndexMessageMinister()
{
    switch(mMSG->command)
    {
        case CMD_TO_PLAYER_START_GAME:
        case CMD_TO_PLAYER_ELECT_DIRECTOR:
            return STATE_DIRECTOR_SELECTION;

        case CMD_TO_PLAYER_START_VOTE:
        case CMD_TO_PLAYER_PLAYER_VOTED:
            return STATE_VOTE_DIRECTOR;

        case CMD_TO_PLAYER_MINISTER_DRAW:
            return STATE_MINISTER_DISCARD;

        case CMD_TO_PLAYER_DIRECTOR_DISCARD:
            return STATE_LAW_IS_VOTING_DIRECTOR;

        case CMD_TO_PLAYER_ASK_MINISTER_TO_VETO:
        case CMD_TO_PLAYER_VETO_RESULT:
            return STATE_VETO_NEGOCIATION;

        case CMD_TO_PLAYER_PUT_LAW_ON_BOARD:
            switch(mMSG->gameStatus.players[mMSG->identifier].power)
            {
                case E_POWER::checkPile:
                    return STATE_POWER_CHECK_PILE;

                case E_POWER::checkRole:
                    return STATE_POWER_CHECK_ROLE;

                case E_POWER::chooseMinister:
                    return STATE_POWER_NEW_MINISTER;

                case E_POWER::kill:
                    return STATE_POWER_KILL;

                case E_POWER::noPower:
                    return STATE_LAW_IS_VOTING_DIRECTOR;
            }

        case CMD_TO_PLAYER_END_GAME:
            return STATE_END_GAME;

        default:
            return STATE_GAME_NOT_STARTED;
    }
}

quint8 Widget_Gameboard_Top::getIndexMessageDirector()
{
    switch(mMSG->command)
    {
        case CMD_TO_PLAYER_START_GAME:
        case CMD_TO_PLAYER_ELECT_DIRECTOR:
            return STATE_WAIT_DIRECTOR_NOMINATION;

        case CMD_TO_PLAYER_START_VOTE:
        case CMD_TO_PLAYER_PLAYER_VOTED:
            return STATE_VOTE_DIRECTOR;

        case CMD_TO_PLAYER_MINISTER_DRAW:
            return STATE_LAW_IS_VOTING_MINISTER;

        case CMD_TO_PLAYER_DIRECTOR_DISCARD:
            return STATE_DIRECTOR_DISCARD;

        case CMD_TO_PLAYER_ASK_MINISTER_TO_VETO:
        case CMD_TO_PLAYER_VETO_RESULT:
            return STATE_VETO_NEGOCIATION;

        case CMD_TO_PLAYER_PUT_LAW_ON_BOARD:
            return STATE_PUT_LAW;

        case CMD_TO_PLAYER_END_GAME:
            return STATE_END_GAME;

        default:
            return STATE_GAME_NOT_STARTED;
    }
}

quint8 Widget_Gameboard_Top::getIndexMessageNoRole()
{
    switch(mMSG->command)
    {
        case CMD_TO_PLAYER_START_GAME:
        case CMD_TO_PLAYER_ELECT_DIRECTOR:
            return STATE_WAIT_DIRECTOR_NOMINATION;

        case CMD_TO_PLAYER_START_VOTE:
        case CMD_TO_PLAYER_PLAYER_VOTED:
            return STATE_VOTE_DIRECTOR;

        case CMD_TO_PLAYER_MINISTER_DRAW:
            return STATE_LAW_IS_VOTING_MINISTER;

        case CMD_TO_PLAYER_DIRECTOR_DISCARD:
            return STATE_LAW_IS_VOTING_DIRECTOR;

        case CMD_TO_PLAYER_ASK_MINISTER_TO_VETO:
        case CMD_TO_PLAYER_VETO_RESULT:
            return STATE_VETO_NEGOCIATION;

        case CMD_TO_PLAYER_PUT_LAW_ON_BOARD:
            return STATE_PUT_LAW;

        case CMD_TO_PLAYER_END_GAME:
            return STATE_END_GAME;

        default:
            return STATE_GAME_NOT_STARTED;
    }
}

void Widget_Gameboard_Top::updateWidget()
{
    update();
}

void Widget_Gameboard_Top::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(":/images/border_top.png")));
    painter.setRenderHint(QPainter::Antialiasing);

    if(mMSG->identifier == E_IDENTIFIER::ID_none)
        return;

    QPen pen_black(QBrush(QColor(0x00,0x00,0x00)), 1);
    QPen pen_white(QBrush(QColor(0xCC,0xCC,0xCC)), 1);

    QRect text = QRect(ui->label->x(), ui->label->y(), ui->label->width(), ui->label->height());
    QRect shadow = QRect(text.x() + 3, text.y(), text.width(), text.height());

    mState = getIndexMessageCorrespondingToSituation();

    QFont font("Harry P");
    font.setPixelSize(ui->label->width()/25);
    font.setBold(true);
    painter.setFont(font);

    painter.setPen(pen_black);
    painter.drawText(shadow, Qt::AlignCenter, mMessages[mState]);

    painter.setPen(pen_white);
    painter.drawText(text, Qt::AlignCenter, mMessages[mState]);
}
