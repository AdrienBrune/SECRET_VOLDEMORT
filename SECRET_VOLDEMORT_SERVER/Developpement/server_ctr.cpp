#include "server_ctr.h"
#include "ui_server_ctr.h"

//#define DEBUG
 
char version[15+1];

QString gameStatusLabel[] = {
    "en attente",
    "en cours",
    "terminée"
};
enum
{
    WAITING = 0,
    IN_PROGRESS,
    FINISHED

};

Server_CTR::Server_CTR(QWidget *parent):
    QMainWindow(parent),
    mGame(S_GAME_STATUS()),
    mPlayerFocus(E_IDENTIFIER::ID_none),
    mTCP_API(nullptr),
    mTurnStateSaveMSG(S_GAME_STATUS()),
    mSaveLastPresident(E_IDENTIFIER::ID_none),
    mRemoveAllRolesForNextTurn(false),
    mBoardStatusDisplay(S_GAMEBOARD_DISPLAYER()),
    mAntiSpoil(true),
    mExpertMode(false),
    ui(new Ui::Secret_Voldemort_Serveur)
{
    ui->setupUi(this);

    mTCP_API = new TCP_API();
    mTCP_API->mGame = &mGame;
    connect(mTCP_API, SIGNAL(sig_playerConnection(QTcpSocket*)), this, SLOT(onAddPlayer(QTcpSocket*)));
    connect(mTCP_API, SIGNAL(sig_playerDisconnection(QTcpSocket*)), this, SLOT(onRemovePlayer(QTcpSocket*)));
    connect(mTCP_API, SIGNAL(sig_MSG_received(S_MESSAGE)), this, SLOT(gameState(S_MESSAGE)));
    connect(mTCP_API, SIGNAL(sig_print(QString)), this, SLOT(onPrint(QString)));
    connect(ui->buttonStart, SIGNAL(clicked()), this, SLOT(onStartGame()));

    mTCP_API->startServer();

    // Initialisation game status.
    mGame.endGame = E_END_GAME::notStarted;
    mGame.board.boardFaciste = 0;
    mGame.board.boardLiberal = 0;
    for(int i = 0; i < 6; i++)
    {
        mGame.board.boardPower[i] = E_POWER::noPower;
    }
    mGame.playerFocus = E_IDENTIFIER::ID_none;
    generateNewPile(&mGame.pile);
    mGame.vetoPower = false;

    ui->version->setText(version);

}

Server_CTR::~Server_CTR()
{
    delete ui;
    delete mTCP_API;
}

void Server_CTR::gameState(S_MESSAGE MSG)
{
    refreshPlayerStatus(MSG);

    // Shuffle pile if necessary.
    qDebug() << "pile size : " << mGame.pile.size();
    if(mGame.pile.size() <= 3)
    {
        generateNewPile(&mGame.pile);
        qDebug() << "shuffle : " << mGame.pile.size();
    }

    printGame(mGame);

    // State machine
    switch(MSG.command)
    {
        case CMD_TO_SERVER_START_NEW_TURN:
        case CMD_TO_SERVER_END_TURN_OK:
            // Save game turn status.
            mTurnStateSaveMSG = mGame;
            STATE_startTurn();
            break;

        case CMD_TO_SERVER_GIVE_CHANCELOR:
            STATE_startChancelorVote(MSG);
            break;

        case CMD_TO_SERVER_PLAYER_VOTED:
            STATE_chancelorVoteResult(MSG);
            break;

        case CMD_TO_SERVER_PRESIDENT_DISCARDED:
            STATE_presidentDiscarded(MSG);
            break;

        case CMD_TO_SERVER_CHANCELOR_DISCARDED:
            STATE_chancelorDiscarded(MSG);
            STATE_putLawOnBoard(MSG);
            break;

        case CMD_TO_SERVER_NEW_PRESIDENT:
            STATE_powerNewPresident(MSG);
            break;

        case CMD_TO_SERVER_KILL_PLAYER:
            STATE_powerKillPlayer(MSG);
            break;

        case CMD_TO_SERVER_DIRECTOR_ASKED_VETO:
            mTCP_API->setCommand(CMD_TO_PLAYER_ASK_MINISTER_TO_VETO);
            mTCP_API->send_MSG(mGame);
            break;

        case CMD_TO_SERVER_MINISTER_VETO_REPLY:
            STATE_sendMinisterVetoReply(MSG);
            break;

        case CMD_TO_SERVER_CHANGE_NAME:
            STATE_changeName(MSG);
            break;

        case CMD_TO_SERVER_JOIN_GAME:
            STATE_playerAskToJoinGame(MSG);
            mTurnStateSaveMSG.players = mGame.players;
            break;

        default:
            printf("erreur, commande non reconnue");
            break;
    }
    refreshGameStatusDisplay();
}

void Server_CTR::STATE_startTurn()
{
    onPrint("Début du tour");

    // Remove potential player focus.
    mGame.playerFocus = E_IDENTIFIER::ID_none;
    // Remove potentiel power.
    for(int i = 0; i < mGame.players.size(); i++)
    {
        mGame.players[i].power = E_POWER::noPower;
    }

    if(mSaveLastPresident != E_IDENTIFIER::ID_none)
    {
        // Last turn was a special turn.
        E_IDENTIFIER specialPresident = E_IDENTIFIER::ID_none;
        for(int i = 0; i < mGame.players.size(); i++)
        {
            if(mGame.players[i].electionRole == E_ELECTION_ROLE::minister)
            {
                specialPresident = static_cast<E_IDENTIFIER>(i);
                break;
            }
        }

        setNextPresidentAfterSpecialTurn(mSaveLastPresident, specialPresident, &mGame);

        setChancelor(&mGame, E_IDENTIFIER::ID_none);
        mSaveLastPresident = E_IDENTIFIER::ID_none;
    }
    else
    {
        // Last turn was a normal turn.
        setNextPresident(&mGame);
        setChancelor(&mGame, E_IDENTIFIER::ID_none);
    }

    // Remove all roles if election tracker has been triggered
    if(mRemoveAllRolesForNextTurn)
    {
        for(int index=0; index < mGame.players.size(); index++)
        {
            if(mGame.players[index].electionRole != E_ELECTION_ROLE::minister)
            {
                mGame.players[index].electionRole = E_ELECTION_ROLE::none;
            }
        }
    }
    mRemoveAllRolesForNextTurn = false;

    mTCP_API->setCommand(CMD_TO_PLAYER_ELECT_CHANCELOR);
    mTCP_API->send_MSG(mGame);
}

void Server_CTR::STATE_startChancelorVote(S_MESSAGE MSG)
{
    onPrint("Election du chancelier");

    mGame.playerFocus = MSG.gameStatus.playerFocus;

    // Reset player votes.
    for(int i = 0; i < mGame.players.size(); i++)
    {
        mGame.players[i].vote = E_VOTE::blank;
    }

    mTCP_API->setCommand(CMD_TO_PLAYER_START_VOTE);
    mTCP_API->send_MSG(mGame);
}

void Server_CTR::STATE_chancelorVoteResult(S_MESSAGE MSG)
{
    if(addPlayerVote(MSG.identifier))
    {
        onPrint("Totalité des votes collectés");
        // Everyone voted.
        switch(getVoteResult())
        {
            case E_VOTE::lumos:
                if(mGame.players[mGame.playerFocus].role == E_ROLE::voldemort && mGame.board.boardFaciste >= 3)
                {
                    onPrint("Fin de la partie : Hitler élu Chancelier");
                    mGame.endGame = E_END_GAME::voldemortElected;
                    mTCP_API->setCommand(CMD_TO_PLAYER_END_GAME);
                    QTimer::singleShot(2000, this, [&](){ mTCP_API->send_MSG(mGame); });
                }
                else
                {
                    // Set the cahncelor.
                    setChancelor(&mGame, mGame.playerFocus);
                    mTCP_API->setCommand(CMD_TO_PLAYER_PRESIDENT_DRAW);
                    QTimer::singleShot(2000, this, [&](){ mTCP_API->send_MSG(mGame); });
                }
                break;

            case E_VOTE::nox:
                // The rule says that after three cancelations, the top card of the pile is put on board and all roles are reset.
                if(++mGame.electionTracker >= 3)
                {
                    mRemoveAllRolesForNextTurn = true;
                    STATE_putLawOnBoard(MSG);
                }
                else
                {
                    QTimer::singleShot(2000, this, SLOT(STATE_startTurn()));
                }
                break;

            default:
                onPrint("erreur, le résultat d'un vote ne devrait pas être différent de yah ou nein");
                break;
        }

        // Remove player votes and focus.
        mGame.playerFocus = E_IDENTIFIER::ID_none;
        for(int i = 0; i < mGame.players.size(); i++)
        {
            if(mGame.players[i].vote == E_VOTE::lumos || mGame.players[i].vote == E_VOTE::nox)
            {
                mGame.players[i].vote = E_VOTE::blank;
            }
        }
    }
}

void Server_CTR::STATE_presidentDiscarded(S_MESSAGE MSG)
{
    onPrint("Le président a défaussé une loi");

    copyNewPile(mGame.pile, MSG.gameStatus.pile);

    mTCP_API->setCommand(CMD_TO_PLAYER_CHANCELOR_DISCARD);
    mTCP_API->send_MSG(mGame);
}

void Server_CTR::STATE_chancelorDiscarded(S_MESSAGE MSG)
{
    onPrint("Le Chancelier a défaussé une loi");

    copyNewPile(mGame.pile, MSG.gameStatus.pile);
}

void Server_CTR::STATE_putLawOnBoard(S_MESSAGE)
{
    onPrint("La loi est posé sur le plateau");

    E_CARD card = mGame.pile.takeLast();

    switch(card)
    {
        case E_CARD::deathEatersLaw:
            mGame.board.boardFaciste++;
            // Check for power.
            mGame.players[getPresidentIdentifier(mGame)].power = mGame.board.boardPower[mGame.board.boardFaciste - 1];
            if(mGame.board.boardPower[mGame.board.boardFaciste - 1] == E_POWER::chooseMinister)
            {
                mSaveLastPresident = getPresidentIdentifier(mGame);
            }
            // If power check pile enable, shuffle pile if its size is inferior to 3 cards.
            if(mGame.board.boardPower[mGame.board.boardFaciste - 1] == E_POWER::checkPile)
            {
                if(mGame.pile.size() <= 3)
                {
                    generateNewPile(&mGame.pile);
                }
            }
            break;

        case E_CARD::phenixOrderLaw:
            mGame.board.boardLiberal++;
            break;
    }

    // Check to enable veto power.
    if(mGame.board.boardFaciste >= 5)
    {
        mGame.vetoPower = true;
    }

    // Check if 6 faciste laws have been put on board.
    if(mGame.board.boardFaciste >= 6)
    {
        onPrint("Fin de la partie : 6 lois facistes posées");
        mGame.endGame = E_END_GAME::deathEatersWon;
        mTCP_API->setCommand(CMD_TO_PLAYER_END_GAME);
        mTCP_API->send_MSG(mGame);
        return;
    }
    // Check if 5 liberal laws have been put on board.
    if(mGame.board.boardLiberal >= 5)
    {
        onPrint("Fin de la partie : 5 lois libérales posées");
        mGame.endGame = E_END_GAME::phenixOrderWon;
        mTCP_API->setCommand(CMD_TO_PLAYER_END_GAME);
        mTCP_API->send_MSG(mGame);
        return;
    }

    mTCP_API->setCommand(CMD_TO_PLAYER_PUT_LAW_ON_BOARD);
    mTCP_API->send_MSG(mGame);

    // Reset election tracker.
    mGame.electionTracker = 0;
}

void Server_CTR::STATE_powerNewPresident(S_MESSAGE MSG)
{
    onPrint("Pouvoir nouveau président utilisé");

    mGame.playerFocus = MSG.gameStatus.playerFocus;
    setNewPresident(&mGame);

    // Remove potential player focus.
    mGame.playerFocus = E_IDENTIFIER::ID_none;
    // Remove potentiel power.
    for(int i = 0; i < mGame.players.size(); i++)
    {
        mGame.players[i].power = E_POWER::noPower;
    }

    setChancelor(&mGame, E_IDENTIFIER::ID_none);

    mTCP_API->setCommand(CMD_TO_PLAYER_ELECT_CHANCELOR);
    mTCP_API->send_MSG(mGame);
}

void Server_CTR::STATE_powerKillPlayer(S_MESSAGE MSG)
{
    onPrint("Pouvoir execution d'un joueur utilisé");

    killPlayer(MSG.gameStatus.playerFocus);

    // If the player killed is Hitler, game finished.
    if(mGame.players[MSG.gameStatus.playerFocus].role == E_ROLE::voldemort)
    {
        mGame.endGame = E_END_GAME::voldemortKilled;
        mTCP_API->setCommand(CMD_TO_PLAYER_END_GAME);
        mTCP_API->send_MSG(mGame);
        return;
    }

    STATE_startTurn();
}

void Server_CTR::STATE_changeName(S_MESSAGE MSG)
{
    mGame.players[MSG.identifier].name = MSG.gameStatus.players[MSG.identifier].name;

    mTCP_API->setCommand(CMD_TO_PLAYER_NEW_CONNECTION);
    mTCP_API->send_MSG(mGame);
}

void Server_CTR::STATE_playerAskToJoinGame(S_MESSAGE MSG)
{
    if(mGame.players[MSG.identifier].status != E_PLAYER_STATUS::notPlaying)
        return;

    if(mGame.players.size() < 5)
        return;

    mGame.players[MSG.identifier].electionRole = E_ELECTION_ROLE::none;
    mGame.players[MSG.identifier].power = E_POWER::noPower;
    mGame.players[MSG.identifier].role = getMissingRole(mGame);
    mGame.players[MSG.identifier].roleName = getMissingRoleName(MSG.identifier, mGame);
    mGame.players[MSG.identifier].vote = E_VOTE::blank;

    // Put player in game.
    if(mGame.players[MSG.identifier].role != E_ROLE::notAssigned)
        mGame.players[MSG.identifier].status = E_PLAYER_STATUS::alive;

    mTCP_API->setCommand(CMD_TO_PLAYER_PLAYER_JOINED);
    mTCP_API->send_MSG(mGame);
}

void Server_CTR::STATE_threeVoteCanceled()
{
    // Check if 6 faciste laws have been put on board.
    if(++mGame.board.boardFaciste >= 6)
    {
        onPrint("Fin de la partie : 6 lois facistes posées");
        mGame.endGame = E_END_GAME::deathEatersWon;
        mTCP_API->setCommand(CMD_TO_PLAYER_END_GAME);
        mTCP_API->send_MSG(mGame);
        return;
    }

    // Check for power.
    mGame.players[getPresidentIdentifier(mGame)].power = mGame.board.boardPower[mGame.board.boardFaciste - 1];

    mTCP_API->setCommand(CMD_TO_PLAYER_PUT_LAW_ON_BOARD);
    QTimer::singleShot(2000, [&](){ mTCP_API->send_MSG(mGame); });
}

void Server_CTR::STATE_sendMinisterVetoReply(S_MESSAGE MSG)
{
    if(MSG.garbage == 1)
    {
        // Minister accepted.

        // Remove the two cards drawn by Director.
        mGame.pile.removeLast();
        mGame.pile.removeLast();

        if(++mGame.electionTracker >= 3)
        {
            mRemoveAllRolesForNextTurn = true;
            STATE_putLawOnBoard(MSG);
        }
        else
        {
            STATE_startTurn();
        }
        return;
    }
    else
    {
        // Minister refused.
        mTCP_API->setCommand(CMD_TO_PLAYER_VETO_RESULT);
        mTCP_API->send_MSG(mGame);
    }
}

void Server_CTR::initGame()
{
#ifndef LOG
    qDebug() << "---------------------------------------";
    qDebug() << "         DEBUT INITIALISATION";
    qDebug() << "---------------------------------------";
    printGame(mGame);
#endif

    initPlayers(&mGame);
    initRole(&mGame);
    initBoard(&mGame);
    generateNewPile(&mGame.pile);
    mGame.endGame = E_END_GAME::notFinished;
    mGame.playerFocus = E_IDENTIFIER::ID_none;
    mGame.electionTracker = 0;
    mGame.vetoPower = false;

    // clean log.
    mMessageLog = "";
    refreshGameStatusDisplay();

#ifndef LOG
    printGame(mGame);
    qDebug() << "---------------------------------------";
    qDebug() << "       INITIALISATION TERMINEE";
    qDebug() << "---------------------------------------";
#endif
}

void Server_CTR::initDummyGame()
{
    while(!mGame.players.isEmpty())
        mGame.players.removeLast();

    for(int i = 0; i < 7; i++)
    {
        S_PLAYER player = { E_PLAYER_STATUS::alive, nullptr, static_cast<E_IDENTIFIER>(i), QString("Player_%1").arg(i), E_ROLE::notAssigned, E_ROLE_NAME::noOne, E_ELECTION_ROLE::none, E_VOTE::blank, E_POWER::noPower};
        mGame.players.append(player);
    }

    initPlayers(&mGame);
    initRole(&mGame);
    initBoard(&mGame);
    mGame.board.boardFaciste = 6;
    mGame.board.boardLiberal = 5;
    generateNewPile(&mGame.pile);
    mGame.endGame = E_END_GAME::notFinished;
    mGame.playerFocus = E_IDENTIFIER::ID_player2;
    mGame.electionTracker = 1;
    mGame.vetoPower = true;

    // clean log.
    mMessageLog = "";
    refreshGameStatusDisplay();
}

void Server_CTR::refreshPlayerStatus(S_MESSAGE MSG)
{
    if(MSG.command == CMD_TO_SERVER_JOIN_GAME)
        return;

    // Get and set new game status from sender.
    for(int i = 0; i < mGame.players.size(); i++)
    {
        if(mGame.players[i].identifier == MSG.identifier)
        {
            QTcpSocket* save = mGame.players[i].socket;
            mGame.players[i] = MSG.gameStatus.players[MSG.identifier];
            mGame.players[i].socket = save;
        }
    }
}

bool Server_CTR::addPlayerVote(E_IDENTIFIER identifier)
{
    onPrint(getVoteString(mGame.players[identifier].vote));

    // Deliver vote to other players.
    for(const S_PLAYER &player : mGame.players)
    {
        if(player.identifier == identifier)
        {
            mTCP_API->setCommand(CMD_TO_PLAYER_PLAYER_VOTED);
            mTCP_API->send_MSG(mGame);
        }
    }

    // Return false if someone didn't vote.
    for(const S_PLAYER &player : mGame.players)
    {
        if(player.vote == E_VOTE::blank && player.status == E_PLAYER_STATUS::alive)
        {
            return false;
        }
    }
    return true;
}

E_VOTE Server_CTR::getVoteResult()
{
    quint8 lumos = 0, nox = 0;

    for(const S_PLAYER &player : mGame.players)
    {
        if(player.vote == E_VOTE::lumos)
        {
            lumos++;
            continue;
        }
        if(player.vote == E_VOTE::nox)
        {
            nox++;
            continue;
        }

        if(player.status == E_PLAYER_STATUS::alive)
        {
            onPrint("Une erreur est survenue lors de la collecte des votes\n"
                    "Un vote différent de yah ou nein a été trouvé");
        }
    }

    if(lumos > nox)
    {
        return E_VOTE::lumos;
    }
    else
    {
        return E_VOTE::nox;
    }
}

void Server_CTR::killPlayer(E_IDENTIFIER identifier)
{
    mGame.players[identifier].status = E_PLAYER_STATUS::dead;
}

void Server_CTR::refreshPlayerIdentifier()
{
    for(int i = 0; i < mGame.players.size(); i++)
    {
        mGame.players[i].identifier = static_cast<E_IDENTIFIER>(i);
        S_MESSAGE MSG = { mGame.players[i].identifier, i, CMD_TO_PLAYER_SET_NEW_IDENTIFIER, mGame };
        mTCP_API->send_MSG(MSG, mGame.players[i].socket);
    }
}

void Server_CTR::refreshGameStatusDisplay()
{
    // Remove widgets display.
    while(!mBoardStatusDisplay.pile.isEmpty())
        delete mBoardStatusDisplay.pile.takeLast();

    while(!mBoardStatusDisplay.boardFaciste.isEmpty())
        delete mBoardStatusDisplay.boardFaciste.takeLast();

    while(!mBoardStatusDisplay.boardLiberal.isEmpty())
        delete mBoardStatusDisplay.boardLiberal.takeLast();

    while(!mBoardStatusDisplay.players.isEmpty())
        delete mBoardStatusDisplay.players.takeLast();

    if(mBoardStatusDisplay.boardMangemort)
    {
        ui->layout_boardMangemort->removeWidget(mBoardStatusDisplay.boardMangemort);
        delete mBoardStatusDisplay.boardMangemort;

    }
    if(mBoardStatusDisplay.boardPhenixOrder)
    {
        ui->layout_boardPhenixOrder->removeWidget(mBoardStatusDisplay.boardPhenixOrder);
        delete mBoardStatusDisplay.boardPhenixOrder;
    }

    // Refresh game status.
    switch(mGame.endGame)
    {
        case E_END_GAME::notStarted:
            ui->input_gameStatus->setText(gameStatusLabel[WAITING]);
            break;

        case E_END_GAME::notFinished:
            ui->input_gameStatus->setText(gameStatusLabel[IN_PROGRESS]);
            break;

        default:
            ui->input_gameStatus->setText(gameStatusLabel[FINISHED]);
            break;
    }

    // Pile refresh.
    for(E_CARD card : mGame.pile)
    {
        mBoardStatusDisplay.pile.prepend(new Widget_Card(this, card, &mAntiSpoil));
    }
    for(int i = 0; i < mGame.pile.size(); i++)
    {
        ui->layoutPile->addWidget(mBoardStatusDisplay.pile[i]);
    }

    mBoardStatusDisplay.boardMangemort = new Widget_Board(this, &mGame, Widget_Board::E_BOARD_TYPE::Mangemort);
    mBoardStatusDisplay.boardPhenixOrder = new Widget_Board(this, &mGame, Widget_Board::E_BOARD_TYPE::PhenixOrder);

    ui->layout_boardMangemort->addWidget(mBoardStatusDisplay.boardMangemort);
    ui->layout_boardPhenixOrder->addWidget(mBoardStatusDisplay.boardPhenixOrder);


    // Players refresh.
    for(const S_PLAYER &player : mGame.players)
    {
        mBoardStatusDisplay.players.append(new Widget_Player(this, player, &mGame, &mAntiSpoil, &mExpertMode));
        ui->layoutPlayers->addWidget(mBoardStatusDisplay.players.last());
    }
}

void Server_CTR::copyGameStatusDisplayedInto(S_GAME_STATUS* game)
{
    // Check if game status is not empty.
    if(mBoardStatusDisplay.pile.isEmpty())
        return;

    if(mBoardStatusDisplay.players.isEmpty())
        return;

    // Set player focus.
    game->playerFocus = ID_none;

    // Set game status.
    game->endGame = E_END_GAME::notFinished;

    // Empty and copy pile.
    while(!game->pile.isEmpty())
        game->pile.removeLast();
    for(Widget_Card* card : mBoardStatusDisplay.pile)
    {
        game->pile.append(card->getCard());
    }

    // Empty and copy players.
    while(!game->players.isEmpty())
        game->players.removeLast();
    for(Widget_Player* player : mBoardStatusDisplay.players)
    {
        game->players.append(player->getPlayer());
    }

    game->board.boardFaciste = static_cast<quint8>(mBoardStatusDisplay.boardFaciste.size());
    game->board.boardLiberal = static_cast<quint8>(mBoardStatusDisplay.boardLiberal.size());
}


void Server_CTR::onStartGame()
{
#ifdef DEBUG
    initDummyGame();
#else
    if(mGame.players.size() < 5)
    {
        for(const S_PLAYER &player : mGame.players)
              onPrint("\t+ " + player.name);
        onPrint(QString("Nombre de joueurs insuffisant pour lancer une partie\n"
                        "Nombre de joueurs actuellement connectés : %1").arg(mGame.players.size()));

        return;
    }
    ui->buttonStart->setEnabled(false);

    initGame();
    mTurnStateSaveMSG = mGame;

    mTCP_API->setCommand(CMD_TO_PLAYER_START_GAME);
    mTCP_API->send_MSG(mGame);

    QTimer::singleShot(5000, this, [&]{ui->buttonStart->setEnabled(true);});
#endif
}

void Server_CTR::onAddPlayer(QTcpSocket* socket)
{
    S_PLAYER player = { E_PLAYER_STATUS::notPlaying, socket, E_IDENTIFIER::ID_none, "", E_ROLE::notAssigned, E_ROLE_NAME::noOne, E_ELECTION_ROLE::none, E_VOTE::blank, E_POWER::noPower};
    player.identifier = getAvailableIdentifier(&mGame);

    if(player.identifier == E_IDENTIFIER::ID_none)
    {
        onPrint("Un joueur a tenté de se connecter mais la partie est déjà pleine");
        return;
    }

    mGame.players.append(player);
    mTurnStateSaveMSG.players = mGame.players;

    onPrint("nouveau joueur dans la partie");

    S_MESSAGE MSG = { player.identifier, 0, CMD_TO_PLAYER_INIT_COMMUNICATION, mGame };
    mTCP_API->send_MSG(MSG, socket);
}

void Server_CTR::onRemovePlayer(QTcpSocket* socket)
{
    E_IDENTIFIER identifier;

    identifier = getPlayerIdentifier(mGame.players, socket);
    if(identifier == E_IDENTIFIER::ID_none)
    {
        onPrint("joueur a retirer de la partie non trouvé");
        return;
    }

    // Increment Minister if necessary.
    E_IDENTIFIER minister = E_IDENTIFIER::ID_none, director = E_IDENTIFIER::ID_none;
    if(mGame.players[identifier].electionRole == E_ELECTION_ROLE::minister)
    {
        minister = identifier;

        do
        {
            if(++minister > mGame.players.size() - 1)
                minister = E_IDENTIFIER::ID_player1;

        }while(mGame.players[minister].status != E_PLAYER_STATUS::alive);

        mGame.players[minister].electionRole = E_ELECTION_ROLE::minister;
    }

    // Increment Director if necessary.
    if(mGame.players[identifier].electionRole == E_ELECTION_ROLE::director)
    {
        director = identifier;

        do
        {
            if(++director > mGame.players.size() - 1)
                director = E_IDENTIFIER::ID_player1;

        }while(mGame.players[director].status != E_PLAYER_STATUS::alive);

        mGame.players[director].electionRole = E_ELECTION_ROLE::director;
    }

    onPrint(mGame.players[identifier].name + " a quitté la partie");
    mGame.players.removeAt(identifier);

    refreshPlayerIdentifier();
}

void Server_CTR::onHidePlayerVotes()
{
    for(int i = 0; i < mGame.players.size(); i++)
    {
        if(mGame.players[i].vote == E_VOTE::lumos || mGame.players[i].vote == E_VOTE::nox)
        {
            mGame.players[i].vote = E_VOTE::blank;
        }
    }
}

void Server_CTR::onPrint(QString message)
{
    mMessageLog = QString("%1\n%2").arg(message, mMessageLog);
    ui->log->setText(mMessageLog);
}

void Server_CTR::on_buttonRestartTurn_clicked()
{
    mGame = mTurnStateSaveMSG;

    // Remove potential player focus.
    mGame.playerFocus = E_IDENTIFIER::ID_none;
    // Remove potentiel power/vote.
    for(int i = 0; i < mGame.players.size(); i++)
    {
        mGame.players[i].power = E_POWER::noPower;
        mGame.players[i].vote = E_VOTE::blank;
    }
    // Remove player Director status.
    for(int i = 0; i < mGame.players.size(); i++)
    {
        if(mGame.players[i].electionRole == E_ELECTION_ROLE::director)
        {
            mGame.players[i].electionRole = E_ELECTION_ROLE::none;
        }
    }

    bool MinisterFound = false;
    for(const S_PLAYER & player : mGame.players)
    {
        if(player.electionRole == E_ELECTION_ROLE::minister)
            MinisterFound = true;
    }

    if(MinisterFound == false)
        mGame.players[0].electionRole = E_ELECTION_ROLE::minister;

    refreshGameStatusDisplay();

    mTCP_API->setCommand(CMD_TO_PLAYER_ELECT_CHANCELOR);
    mTCP_API->send_MSG(mGame);
}

void Server_CTR::on_buttonStartTurnWithInputs_clicked()
{
    copyGameStatusDisplayedInto(&mGame);

    // Remove potential player focus.
    mGame.playerFocus = E_IDENTIFIER::ID_none;
    // Remove potentiel power.
    for(int i = 0; i < mGame.players.size(); i++)
    {
        mGame.players[i].power = E_POWER::noPower;
    }
    // Remove player Director status.
    for(int i = 0; i < mGame.players.size(); i++)
    {
        if(mGame.players[i].electionRole == E_ELECTION_ROLE::director)
        {
            mGame.players[i].electionRole = E_ELECTION_ROLE::none;
        }
    }

    refreshGameStatusDisplay();

    mTCP_API->setCommand(CMD_TO_PLAYER_ELECT_CHANCELOR);
    mTCP_API->send_MSG(mGame);
}

void Server_CTR::on_buttonAntiSpoil_stateChanged(int toggle)
{
    mAntiSpoil = toggle;
    refreshGameStatusDisplay();
}

void Server_CTR::on_buttonExpertMode_stateChanged(int toggle)
{
    mExpertMode = toggle;
    refreshGameStatusDisplay();
}

void Server_CTR::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(QBrush(COLOR_DARK));
    painter.drawRect(QRect(0,0,width(),height()));
}
