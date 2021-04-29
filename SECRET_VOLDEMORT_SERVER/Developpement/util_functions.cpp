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

#ifndef LOG
    qDebug() << "aucun joueur trouvé avec getPlayer";
#endif

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
                game->players[index].role = E_ROLE::hitler;
                game->players[index++].roleName = E_ROLE_NAME::Voldemort;
                hitlerAssigned++;
            }
        }
        // Faciste
        else if(role < nbFacistes + 1)
        {
            if(nbFacistesAssigned < nbFacistes)
            {
                game->players[index].role = E_ROLE::faciste;
                game->players[index++].roleName = DeathEatersCaracters.takeAt(QRandomGenerator::global()->bounded(DeathEatersCaracters.size()));
                nbFacistesAssigned++;
            }
        }
        // Liberal
        else
        {
            if(nbLiberalsAssigned < nb_liberal)
            {
                game->players[index].role = E_ROLE::liberal;
                game->players[index++].roleName = PhenixOrderCaracters.takeAt(QRandomGenerator::global()->bounded(PhenixOrderCaracters.size()));
                nbLiberalsAssigned++;
            }
        }
    }
}

void initBoard(S_GAME_STATUS* game)
{
#ifndef LOG
    qDebug() << "---------------------------------------";
    qDebug() << "       INITIALISATION DU BOARD";
    qDebug() << "---------------------------------------";

    qDebug() << *game;
#endif

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
            game->board.boardPower[4] = E_POWER::voteToKill;
            game->board.boardPower[5] = E_POWER::noPower;
            break;

        case 7:
        case 8:
            game->board.boardPower[0] = E_POWER::noPower;
            game->board.boardPower[1] = E_POWER::checkRole;
            game->board.boardPower[2] = E_POWER::choosePresident;
            game->board.boardPower[3] = E_POWER::kill;
            game->board.boardPower[4] = E_POWER::voteToKill;
            game->board.boardPower[5] = E_POWER::noPower;
            break;

        case 9:
        case 10:
            game->board.boardPower[0] = E_POWER::checkRole;
            game->board.boardPower[1] = E_POWER::checkRole;
            game->board.boardPower[2] = E_POWER::choosePresident;
            game->board.boardPower[3] = E_POWER::kill;
            game->board.boardPower[4] = E_POWER::voteToKill;
            game->board.boardPower[5] = E_POWER::noPower;
            break;
    }

#ifndef LOG
    qDebug() << *game;

    qDebug() << "---------------------------------------";
    qDebug() << "       INITIALISATION TERMINEE";
    qDebug() << "---------------------------------------";
#endif
}

void initPile(S_GAME_STATUS* game)
{
    quint8 LCardsNotput = 11, FCardsNotput = 22;

    while(!game->pile.isEmpty())
        game->pile.removeLast();

    do
    {
        quint8 FCardPourcentage = static_cast<quint8>((static_cast<qreal>(FCardsNotput) / static_cast<qreal>(FCardsNotput + LCardsNotput)) * 100);
        quint8 pourcentage = static_cast<quint8>(QRandomGenerator::global()->bounded(100 + 1));

        if(pourcentage <= FCardPourcentage)
        {
            if(FCardsNotput != 0)
            {
                game->pile.append(E_CARD::facisteLaw);
                FCardsNotput--;
            }
        }
        else
        {
            if(LCardsNotput != 0)
            {
                game->pile.append(E_CARD::liberalLaw);
                LCardsNotput--;
            }
        }

    }while(LCardsNotput != 0 || FCardsNotput != 0);
}

void initPlayers(S_GAME_STATUS* game)
{
    quint8 president = static_cast<quint8>(QRandomGenerator::global()->bounded(game->players.size()));

    for(int i = 0; i < game->players.size(); i++)
    {
        game->players[i].power = E_POWER::noPower;
        game->players[i].role = E_ROLE::notAssigned;
        game->players[i].vote = E_VOTE::blank;
        game->players[i].electionRole = E_ELECTION_ROLE::none;
        game->players[i].status = E_PLAYER_STATUS::alive;

        if(game->players[i].identifier == president)
        {
            game->players[i].electionRole = E_ELECTION_ROLE::president;
        }
    }
}

void setNextPresident(S_GAME_STATUS* game)
{
    E_IDENTIFIER identifier = getPresidentIdentifier(*game);

    // Set vice Presdient to none.
    for(int i = 0; i < game->players.size(); i++)
    {
        if(game->players[i].electionRole == E_ELECTION_ROLE::vicePresident)
        {
            game->players[i].electionRole = E_ELECTION_ROLE::none;
            break;
        }
    }
    // Set new vice President.
    for(int i = 0; i < game->players.size(); i++)
    {
        if(game->players[i].electionRole == E_ELECTION_ROLE::president)
        {
            game->players[i].electionRole = E_ELECTION_ROLE::vicePresident;
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
            game->players[i].electionRole = E_ELECTION_ROLE::president;
        }
    }
}

void setNewPresident(S_GAME_STATUS* game)
{
    // Set vice Presdient to none.
    for(int i = 0; i < game->players.size(); i++)
    {
        if(game->players[i].electionRole == E_ELECTION_ROLE::vicePresident)
        {
            game->players[i].electionRole = E_ELECTION_ROLE::none;
            break;
        }
    }
    // Set new vice President.
    for(int i = 0; i < game->players.size(); i++)
    {
        if(game->players[i].electionRole == E_ELECTION_ROLE::president)
        {
            game->players[i].electionRole = E_ELECTION_ROLE::vicePresident;
            break;
        }
    }

   if(game->playerFocus >= game->players.size())
   {
       qDebug() << "erreur, le président ne peut être assigné car l'index n'est pas bon";
       game->players[0].electionRole = E_ELECTION_ROLE::president;
   }
   else
   {
       game->players[game->playerFocus].electionRole = E_ELECTION_ROLE::president;
   }
}

void setChancelor(S_GAME_STATUS* game, E_IDENTIFIER player)
{
    if(player == E_IDENTIFIER::ID_none)
    {
        // Set vice Chancelor to none.
        for(int i = 0; i < game->players.size(); i++)
        {
            if(game->players[i].electionRole == E_ELECTION_ROLE::viceChancelor)
            {
                game->players[i].electionRole = E_ELECTION_ROLE::none;
                break;
            }
        }
        // Set new vice Chancelor.
        for(int i = 0; i < game->players.size(); i++)
        {
            if(game->players[i].electionRole == E_ELECTION_ROLE::chancelor)
            {
                game->players[i].electionRole = E_ELECTION_ROLE::viceChancelor;
            }
        }
    }
    // Set Chancelor.
    if(player != E_IDENTIFIER::ID_none)
    {
        game->players[player].electionRole = E_ELECTION_ROLE::chancelor;
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
                case E_ROLE::faciste:
                    numberFacists++;
                    break;

                case E_ROLE::liberal:
                    numberLiberals++;
                    break;

                case E_ROLE::hitler:
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
                return E_ROLE::faciste;
            }
            if(numberLiberals < numberPlayers - 2)
            {
                return E_ROLE::liberal;
            }
            if(hitlerAssigned == 0)
            {
                return E_ROLE::hitler;
            }
            break;

        case 7:
        case 8:
            if(numberFacists < 2)
            {
                return E_ROLE::faciste;
            }
            if(numberLiberals < numberPlayers - 3)
            {
                return E_ROLE::liberal;
            }
            if(hitlerAssigned == 0)
            {
                return E_ROLE::hitler;
            }
            break;

        case 9:
        case 10:
            if(numberFacists < 3)
            {
                return E_ROLE::faciste;
            }
            if(numberLiberals < numberPlayers - 4)
            {
                return E_ROLE::liberal;
            }
            if(hitlerAssigned == 0)
            {
                return E_ROLE::hitler;
            }
            break;
    }

    return E_ROLE::notAssigned;
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

E_IDENTIFIER getPresidentIdentifier(S_GAME_STATUS game)
{
    for(const S_PLAYER &player : game.players)
    {
        if(player.electionRole == E_ELECTION_ROLE::president)
        {
            qDebug() << "pres id : " << player.identifier;
            return player.identifier;
        }
    }

    qDebug() << "erreur, aucun président trouvé";

    return E_IDENTIFIER::ID_none;
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

        case E_POWER::choosePresident:
            return "selection Président";

        case E_POWER::kill:
            return "execution";

        case E_POWER::voteToKill:
            return "execution par vote";
    }
}

QString getVoteString(E_VOTE vote)
{
    switch(vote)
    {
        case E_VOTE::blank:
            return "aucun";

        case E_VOTE::yah:
            return "yah";

        case E_VOTE::nein:
        return "nein";
    }
}

QString getRoleString(E_ROLE role)
{
    switch(role)
    {
        case E_ROLE::notAssigned:
            return "non assigné";

        case E_ROLE::faciste:
            return "facist";

        case E_ROLE::liberal:
            return "libéral";

        case E_ROLE::hitler:
            return "hitler";
    }
}

QString getCommandString(quint8 command)
{
    switch(command)
    {
        case CMD_TO_PLAYER_START_GAME:
            return "Js <- commencer la partie";

        case CMD_TO_SERVER_START_NEW_TURN:
            return "S <- commencer un nouveau tour";

        case CMD_TO_PLAYER_ELECT_CHANCELOR:
            return "Js <- commencer l'élection du Chancelier";

        case CMD_TO_SERVER_GIVE_CHANCELOR:
            return "S <- choix du Chancelier";

        case CMD_TO_PLAYER_START_VOTE:
            return "Js <- commencer le vote du Chancelier";

        case CMD_TO_SERVER_PLAYER_VOTED:
            return "S <- un joueur a voté";

        case CMD_TO_PLAYER_PLAYER_VOTED:
            return "Js <- un joueur a voté";

        case CMD_TO_PLAYER_PLAYER_VOTED_TO_KILL:
            return "Js <- un joueur a voté pour tuer";

        case CMD_TO_PLAYER_PRESIDENT_DRAW:
            return "Js <- le Président doit piocher trois lois";

        case CMD_TO_SERVER_PRESIDENT_DISCARDED:
            return "S <- le Président a défaussé un loi";

        case CMD_TO_PLAYER_CHANCELOR_DISCARD:
            return "Js <- le Chancelier doit défausser une loi";

        case CMD_TO_SERVER_CHANCELOR_DISCARDED:
            return "S <- le Chancelier a défaussé une loi";

        case CMD_TO_PLAYER_PUT_LAW_ON_BOARD:
            return "Js <- une loi a été votée";

        case CMD_TO_SERVER_END_TURN_OK:
            return "S <- fin du tour";

        case CMD_TO_SERVER_NEW_PRESIDENT:
            return "S <- choix du prochain Président";

        case CMD_TO_SERVER_KILL_PLAYER:
            return "S <- execution d'un joueur";

        case CMD_TO_SERVER_START_VOTE_TO_KILL:
            return "S <- début du vote pour l'execution";

        case CMD_TO_PLAYER_VOTE_TO_KILL:
            return "Js <- un joueur a voté pour l'execution";

        case CMD_TO_SERVER_PLAYER_VOTED_TO_KILL:
            return "S <- un joueur a voté pour l'execution";

        case CMD_TO_PLAYER_END_GAME:
            return "Js <- fin de la partie";

        case CMD_TO_SERVER_CHANGE_NAME:
            return "S <- changement de nom d'un joueur";

        case CMD_TO_PLAYER_INIT_COMMUNICATION:
            return "Js <- initialisation de la communication avec le serveur";

        case CMD_TO_PLAYER_SET_NEW_IDENTIFIER:
            return "Js <- changement de l'identifiant";

        case CMD_TO_PLAYER_NEW_CONNECTION:
            return "Js <- nouvelle connexion";

        case CMD_TO_PLAYER_PLAYER_JOINED:
            return "Js <- join la partie";

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
