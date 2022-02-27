#ifndef TYPES_H
#define TYPES_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <stdio.h>
#include <QDebug>

#define LOG

typedef enum
{
    Voldemort = 0,
    Lucius = 1,
    Beatrix = 2,
    Drago = 3,
    Harry = 4,
    Albus = 5,
    Ron = 6,
    Hermione = 7,
    Sirius = 8,
    Neville = 9,
    noOne = 10

}E_ROLE_NAME;

typedef enum
{
    notAssigned = 11,
    phenixOrder = 12,
    deathEaters = 13,
    voldemort = 14

}E_ROLE;

typedef enum
{
    none = 15,
    minister = 16,
    viceMinister = 17,
    director = 18,
    viceDirector = 19

}E_ELECTION_ROLE;

typedef enum
{
    notPlaying = 20,
    alive = 21,
    dead = 22

}E_PLAYER_STATUS;

typedef enum
{
    phenixOrderLaw = 23,
    deathEatersLaw = 24

}E_CARD;

typedef enum
{
    blank = 25,
    lumos = 26,
    nox = 27

}E_VOTE;

typedef enum
{
    noPower = 28,
    checkPile = 29,
    checkRole = 30,
    chooseMinister = 31,
    kill = 32,

}E_POWER;

typedef enum
{
    notStarted = 33,
    notFinished = 34,
    phenixOrderWon = 35,
    deathEatersWon = 36,
    voldemortElected = 37,
    voldemortKilled = 38

}E_END_GAME;

typedef enum
{
    ID_none = 39,
    ID_player1 = 0,
    ID_player2 = 1,
    ID_player3 = 2,
    ID_player4 = 3,
    ID_player5 = 4,
    ID_player6 = 5,
    ID_player7 = 6,
    ID_player8 = 7,
    ID_player9 = 8,
    ID_player10 = 9,
    ID_server = 41

}E_IDENTIFIER;

inline E_IDENTIFIER & operator++(E_IDENTIFIER& id)
{
    using IntType = typename std::underlying_type<E_IDENTIFIER>::type;
    id = static_cast<E_IDENTIFIER>(static_cast<IntType>(id) + 1);
    return id;
}

inline E_IDENTIFIER operator++(E_IDENTIFIER& id, int)
{
    E_IDENTIFIER result = id;
    ++id;
    return result;
}

typedef struct
{
    E_PLAYER_STATUS status;
    QTcpSocket * socket;
    E_IDENTIFIER identifier;
    QString name;
    E_ROLE role;
    E_ROLE_NAME roleName;
    E_ELECTION_ROLE electionRole;
    E_VOTE vote;
    E_POWER power;

}S_PLAYER;

inline QDataStream & operator<<(QDataStream & stream, const S_PLAYER & MSG)
{
    quint32 status = MSG.status,
            electionRole = MSG.electionRole,
            power = MSG.power,
            role = MSG.role,
            roleName = MSG.roleName,
            vote = MSG.vote;

    return stream << status << MSG.identifier << MSG.name << role << roleName << electionRole << vote << power;
}

inline QDataStream & operator >>(QDataStream & stream, S_PLAYER & MSG)
{
    quint32 enumToCast;

    MSG = S_PLAYER();

    stream >> enumToCast;
    MSG.status = static_cast<E_PLAYER_STATUS>(enumToCast);
    stream >> enumToCast;
    MSG.identifier = static_cast<E_IDENTIFIER>(enumToCast);
    stream >> MSG.name;
    stream >> enumToCast;
    MSG.role = static_cast<E_ROLE>(enumToCast);
    stream >> enumToCast;
    MSG.roleName = static_cast<E_ROLE_NAME>(enumToCast);
    stream >> enumToCast;
    MSG.electionRole = static_cast<E_ELECTION_ROLE>(enumToCast);
    stream >> enumToCast;
    MSG.vote = static_cast<E_VOTE>(enumToCast);
    stream >> enumToCast;
    MSG.power = static_cast<E_POWER>(enumToCast);

    return stream;
}

typedef struct
{
    uint8_t boardFaciste;
    uint8_t boardLiberal;
    E_POWER boardPower[6];

}S_BOARD;

inline QDataStream & operator<<(QDataStream & stream, const S_BOARD & MSG)
{
    stream << MSG.boardFaciste << MSG.boardLiberal;
    for(int i = 0; i < 6; i++)
        stream << MSG.boardPower[i];
    return stream;
}

inline QDataStream & operator >>(QDataStream & stream, S_BOARD & MSG)
{
    qint32 tmp;

    stream >> MSG.boardFaciste >> MSG.boardLiberal;
    for(int i = 0; i < 6; i++)
    {
        stream >> tmp;
        MSG.boardPower[i] = static_cast<E_POWER>(tmp);
    }
    return stream;
}

typedef struct
{
    QList<E_CARD> pile;
    QList<S_PLAYER> players;
    E_IDENTIFIER playerFocus;
    S_BOARD board;
    E_END_GAME endGame;
    quint8 electionTracker;
    bool vetoPower;

}S_GAME_STATUS;

inline QDataStream & operator<<(QDataStream & stream, const S_GAME_STATUS & MSG)
{
    QList<quint32> cards;
    S_PLAYER player;

    return stream << MSG.board << MSG.endGame << MSG.pile << MSG.players << MSG.playerFocus << MSG.electionTracker << MSG.vetoPower;
}

inline QDataStream & operator >>(QDataStream & stream, S_GAME_STATUS & MSG)
{
    QList<quint32> cards;
    QList<S_PLAYER> players;
    quint32 tmp;
    S_PLAYER player;

    MSG = S_GAME_STATUS();
    while(!MSG.pile.isEmpty())
        MSG.pile.removeLast();
    while(!MSG.players.isEmpty())
        MSG.players.removeLast();

    stream >> MSG.board;
    stream >> tmp;
    MSG.endGame = static_cast<E_END_GAME>(tmp);
    stream >> cards;
    for(quint32 card : cards)
        MSG.pile.append(static_cast<E_CARD>(card));
    stream >> players;
    for(S_PLAYER player : players)
        MSG.players.append(static_cast<S_PLAYER>(player));
    stream >> tmp;
    MSG.playerFocus = static_cast<E_IDENTIFIER>(tmp);
    stream >> MSG.electionTracker;
    stream >> MSG.vetoPower;

    return stream;
}

typedef struct
{
    E_IDENTIFIER identifier;
    int garbage;
    uint8_t command;
    S_GAME_STATUS gameStatus;

}S_MESSAGE;

inline QDataStream & operator<<(QDataStream & stream, const S_MESSAGE & MSG)
{
    return stream << MSG.identifier << MSG.garbage << MSG.command << MSG.gameStatus;
}

inline QDataStream & operator>>(QDataStream & stream, S_MESSAGE & MSG)
{
    qint32 tmp;
    MSG = S_MESSAGE();

    stream >> tmp ;
    MSG.identifier = static_cast<E_IDENTIFIER>(tmp);
    stream >> MSG.garbage >> MSG.command >> MSG.gameStatus;

    return stream;
}

// Commandes

#define CMD_TO_PLAYER_START_GAME            30
#define CMD_TO_SERVER_START_NEW_TURN        0
#define CMD_TO_PLAYER_ELECT_CHANCELOR       1
#define CMD_TO_SERVER_GIVE_CHANCELOR        2
#define CMD_TO_PLAYER_START_VOTE            3
#define CMD_TO_SERVER_PLAYER_VOTED          4
#define CMD_TO_PLAYER_PLAYER_VOTED          40
#define CMD_TO_PLAYER_PRESIDENT_DRAW        5
#define CMD_TO_SERVER_PRESIDENT_DISCARDED   6
#define CMD_TO_PLAYER_CHANCELOR_DISCARD     7
#define CMD_TO_SERVER_CHANCELOR_DISCARDED   8
#define CMD_TO_PLAYER_PUT_LAW_ON_BOARD      9
#define CMD_TO_SERVER_END_TURN_OK           10
#define CMD_TO_SERVER_NEW_PRESIDENT         11
#define CMD_TO_SERVER_KILL_PLAYER           12
#define CMD_TO_PLAYER_END_GAME              16
#define CMD_TO_SERVER_CHANGE_NAME           17
#define CMD_TO_PLAYER_INIT_COMMUNICATION    18
#define CMD_TO_SERVER_DIRECTOR_ASKED_VETO   19
#define CMD_TO_PLAYER_ASK_MINISTER_TO_VETO  20
#define CMD_TO_SERVER_MINISTER_VETO_REPLY   21
#define CMD_TO_PLAYER_VETO_RESULT           22
#define CMD_TO_PLAYER_SET_NEW_IDENTIFIER    23
#define CMD_TO_PLAYER_NEW_CONNECTION        24
#define CMD_TO_SERVER_JOIN_GAME             25
#define CMD_TO_PLAYER_PLAYER_JOINED         26
#define CMD_TO_PLAYER_PLAYER_LEFT_GAME      27


// Resource paths.

#define RES_ROLE_VOLDEMORT          "Role_Voldermort.png"
#define RES_ROLE_LUCIUS             "Role_Lucius.png"
#define RES_ROLE_BEATRIX            "Role_Beatrix.png"
#define RES_ROLE_DRAGO              "Role_Drago.png"
#define RES_ROLE_HARRY              "Role_Harry.png"
#define RES_ROLE_ALBUS              "Role_Albus.png"
#define RES_ROLE_RON                "Role_Ron.png"
#define RES_ROLE_HERMIONE           "Role_Hermione.png"
#define RES_ROLE_SIRIUS             "Role_Sirius.png"
#define RES_ROLE_NEVILLE            "Role_Neville.png"
#define RES_VOLDEMORT               "Voldemort.png"
#define RES_LUCIUS                  "Lucius.png"
#define RES_BEATRIX                 "Beatrix.png"
#define RES_DRAGO                   "Drago.png"
#define RES_ALLEGIANCE_DEATHEATERS  "Allegeance_Mangemort.png"
#define RES_ALLEGIANCE_PHENIXORDER  "Allegeance_OrdrePhenix.png"
#define RES_BOARD_PHENIX            "Plateau_Phenix.png"
#define RES_BOARD_DEATHEATERS_56    "Plateau_Mangemort_56.png"
#define RES_BOARD_DEATHEATERS_78    "Plateau_Mangemort_78.png"
#define RES_BOARD_DEATHEATERS_910   "Plateau_Mangemort_910.png"
#define RES_DIRECTOR                "Chevalet_Directeur.png"
#define RES_MINISTER                "Chevalet_Ministre.png"
#define RES_LAW_DEATHEATERS         "Tuile_Mangemort.png"
#define RES_LAW_PHENIXORDER         "Tuile_Phenix.png"
#define RES_LAW_BACK                "Tuile.png"
#define RES_SECRET_ROLE             "Secret_Role.png"
#define RES_VOTE_LUMOS              "Vote_Lumos.png"
#define RES_VOTE_NOX                "Vote_Nox.png"

#define COLOR_DARK                  QColor(24,24,24)
#define COLOR_LIGHT_DARK            QColor(40,40,40)


/*
QDebug operator<<(QDebug debug, const S_PLAYER & player)
{
    debug.nospace() << player.name;
    return debug;
}

inline QDebug operator<<(QDebug debug, const S_PLAYER &player)
{
    QString role, election, state, vote, power, roleName;
    switch(player.role)
    {
        case E_ROLE::deathEaters:
            role = "Faciste";
            break;

        case E_ROLE::phenixOrder:
            role = "Liberal";
            break;

        case E_ROLE::voldemort:
            role = "Hitler";
            break;

        case E_ROLE::notAssigned:
            role = "Pas de rôle";
            break;
    }

    switch(player.status)
    {
        case E_PLAYER_STATUS::alive:
            state = "Vivant";
            break;

        case E_PLAYER_STATUS::dead:
            state = "Mort";
            break;

        case E_PLAYER_STATUS::notPlaying:
            state = "Hors game";
            break;
    }

    switch(player.vote)
    {
        case E_VOTE::blank:
            vote = "Pas de vote";
            break;

        case E_VOTE::lumos:
            vote = "Yah";
            break;

        case E_VOTE::nox:
            vote = "Nein";
            break;
    }

    switch(player.electionRole)
    {
        case E_ELECTION_ROLE::none:
            election = "Aucune fonction";
            break;

        case E_ELECTION_ROLE::minister:
            election = "Président";
            break;

        case E_ELECTION_ROLE::director:
            election = "Chancelier";
            break;

        case E_ELECTION_ROLE::viceMinister:
            election = "Vice Président";
            break;

        case E_ELECTION_ROLE::viceDirector:
            election = "Vice Chancelier";
            break;
    }

    switch(player.power)
    {
        case E_POWER::checkPile:
            power = "Check pile";
            break;

        case E_POWER::checkRole:
            power = "Check rôle";
            break;

        case E_POWER::chooseMinister:
            power = "Choix Président";
            break;

        case E_POWER::kill:
            power = "Kill";
            break;

        case E_POWER::voteToKill:
            power = "Kill vote";
            break;

        case E_POWER::noPower:
            power = "Aucun pouvoir";
            break;
    }

    switch(player.roleName)
    {
        case E_ROLE_NAME::Albus:
            roleName = "Albus";
            break;

        case E_ROLE_NAME::Beatrix:
            roleName = "Beatrix";
            break;

        case E_ROLE_NAME::Drago:
            roleName = "Drago";
            break;

        case E_ROLE_NAME::Harry:
            roleName = "Harry";
            break;

        case E_ROLE_NAME::Hermione:
            roleName = "Hermione";
            break;

        case E_ROLE_NAME::Lucius:
            roleName = "Lucius";
            break;

        case E_ROLE_NAME::Neville:
            roleName = "Neville";
            break;

        case E_ROLE_NAME::Ron:
            roleName = "Ron";
            break;

        case E_ROLE_NAME::Sirius:
            roleName = "Sirius";
            break;

        case E_ROLE_NAME::Voldemort:
            roleName = "Voldemort";
            break;

        case E_ROLE_NAME::noOne:
            roleName = "pas de rôle";
            break;
    }

    //qDebug() << '(';
                      << "" << player.identifier
                      << ", " << player.name
                      << ", " << role
                      << ", " << roleName
                      << ", " << state
                      << ", " << election
                      << ", " << vote
                      << ", " << power
                      << ", " << player.socket
                      << ')';

    return debug;
}

inline QDebug operator<<(QDebug debug, const S_BOARD &board)
{
    QString boardFaciste, boardLiberal, boardPower;

    boardFaciste = "Faciste :";
    boardLiberal = "Liberal :";
    boardPower = "Pouvoirs :";
    for(int i = 0; i < 6; i++)
    {
        if(i < board.boardFaciste)
        {
            boardFaciste += " O ";
        }
        else
        {
            boardFaciste += " X ";
        }

        switch(board.boardPower[i])
        {
            case E_POWER::checkPile:
                boardPower += " Check pile - ";
                break;

            case E_POWER::checkRole:
                boardPower += " Check rôle - ";
                break;

            case E_POWER::chooseMinister:
                boardPower += " Choix Président - ";
                break;

            case E_POWER::kill:
                boardPower += " Kill - ";
                break;

            case E_POWER::voteToKill:
                boardPower += " Kill vote - ";
                break;

            case E_POWER::noPower:
                boardPower += " Vide - ";
                break;
        }
    }

    for(int i = 0; i < 5; i++)
    {
        if(i < board.boardLiberal)
        {
            boardLiberal += " O ";
        }
        else
        {
            boardLiberal += " X ";
        }
    }

    debug.nospace() << "     -------------------- BOARD --------------------";
    debug.nospace() << "     " << boardFaciste;
    debug.nospace() << "     " << boardLiberal;
    debug.nospace() << "     " << boardPower;
    debug.nospace() << "     -----------------------------------------------";
    return debug.nospace();
}

inline QDebug operator<<(QDebug debug, const S_GAME_STATUS &game)
{
    QString pile = "PILE : (", endGame;
    for(E_CARD card : game.pile)
    {
        switch(card)
        {
            case E_CARD::deathEatersLaw:
                pile += " F";
                break;

            case E_CARD::phenixOrderLaw:
                pile += " L";
                break;
        }
    }
    pile += ")";

    switch(game.endGame)
    {
        case E_END_GAME::deathEatersWon:
            endGame = "Faciste gagnant";
            break;

        case E_END_GAME::phenixOrderWon:
            endGame = "Libéraux gagant";
            break;

        case E_END_GAME::voldemortElected:
            endGame = "Hitler élu";
            break;

        case E_END_GAME::voldemortKilled:
            endGame = "Hitler tué";
            break;

        case E_END_GAME::notFinished:
            endGame = "Partie en cours";
            break;

        case E_END_GAME::notStarted:
            endGame = "Partie non commencée";
            break;
    }

    debug.nospace() << "     -------------------- GAME --------------------";
    debug.nospace() << "     " << endGame;
    debug.nospace() << "     Focus sur " << game.playerFocus;
    debug.nospace() << "     ------------------ JOUEURS ------------------";
    for(int i = 0; i < game.players.size(); i++)
        debug.nospace() << &game.players[i];
    qDebug() << "pass";
    debug.nospace() << "     ---------------------------------------------";
    debug.nospace() << game.board;
    //debug.nospace() << "     " << pile;
    //for(int i = 0; i < pile.size(); i++)
        //debug.nospace() << pile;
    debug.nospace() << "     ----------------------------------------------";
    return debug;
}

inline QDebug operator<<(QDebug debug, const S_MESSAGE &MSG)
{
    debug.nospace() << "------------------ MESSAGE --------------------";
    debug.nospace() << "Commande : "<< MSG.command << "Identifiant : " << MSG.identifier << "   garbage : " << MSG.garbage;
    debug.nospace() << MSG.gameStatus;
    debug.nospace() << "----------------------------------------------";
    return debug;
}
*/
#endif // TYPES_H
