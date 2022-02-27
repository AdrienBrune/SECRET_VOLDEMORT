#include "util_functions.h"
#include <QRandomGenerator>

E_IDENTIFIER getPlayerIdentifier(QList<S_PLAYER> players, QTcpSocket* socket)
{
    for(S_PLAYER player : players)
    {
        if(socket == player.socket)
        {
            return player.identifier;
        }
    }

    return E_IDENTIFIER::ID_none;
}

S_PLAYER *getPlayer(E_IDENTIFIER identifier, QList<S_PLAYER> & players)
{
    for(int i = 0; i < players.size(); i++)
    {
        if(players[i].identifier == identifier)
        {
            return &players[i];
        }
    }

    qDebug() << "aucun joueur trouvé avec getPlayer";

    return nullptr;
}

void initRole(S_GAME_STATUS* game)
{
    QList<E_ROLE_NAME> PhenixOrderCaracters, DeathEatersCaracters;
    PhenixOrderCaracters << E_ROLE_NAME::Albus << E_ROLE_NAME::Harry << E_ROLE_NAME::Hermione << E_ROLE_NAME::Neville << E_ROLE_NAME::Ron << E_ROLE_NAME::Sirius;
    DeathEatersCaracters << E_ROLE_NAME::Beatrix << E_ROLE_NAME::Drago << E_ROLE_NAME::Lucius;

    quint8 nbFacistes = 0, nb_liberal = 0, nbLiberalsAssigned = 0, nbFacistesAssigned = 0, hitlerAssigned = 0;
    switch(game->players.size())
    {
        case 5:
            nbFacistes = 1;
            nb_liberal = 3;
            break;

        case 6:
            nbFacistes = 1;
            nb_liberal = 4;
            break;

        case 7:
            nbFacistes = 2;
            nb_liberal = 4;
            break;

        case 8:
            nbFacistes = 2;
            nb_liberal = 5;
            break;

        case 9:
            nbFacistes = 3;
            nb_liberal = 5;
            break;

        case 10:
            nbFacistes = 3;
            nb_liberal = 6;
            break;
    }

    quint8 index = 0;
    while((nbLiberalsAssigned + nbFacistesAssigned + hitlerAssigned) != game->players.size())
    {

        quint8 role = static_cast<quint8>(QRandomGenerator::global()->bounded(game->players.size()));

        // Hitler
        if(role == 0)
        {
            if(hitlerAssigned == 0)
            {
                game->players[index].role = E_ROLE::voldemort;
                game->players[index++].roleName = E_ROLE_NAME::Voldemort;
                hitlerAssigned++;
            }
        }
        // Faciste
        else if(role < nbFacistes + 1)
        {
            if(nbFacistesAssigned < nbFacistes)
            {
                game->players[index].role = E_ROLE::deathEaters;
                game->players[index++].roleName = DeathEatersCaracters.takeAt(QRandomGenerator::global()->bounded(DeathEatersCaracters.size()));
                nbFacistesAssigned++;
            }
        }
        // Liberal
        else
        {
            if(nbLiberalsAssigned < nb_liberal)
            {
                game->players[index].role = E_ROLE::phenixOrder;
                game->players[index++].roleName = PhenixOrderCaracters.takeAt(QRandomGenerator::global()->bounded(PhenixOrderCaracters.size()));
                nbLiberalsAssigned++;
            }
        }
    }
}

void initBoard(S_GAME_STATUS* game)
{
    game->board.boardFaciste = 0;
    game->board.boardLiberal = 0;

    switch(game->players.size())
    {
        case 5:
        case 6:
            game->board.boardPower[0] = E_POWER::noPower;
            game->board.boardPower[1] = E_POWER::noPower;
            game->board.boardPower[2] = E_POWER::checkPile;
            game->board.boardPower[3] = E_POWER::kill;
            game->board.boardPower[4] = E_POWER::kill;
            game->board.boardPower[5] = E_POWER::noPower;
            break;

        case 7:
        case 8:
            game->board.boardPower[0] = E_POWER::noPower;
            game->board.boardPower[1] = E_POWER::checkRole;
            game->board.boardPower[2] = E_POWER::chooseMinister;
            game->board.boardPower[3] = E_POWER::kill;
            game->board.boardPower[4] = E_POWER::kill;
            game->board.boardPower[5] = E_POWER::noPower;
            break;

        case 9:
        case 10:
            game->board.boardPower[0] = E_POWER::checkRole;
            game->board.boardPower[1] = E_POWER::checkRole;
            game->board.boardPower[2] = E_POWER::chooseMinister;
            game->board.boardPower[3] = E_POWER::kill;
            game->board.boardPower[4] = E_POWER::kill;
            game->board.boardPower[5] = E_POWER::noPower;
            break;
    }
}

void initPile(S_GAME_STATUS* game)
{
    generateNewPile(&game->pile);
}

void initPlayers(S_GAME_STATUS* game)
{
    quint8 minister = static_cast<quint8>(QRandomGenerator::global()->bounded(game->players.size()));

    for(int i = 0; i < game->players.size(); i++)
    {
        game->players[i].power = E_POWER::noPower;
        game->players[i].role = E_ROLE::notAssigned;
        game->players[i].vote = E_VOTE::blank;
        game->players[i].electionRole = E_ELECTION_ROLE::none;
        game->players[i].status = E_PLAYER_STATUS::alive;

        if(game->players[i].identifier == minister)
        {
            game->players[i].electionRole = E_ELECTION_ROLE::minister;
        }
    }
}

void setNextPresident(S_GAME_STATUS* game)
{
    E_IDENTIFIER identifier = getPresidentIdentifier(*game);

    // Set vice Minister to none.
    for(int i = 0; i < game->players.size(); i++)
    {
        if(game->players[i].electionRole == E_ELECTION_ROLE::viceMinister)
        {
            game->players[i].electionRole = E_ELECTION_ROLE::none;
            break;
        }
    }
    // Set new vice Minister.
    for(int i = 0; i < game->players.size(); i++)
    {
        if(game->players[i].electionRole == E_ELECTION_ROLE::minister)
        {
            if(game->players.size() > 5)
                game->players[i].electionRole = E_ELECTION_ROLE::viceMinister;
            else
                game->players[i].electionRole = E_ELECTION_ROLE::none;
            break;
        }
    }

    do
    {
        if(++identifier > game->players.size() - 1)
            identifier = E_IDENTIFIER::ID_player1;

    }while(game->players[identifier].status != E_PLAYER_STATUS::alive);


    for(int i = 0; i < game->players.size(); i++)
    {
        if(game->players[i].identifier == identifier)
        {
            game->players[i].electionRole = E_ELECTION_ROLE::minister;
        }
    }
}

void setNewPresident(S_GAME_STATUS* game)
{
    // Set vice Minister to none.
    for(int i = 0; i < game->players.size(); i++)
    {
        if(game->players[i].electionRole == E_ELECTION_ROLE::viceMinister)
        {
            game->players[i].electionRole = E_ELECTION_ROLE::none;
            break;
        }
    }
    // Set new vice Minister.
    for(int i = 0; i < game->players.size(); i++)
    {
        if(game->players[i].electionRole == E_ELECTION_ROLE::minister)
        {
            game->players[i].electionRole = E_ELECTION_ROLE::viceMinister;
            break;
        }
    }

   if(game->playerFocus >= game->players.size())
   {
       qDebug() << "erreur, le président ne peut être assigné car l'index n'est pas bon";
       game->players[0].electionRole = E_ELECTION_ROLE::minister;
   }
   else
   {
       game->players[game->playerFocus].electionRole = E_ELECTION_ROLE::minister;
   }
}

void setNextPresidentAfterSpecialTurn(E_IDENTIFIER lastPresident, E_IDENTIFIER specialPresident, S_GAME_STATUS *game)
{
    E_IDENTIFIER indexPresident = lastPresident;
    do
    {
        if(++indexPresident > game->players.size() - 1)
            indexPresident = E_IDENTIFIER::ID_player1;

    }while(game->players[indexPresident].status != E_PLAYER_STATUS::alive);

    game->players[lastPresident].electionRole = E_ELECTION_ROLE::none;
    game->players[specialPresident].electionRole =  E_ELECTION_ROLE::viceMinister;

    // Set the Minister.
    for(int i = 0; i < game->players.size(); i++)
    {
        if(game->players[i].identifier == indexPresident)
        {
            game->players[i].electionRole = E_ELECTION_ROLE::minister;
        }
    }
}

void setChancelor(S_GAME_STATUS* game, E_IDENTIFIER identifier)
{
    if(identifier == E_IDENTIFIER::ID_none)
    {
        // Set vice Director to none.
        for(int i = 0; i < game->players.size(); i++)
        {
            if(game->players[i].electionRole == E_ELECTION_ROLE::viceDirector)
            {
                game->players[i].electionRole = E_ELECTION_ROLE::none;
                break;
            }
        }
        // Set new vice Director.
        for(int i = 0; i < game->players.size(); i++)
        {
            if(game->players[i].electionRole == E_ELECTION_ROLE::director)
            {
                game->players[i].electionRole = E_ELECTION_ROLE::viceDirector;
            }
        }
    }
    else // Set Director.
    {
        game->players[identifier].electionRole = E_ELECTION_ROLE::director;
    }
}

E_IDENTIFIER getAvailableIdentifier(S_GAME_STATUS* game)
{
    bool ID_alreadyUse = false;
    for(E_IDENTIFIER identifier = E_IDENTIFIER::ID_player1; identifier < E_IDENTIFIER::ID_player10 + 1; identifier++)
    {
        ID_alreadyUse = false;
        for(int i = 0; i < game->players.size(); i++)
        {
            if(game->players[i].identifier == identifier)
            {
                ID_alreadyUse = true;
            }
        }

        if(!ID_alreadyUse)
        {
            return identifier;
        }
    }

    return E_IDENTIFIER::ID_none;
}

E_ROLE getMissingRole(S_GAME_STATUS game)
{
    // Get number of player, new one included.
    quint8 numberPlayers = 1;
    for(const S_PLAYER &player : game.players)
    {
        if(player.status != E_PLAYER_STATUS::notPlaying)
        {
            numberPlayers++;
        }
    }

    // Get number facists, liberal and hitler.
    quint8 numberFacists = 0, numberLiberals = 0, hitlerAssigned = 0;
    for(const S_PLAYER &player : game.players)
    {
        if(player.status != E_PLAYER_STATUS::notPlaying)
        {
            switch(player.role)
            {
                case E_ROLE::deathEaters:
                    numberFacists++;
                    break;

                case E_ROLE::phenixOrder:
                    numberLiberals++;
                    break;

                case E_ROLE::voldemort:
                    hitlerAssigned++;
                    break;

                default:
                    break;
            }
        }
    }


    switch(numberPlayers)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            if(numberFacists < 1)
            {
                return E_ROLE::deathEaters;
            }
            if(numberLiberals < numberPlayers - 2)
            {
                return E_ROLE::phenixOrder;
            }
            if(hitlerAssigned == 0)
            {
                return E_ROLE::voldemort;
            }
            break;

        case 7:
        case 8:
            if(numberFacists < 2)
            {
                return E_ROLE::deathEaters;
            }
            if(numberLiberals < numberPlayers - 3)
            {
                return E_ROLE::phenixOrder;
            }
            if(hitlerAssigned == 0)
            {
                return E_ROLE::voldemort;
            }
            break;

        case 9:
        case 10:
            if(numberFacists < 3)
            {
                return E_ROLE::deathEaters;
            }
            if(numberLiberals < numberPlayers - 4)
            {
                return E_ROLE::phenixOrder;
            }
            if(hitlerAssigned == 0)
            {
                return E_ROLE::voldemort;
            }
            break;
    }

    return E_ROLE::notAssigned;
}

E_ROLE_NAME getMissingRoleName(E_IDENTIFIER identifier, S_GAME_STATUS game)
{
    S_PLAYER playerToSet = game.players[identifier];
    bool nameAvailable = false;
    switch(playerToSet.role)
    {
        case E_ROLE::deathEaters:
            while(!nameAvailable)
            {
                nameAvailable = true;
                playerToSet.roleName = static_cast<E_ROLE_NAME>(QRandomGenerator::global()->bounded(3) + 1);

                for(const S_PLAYER & player : game.players)
                {
                    if(playerToSet.roleName == player.roleName)
                    {
                        nameAvailable = false;
                    }
                }
            }
            return playerToSet.roleName;

        case E_ROLE::phenixOrder:
            while(!nameAvailable)
            {
                nameAvailable = true;
                playerToSet.roleName = static_cast<E_ROLE_NAME>(QRandomGenerator::global()->bounded(6) + 4);

                for(const S_PLAYER & player : game.players)
                {
                    if(playerToSet.roleName == player.roleName)
                    {
                        nameAvailable = false;
                    }
                }
            }
            return playerToSet.roleName;

        case E_ROLE::voldemort:
            return E_ROLE_NAME::Voldemort;

        default:
            return E_ROLE_NAME::noOne;
    }
}

E_IDENTIFIER getPresidentIdentifier(S_GAME_STATUS game)
{
    for(const S_PLAYER & player : game.players)
    {
        if(player.electionRole == E_ELECTION_ROLE::minister)
        {
            qDebug() << "pres id : " << player.identifier;
            return player.identifier;
        }
    }

    qDebug() << "erreur, aucun président trouvé";

    return E_IDENTIFIER::ID_none;
}

void copyNewPile(QList<E_CARD> &newPile, QList<E_CARD> pileToCopy)
{
    while(!newPile.isEmpty())
        newPile.removeLast();

    for(E_CARD card : pileToCopy)
    {
        newPile.append(card);
    }
}

void generateNewPile(QList<E_CARD> * pile)
{
    quint8 LCardsNotput = 6, FCardsNotput = 11;

    while(!pile->isEmpty())
        pile->removeLast();

    do
    {
        quint8 FCardPourcentage = static_cast<quint8>((static_cast<qreal>(FCardsNotput) / static_cast<qreal>(FCardsNotput + LCardsNotput)) * 100);
        quint8 pourcentage = static_cast<quint8>(QRandomGenerator::global()->bounded(100 + 1));

        if(pourcentage <= FCardPourcentage)
        {
            if(FCardsNotput != 0)
            {
                pile->append(E_CARD::deathEatersLaw);
                FCardsNotput--;
            }
        }
        else
        {
            if(LCardsNotput != 0)
            {
                pile->append(E_CARD::phenixOrderLaw);
                LCardsNotput--;
            }
        }

    }while(LCardsNotput != 0 || FCardsNotput != 0);
}

QString getPowerString(E_POWER power)
{
    switch(power)
        {
        case E_POWER::noPower:
            return "aucun";

        case E_POWER::checkPile:
            return "check pile";

        case E_POWER::checkRole:
            return "check role";

        case E_POWER::chooseMinister:
            return "selection Président";

        case E_POWER::kill:
            return "execution";
    }
}

QString getVoteString(E_VOTE vote)
{
    switch(vote)
    {
        case E_VOTE::blank:
            return "aucun";

        case E_VOTE::lumos:
            return "yah";

        case E_VOTE::nox:
        return "nein";
    }
}

QString getRoleString(E_ROLE role)
{
    switch(role)
    {
        case E_ROLE::notAssigned:
            return "non assigné";

        case E_ROLE::deathEaters:
            return "facist";

        case E_ROLE::phenixOrder:
            return "libéral";

        case E_ROLE::voldemort:
            return "hitler";
    }
}

QString getCommandString(quint8 command)
{
    switch(command)
    {
        case CMD_TO_PLAYER_START_GAME:
            return "Js < commencer la partie";

        case CMD_TO_SERVER_START_NEW_TURN:
            return "S < commencer un nouveau tour";

        case CMD_TO_PLAYER_ELECT_CHANCELOR:
            return "Js < commencer l'élection du Chancelier";

        case CMD_TO_SERVER_GIVE_CHANCELOR:
            return "S < choix du Chancelier";

        case CMD_TO_PLAYER_START_VOTE:
            return "Js < commencer le vote du Chancelier";

        case CMD_TO_SERVER_PLAYER_VOTED:
            return "S < un joueur a voté";

        case CMD_TO_PLAYER_PLAYER_VOTED:
            return "Js < un joueur a voté";

        case CMD_TO_PLAYER_PRESIDENT_DRAW:
            return "Js < le Président doit piocher trois lois";

        case CMD_TO_SERVER_PRESIDENT_DISCARDED:
            return "S < le Président a défaussé un loi";

        case CMD_TO_PLAYER_CHANCELOR_DISCARD:
            return "Js < le Chancelier doit défausser une loi";

        case CMD_TO_SERVER_CHANCELOR_DISCARDED:
            return "S < le Chancelier a défaussé une loi";

        case CMD_TO_PLAYER_PUT_LAW_ON_BOARD:
            return "Js < une loi a été votée";

        case CMD_TO_SERVER_END_TURN_OK:
            return "S < fin du tour";

        case CMD_TO_SERVER_NEW_PRESIDENT:
            return "S < choix du prochain Président";

        case CMD_TO_SERVER_KILL_PLAYER:
            return "S < execution d'un joueur";

        case CMD_TO_PLAYER_END_GAME:
            return "Js < fin de la partie";

        case CMD_TO_SERVER_CHANGE_NAME:
            return "S < changement de nom d'un joueur";

        case CMD_TO_PLAYER_INIT_COMMUNICATION:
            return "Js < initialisation de la communication avec le serveur";

        case CMD_TO_PLAYER_SET_NEW_IDENTIFIER:
            return "Js < changement de l'identifiant";

        case CMD_TO_PLAYER_NEW_CONNECTION:
            return "Js < nouvelle connexion";

        case CMD_TO_PLAYER_PLAYER_JOINED:
            return "Js < join la partie";

        default:
            return "commande non renseignée";
    }
}

quint8 getNumberPlayersPlaying(S_GAME_STATUS game)
{
    quint8 numberPlayers = 0;
    for(const S_PLAYER &player : game.players)
    {
        if(player.status != E_PLAYER_STATUS::notPlaying)
        {
            numberPlayers++;
        }
    }

    return numberPlayers;
}

void printPlayer(const S_PLAYER &player)
{
    QString roleNameMapping[] = {
        "Voldemort",
        "Lucius",
        "Beatrix",
        "Drago",
        "Harry",
        "Albus",
        "Ron",
        "Hermione",
        "Sirius",
        "Neville",
        "pas de nom"
    };

    QString powerMapping[] = {
        "X",
        "regarder la pile",
        "regarder un rôle",
        "sélection du Ministre",
        "Avada Kedavra",
        "vote + Avada Kedavra"
    };

    QString electionRoleMapping[] = {
        "Sorcier",
        "Ministre",
        "Vice Directeur",
        "Directeur",
        "Vice Ministre"
    };

    QString roleMapping[] = {
        "pas de rôle",
        "Ordre du Phénix",
        "Mangemort",
        "Chef des Mangemort"
    };

    QString voteMapping[] = {
        "X",
        "Lumos",
        "Nox"
    };

    switch(player.status)
    {
        case E_PLAYER_STATUS::alive:
            qDebug().nospace() << "[O][id:" << player.identifier << " " << player.name << "]"
                << " (" << roleMapping[player.role-E_ROLE::notAssigned] << " : " << roleNameMapping[player.roleName-E_ROLE_NAME::Voldemort]
                << "(" << electionRoleMapping[player.electionRole-E_ELECTION_ROLE::none] << "))"
                << " (vote : " << voteMapping[player.vote-E_VOTE::blank] << " pouvoir : " << powerMapping[player.power-E_POWER::noPower] << ")";
            break;

        case E_PLAYER_STATUS::dead:
            qDebug().nospace() << "[X][id:" << player.identifier << " " << player.name << "]"
                << " (" << roleMapping[player.role-E_ROLE::notAssigned] << " : " << roleNameMapping[player.roleName-E_ROLE_NAME::Voldemort]
                << "(" << electionRoleMapping[player.electionRole-E_ELECTION_ROLE::none] << "))"
                << " (vote : " << voteMapping[player.vote-E_VOTE::blank] << " pouvoir : " << powerMapping[player.power-E_POWER::noPower] << ")";
            break;

        case E_PLAYER_STATUS::notPlaying:
            qDebug().nospace() << "[-][id:" << player.identifier << " " << player.name << "]";
            break;
    }
}

void printBoard(const S_BOARD &board)
{
    QString topBoard, botBoard;

    QString powerMapping[] = {
        "-",
        "P",
        "R",
        "S",
        "K",
        "V"
    };

    for(int i = 0; i < 5; i++)
    {
        if(i < board.boardLiberal)
        {
            topBoard += "O ";
        }
        else
        {
            topBoard += "- ";
        }
    }
    topBoard += "  ";

    for(int i = 0; i < 6; i++)
    {
        if(i < board.boardFaciste)
        {
            botBoard += "O ";
        }
        else
        {
            botBoard += powerMapping[board.boardPower[i]-E_POWER::noPower] + " ";
        }
    }

    qDebug() << "------PLATEAU-------";
    qDebug() << "  " << topBoard;
    qDebug() << "  " << botBoard;
    qDebug() << "--------------------";
}

void printGame(const S_GAME_STATUS & game)
{
    QString statusMapping[] = {
        "Partie en attente",
        "Partie en cours",
        "Ordre du Phenix victorieux",
        "Mangemorts victorieux",
        "Voldemort a prit le pouvoir",
        "Voldemort tué"
    };

    qDebug();
    qDebug() << "-------PARTIE-------";
    qDebug() << statusMapping[game.endGame-E_END_GAME::notStarted];
    qDebug() << "-------JOUEURS------";
    if(game.players.size() != 0)
        if(game.playerFocus != E_IDENTIFIER::ID_none)
            qDebug() << "Focus : " << game.playerFocus;
    for(const S_PLAYER & player : game.players)
        printPlayer(player);
    qDebug() << "--------------------";
    printBoard(game.board);
    qDebug();
}

quint8 getNumberPlayersAlive(S_GAME_STATUS game)
{
    quint8 playersAlive = 0;
    for(int i = 0; i < game.players.size(); i++)
    {
        if(game.players[i].status == E_PLAYER_STATUS::alive)
            playersAlive++;
    }
    return playersAlive;
}
