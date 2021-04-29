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
        "La partie n'a pas encore commencé",        ///< STATE_GAME_NOT_STARTED          0
        "Le président choisie son Chancelier",      ///< STATE_WAIT_CHANCELOR_NOMINATION 1
        "Veuillez nommer un Chancelier",            ///< STATE_CHANCELOR_SELECTION       2
        "Votez pour élire le joueur Chancelier",    ///< STATE_VOTE_CHANCELOR            3
        "Le joueur a été élu suite au vote",        ///< STATE_GET_VOTE_RESULT           4
        "Le Président étudie la loi a voter",       ///< STATE_LAW_IS_VOTING_PRESIDENT   5
        "Veuillez piocher trois cartes de loi",     ///< STATE_PRESIDENT_DRAW            6
        "Veuillez choisir la loi a défausser",      ///< STATE_PRESIDENT_DISCARD         7
        "Le chancelier étudie la loi à voter",      ///< STATE_LAW_IS_VOTING_CHANCELOR   8
        "Veuillez choisir la loi à défausser",      ///< STATE_CHANCELOR_DISCARD         9
        "Un pouvoir a été débloqué",                ///< STATE_POWER_IN_PROGRESS         10
        "Regardez trois cartes de la pile",         ///< STATE_POWER_CHECK_PILE          11
        "Regardez le role d'un des joueurs",        ///< STATE_POWER_CHECK_ROLE          12
        "Choisissez le prochain Président",         ///< STATE_POWER_NEW_PRESIDENT       13
        "Choisissez un joueur à assassiner",        ///< STATE_POWER_KILL                14
        "Votez pour executer ce joueur",            ///< STATE_START_VOTE_TO_KILL        15
        "Choisissez un joueur à executer",          ///< STATE_POWER_CHOOSE_TO_KILL      16
        "La partie est finie"                       ///< STATE_END_GAME                  17
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
            return getIndexMessagePresident();

        case E_ELECTION_ROLE::director:
            return getIndexMessageChancelor();

        default:
            return getIndexMessageNoRole();
    }
}

quint8 Widget_Gameboard_Top::getIndexMessagePresident()
{
    switch(mMSG->command)
    {
        case CMD_TO_PLAYER_START_GAME:
        case CMD_TO_PLAYER_ELECT_CHANCELOR:
            return STATE_CHANCELOR_SELECTION;

        case CMD_TO_PLAYER_START_VOTE:
        case CMD_TO_PLAYER_PLAYER_VOTED:
            return STATE_VOTE_CHANCELOR;

        case CMD_TO_PLAYER_PRESIDENT_DRAW:
            return STATE_PRESIDENT_DISCARD;

        case CMD_TO_PLAYER_CHANCELOR_DISCARD:
            return STATE_LAW_IS_VOTING_CHANCELOR;

        case CMD_TO_PLAYER_PUT_LAW_ON_BOARD:
            switch(mMSG->gameStatus.players[mMSG->identifier].power)
            {
                case E_POWER::checkPile:
                    return STATE_POWER_CHECK_PILE;

                case E_POWER::checkRole:
                    return STATE_POWER_CHECK_ROLE;

                case E_POWER::chooseMinister:
                    return STATE_POWER_NEW_PRESIDENT;

                case E_POWER::kill:
                    return STATE_POWER_KILL;

                case E_POWER::voteToKill:
                    return STATE_POWER_CHOOSE_TO_KILL;

                case E_POWER::noPower:
                    return STATE_LAW_IS_VOTING_CHANCELOR;
            }

        case CMD_TO_PLAYER_VOTE_TO_KILL:
        case CMD_TO_PLAYER_PLAYER_VOTED_TO_KILL:
            return STATE_START_VOTE_TO_KILL;

        case CMD_TO_PLAYER_END_GAME:
            return STATE_END_GAME;

        default:
            return STATE_GAME_NOT_STARTED;
    }
}

quint8 Widget_Gameboard_Top::getIndexMessageChancelor()
{
    switch(mMSG->command)
    {
        case CMD_TO_PLAYER_START_GAME:
        case CMD_TO_PLAYER_ELECT_CHANCELOR:
            return STATE_WAIT_CHANCELOR_NOMINATION;

        case CMD_TO_PLAYER_START_VOTE:
        case CMD_TO_PLAYER_PLAYER_VOTED:
            return STATE_VOTE_CHANCELOR;

        case CMD_TO_PLAYER_PRESIDENT_DRAW:
            return STATE_LAW_IS_VOTING_PRESIDENT;

        case CMD_TO_PLAYER_CHANCELOR_DISCARD:
            return STATE_CHANCELOR_DISCARD;

        case CMD_TO_PLAYER_PUT_LAW_ON_BOARD:
            return STATE_POWER_IN_PROGRESS;

        case CMD_TO_PLAYER_VOTE_TO_KILL:
        case CMD_TO_PLAYER_PLAYER_VOTED_TO_KILL:
            return STATE_START_VOTE_TO_KILL;

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
        case CMD_TO_PLAYER_ELECT_CHANCELOR:
            return STATE_WAIT_CHANCELOR_NOMINATION;

        case CMD_TO_PLAYER_START_VOTE:
        case CMD_TO_PLAYER_PLAYER_VOTED:
            return STATE_VOTE_CHANCELOR;

        case CMD_TO_PLAYER_PRESIDENT_DRAW:
            return STATE_LAW_IS_VOTING_PRESIDENT;

        case CMD_TO_PLAYER_CHANCELOR_DISCARD:
            return STATE_LAW_IS_VOTING_CHANCELOR;

        case CMD_TO_PLAYER_PUT_LAW_ON_BOARD:
            return STATE_POWER_IN_PROGRESS;

        case CMD_TO_PLAYER_VOTE_TO_KILL:
        case CMD_TO_PLAYER_PLAYER_VOTED_TO_KILL:
            return STATE_START_VOTE_TO_KILL;

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

void Widget_Gameboard_Top::setGameMessage(quint8 state)
{
   /*
   mState = state;
   update();
   */
}

void Widget_Gameboard_Top::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap::fromImage(QImage(":/images/border_top.png")));
    painter.setRenderHint(QPainter::Antialiasing);

    if(mMSG->identifier == E_IDENTIFIER::ID_none)
        return;

    QPen pen_black(QBrush(QColor(0x00,0x00,0x00)), 1);
    QPen pen_white(QBrush(QColor(0xFF,0xFF,0xFF)), 1);

    QRect text = QRect(ui->label->x(), ui->label->y(), ui->label->width(), ui->label->height());
    QRect shadow = QRect(text.x() + 3, text.y(), text.width(), text.height());

    mState = getIndexMessageCorrespondingToSituation();

    QFont font("Castellar");
    font.setStretch(45);
    font.setPixelSize(ui->label->width()/21);
    font.setBold(true);
    painter.setFont(font);

    painter.setPen(pen_black);
    painter.drawText(shadow, Qt::AlignCenter, mMessages[mState]);

    painter.setPen(pen_white);
    painter.drawText(text, Qt::AlignCenter, mMessages[mState]);
}
