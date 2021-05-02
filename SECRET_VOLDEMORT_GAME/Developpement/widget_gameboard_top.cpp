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
        "Le Ministre choisie le Directeur",         ///< STATE_WAIT_DIRECTOR_NOMINATION  1
        "Veuillez nommer un Directeur",             ///< STATE_DIRECTOR_SELECTION        2
        "Votez pour élire le joueur Directeur",     ///< STATE_VOTE_DIRECTOR             3
        "Le joueur a été élu suite au vote",        ///< STATE_GET_VOTE_RESULT           4
        "Le Ministre étudie la loi a voter",        ///< STATE_LAW_IS_VOTING_MINISTER    5
        "Veuillez piocher trois cartes de loi",     ///< STATE_MINISTER_DRAW             6
        "Veuillez choisir la loi a défausser",      ///< STATE_MINISTER_DISCARD          7
        "Le Directeur étudie la loi à voter",       ///< STATE_LAW_IS_VOTING_DIRECTOR    8
        "Veuillez choisir la loi à défausser",      ///< STATE_DIRECTOR_DISCARD          9
        "Un pouvoir a été débloqué",                ///< STATE_POWER_IN_PROGRESS         10
        "Une nouvelle loi a été votée",             ///< STATE_PUT_LAW                   11
        "Regardez trois cartes de la pile",         ///< STATE_POWER_CHECK_PILE          12
        "Regardez le role d'un des joueurs",        ///< STATE_POWER_CHECK_ROLE          13
        "Choisissez le prochain Ministre",          ///< STATE_POWER_NEW_MINISTER        14
        "Choisissez un joueur à assassiner",        ///< STATE_POWER_KILL                15
        "Une demande de veto est en négociation",   ///< STATE_VETO_NEGOCIATION          16
        "",                                         ///< EMPTY                           17
        "La partie est finie"                       ///< STATE_END_GAME                  18
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
