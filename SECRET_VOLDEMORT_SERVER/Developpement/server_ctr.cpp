#include "server_ctr.h"
#include "ui_server_ctr.h"

Server_CTR::Server_CTR(QWidget *parent):
    QMainWindow(parent),
    mGame(S_GAME_STATUS()),
    mPlayerFocus(E_IDENTIFIER::ID_none),
    mTCP_API(nullptr),
    mTurnStateSaveMSG(S_GAME_STATUS()),
    mElectionCancelationCounter(0),
    mBoardStatusDisplay(S_GAMEBOARD_DISPLAYER()),
    ui(new Ui::Server_CTR)
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
}

Server_CTR::~Server_CTR()
{
    delete ui;
    delete mTCP_API;
}

void Server_CTR::gameState(S_MESSAGE MSG)
{
    refreshPlayerStatus(MSG);

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

        case CMD_TO_SERVER_START_VOTE_TO_KILL:
            STATE_powerStartVoteToKill(MSG);
            break;

        case CMD_TO_SERVER_PLAYER_VOTED_TO_KILL:
            STATE_powerKillPlayerVoteResult(MSG);
            break;

        case CMD_TO_SERVER_CHANGE_NAME:
            STATE_changeName(MSG);
            break;

        case CMD_TO_SERVER_JOIN_GAME:
            STATE_playerAskToJoinGame(MSG);
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

    setNextPresident(&mGame);
    setChancelor(&mGame, E_IDENTIFIER::ID_none);

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
qDebug() << "in";
    if(addPlayerVote(MSG.identifier))
    {qDebug() << "2";
        onPrint("Totalité des votes collectés");
        // Everyone voted.
        switch(getVoteResult())
        {
            case E_VOTE::yah:
                mElectionCancelationCounter = 0;
                if(mGame.players[mGame.playerFocus].role == E_ROLE::hitler && mGame.board.boardFaciste >= 3)
                {
                    onPrint("Fin de la partie : Hitler élu Chancelier");
                    mGame.endGame = E_END_GAME::hitlerElected;
                    mTCP_API->setCommand(CMD_TO_PLAYER_END_GAME);
                    QTimer::singleShot(2000, this, [&](){ mTCP_API->send_MSG(mGame); });
                }
                else
                {
                    setChancelor(&mGame, mGame.playerFocus);
                    mTCP_API->setCommand(CMD_TO_PLAYER_PRESIDENT_DRAW);
                    QTimer::singleShot(2000, this, [&](){ mTCP_API->send_MSG(mGame); });
                }
                break;

            case E_VOTE::nein:
                // The rule says that after three cancelations, a facist law is put on board.
                if(++mElectionCancelationCounter >= 3)
                {
                    mElectionCancelationCounter = 0;
                    STATE_threeVoteCanceled();
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
qDebug() << "out";
        // Remove player votes and focus.
        mGame.playerFocus = E_IDENTIFIER::ID_none;
        for(int i = 0; i < mGame.players.size(); i++)
        {
            if(mGame.players[i].vote == E_VOTE::yah || mGame.players[i].vote == E_VOTE::nein)
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
        case E_CARD::facisteLaw:
            mGame.board.boardFaciste++;
            // Check for power.
            mGame.players[getPresidentIdentifier(mGame)].power = mGame.board.boardPower[mGame.board.boardFaciste - 1];
            break;

        case E_CARD::liberalLaw:
            mGame.board.boardLiberal++;
            break;
    }

    // Check if 6 faciste laws have been put on board.
    if(mGame.board.boardFaciste >= 6)
    {
        onPrint("Fin de la partie : 6 lois facistes posées");
        mGame.endGame = E_END_GAME::facisteWon;
        mTCP_API->setCommand(CMD_TO_PLAYER_END_GAME);
        mTCP_API->send_MSG(mGame);
        return;
    }
    // Check if 5 liberal laws have been put on board.
    if(mGame.board.boardLiberal >= 5)
    {
        onPrint("Fin de la partie : 5 lois libérales posées");
        mGame.endGame = E_END_GAME::liberalWon;
        mTCP_API->setCommand(CMD_TO_PLAYER_END_GAME);
        mTCP_API->send_MSG(mGame);
        return;
    }

    mTCP_API->setCommand(CMD_TO_PLAYER_PUT_LAW_ON_BOARD);
    mTCP_API->send_MSG(mGame);
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
    if(mGame.players[MSG.gameStatus.playerFocus].role == E_ROLE::hitler)
    {
        mGame.endGame = E_END_GAME::hitlerKilled;
        mTCP_API->setCommand(CMD_TO_PLAYER_END_GAME);
        mTCP_API->send_MSG(mGame);
        return;
    }

    STATE_startTurn();
}

void Server_CTR::STATE_powerStartVoteToKill(S_MESSAGE MSG)
{
    onPrint("Pouvoir vote pour executer un joueur a commencé");

    mGame.playerFocus = MSG.gameStatus.playerFocus;
    mTCP_API->setCommand(CMD_TO_PLAYER_VOTE_TO_KILL);
    mTCP_API->send_MSG(mGame);
}

void Server_CTR::STATE_powerKillPlayerVoteResult(S_MESSAGE MSG)
{
    if(addPlayerVote(MSG.identifier))
    {
        onPrint("Pouvoir tous les votes ont été collectés");
        // Everyone voted.
        switch(getVoteResult())
        {
            case E_VOTE::yah:
                killPlayer(mGame.playerFocus);
                if(mGame.players[mGame.playerFocus].role == E_ROLE::hitler)
                {
                    onPrint("Fin de la partie : Hitler tué");
                    killPlayer(static_cast<E_IDENTIFIER>(mGame.playerFocus));
                    mGame.endGame = E_END_GAME::hitlerKilled;
                    mTCP_API->setCommand(CMD_TO_PLAYER_END_GAME);
                    QTimer::singleShot(2000, this, [&](){ mTCP_API->send_MSG(mGame); });
                }
                else
                {
                    killPlayer(mGame.playerFocus);
                    QTimer::singleShot(2000, this, SLOT(STATE_startTurn()));
                }
                break;

            case E_VOTE::nein:
                QTimer::singleShot(2000, this, SLOT(STATE_startTurn()));
                break;

            default:
                onPrint("erreur, le résultat d'un vote ne devrait pas être différent de yah ou nein");
                break;
        }

        // Remove votes and focus.
        mGame.playerFocus = E_IDENTIFIER::ID_none;
        for(int i = 0; i < mGame.players.size(); i++)
        {
            if(mGame.players[i].vote == E_VOTE::yah || mGame.players[i].vote == E_VOTE::nein)
            {
                mGame.players[i].vote = E_VOTE::blank;
            }
        }
        //QTimer::singleShot(5000, this, SLOT(onHidePlayerVotes()));
    }
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
        mGame.endGame = E_END_GAME::facisteWon;
        mTCP_API->setCommand(CMD_TO_PLAYER_END_GAME);
        mTCP_API->send_MSG(mGame);
        return;
    }

    // Check for power.
    mGame.players[getPresidentIdentifier(mGame)].power = mGame.board.boardPower[mGame.board.boardFaciste - 1];

    mTCP_API->setCommand(CMD_TO_PLAYER_PUT_LAW_ON_BOARD);
    QTimer::singleShot(2000, [&](){ mTCP_API->send_MSG(mGame); });
}

void Server_CTR::initGame()
{
    initPlayers(&mGame);
    initRole(&mGame);
    initBoard(&mGame);
    initPile(&mGame);
    mGame.endGame = E_END_GAME::notFinished;
    mGame.playerFocus = E_IDENTIFIER::ID_none;

    // clean log.
    mMessageLog = "";
    refreshGameStatusDisplay();
}

void Server_CTR::refreshPlayerStatus(S_MESSAGE MSG)
{
    if(MSG.command == CMD_TO_SERVER_JOIN_GAME)
        return;

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
            qDebug() << "n" << player.identifier;
            if(mGame.players[getPresidentIdentifier(mGame)].power == E_POWER::voteToKill)
            {
                mTCP_API->setCommand(CMD_TO_PLAYER_PLAYER_VOTED_TO_KILL);
                mTCP_API->send_MSG(mGame);
            }
            else
            {
                mTCP_API->setCommand(CMD_TO_PLAYER_PLAYER_VOTED);
                mTCP_API->send_MSG(mGame);
            }
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
qDebug() << "cheh4";
    return true;
}

E_VOTE Server_CTR::getVoteResult()
{
    quint8 yah = 0, nein = 0;

    for(const S_PLAYER &player : mGame.players)
    {
        if(player.vote == E_VOTE::yah)
        {
            yah++;
            continue;
        }
        if(player.vote == E_VOTE::nein)
        {
            nein++;
            continue;
        }

        if(player.status == E_PLAYER_STATUS::alive)
        {
            onPrint("Une erreur est survenue lors de la collecte des votes\n"
                    "Un vote différent de yah ou nein a été trouvé");
        }
    }

    if(yah >= nein)
    {
        return E_VOTE::yah;
    }
    else
    {
        return E_VOTE::nein;
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

    // Refresh game status.
    switch(mGame.endGame)
    {
        case E_END_GAME::notStarted:
            ui->edit_gameStatus->setText("partie non commencée");
            break;

        case E_END_GAME::notFinished:
            ui->edit_gameStatus->setText("partie en cours");
            break;

        default:
            ui->edit_gameStatus->setText("partie terminée");
            break;
    }

    if(mPlayerFocus != E_IDENTIFIER::ID_none)
    {
        ui->edit_playerFocus->setText(QString("%1").arg(mPlayerFocus));
    }

    // Pile refresh.
    for(E_CARD card : mGame.pile)
    {
        mBoardStatusDisplay.pile.append(new Widget_Card(this, card));
        ui->layoutPile->addWidget(mBoardStatusDisplay.pile.last());
    }

    // Boards refresh.
    for(int i = 0; i < mGame.board.boardFaciste; i++)
    {
        mBoardStatusDisplay.boardFaciste.append(new Widget_Card(this, E_CARD::facisteLaw));
        ui->layoutBoardFaciste->addWidget(mBoardStatusDisplay.boardFaciste.last());
    }
    for(int i = 0; i < mGame.board.boardLiberal; i++)
    {
        mBoardStatusDisplay.boardLiberal.append(new Widget_Card(this, E_CARD::liberalLaw));
        ui->layoutBoardLiberal->addWidget(mBoardStatusDisplay.boardLiberal.last());
    }

    // Players refresh.
    for(const S_PLAYER &player : mGame.players)
    {
        mBoardStatusDisplay.players.append(new Widget_Player(this, player));
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
    if(mGame.players.size() < 5)
    {
        onPrint("Nombre de joueurs insuffisant pour lancer une partie\n"
              "\tNombre de joueurs actuellement connectés : " + QString(mGame.players.size()));
        for(const S_PLAYER &player : mGame.players)
              onPrint("\t- " + player.name);
        return;
    }

    initGame();
    mTurnStateSaveMSG = mGame;

    mTCP_API->setCommand(CMD_TO_PLAYER_START_GAME);
    mTCP_API->send_MSG(mGame);
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

    // Get number of players.
    quint8 numberPlayers = 0;
    for(const S_PLAYER &player : mGame.players)
    {
        numberPlayers++;
    }

    // Increment President if necessary.
    E_IDENTIFIER president = E_IDENTIFIER::ID_none, chancelor = E_IDENTIFIER::ID_none;
    if(mGame.players[identifier].electionRole == E_ELECTION_ROLE::president)
    {
        president = identifier;

        do
        {
            if(++president > mGame.players.size() - 1)
                president = E_IDENTIFIER::ID_player1;

        }while(mGame.players[president].status != E_PLAYER_STATUS::alive);

        mGame.players[president].electionRole = E_ELECTION_ROLE::president;
    }

    // Increment Chancelor if necessary.
    if(mGame.players[identifier].electionRole == E_ELECTION_ROLE::chancelor)
    {
        chancelor = identifier;

        do
        {
            if(++chancelor > mGame.players.size() - 1)
                chancelor = E_IDENTIFIER::ID_player1;

        }while(mGame.players[chancelor].status != E_PLAYER_STATUS::alive);

        mGame.players[chancelor].electionRole = E_ELECTION_ROLE::chancelor;
    }

    onPrint(mGame.players[identifier].name + " a quitté la partie");
    mGame.players.removeAt(identifier);

    refreshPlayerIdentifier();
}

void Server_CTR::onHidePlayerVotes()
{
    for(int i = 0; i < mGame.players.size(); i++)
    {
        if(mGame.players[i].vote == E_VOTE::yah || mGame.players[i].vote == E_VOTE::nein)
        {
            mGame.players[i].vote = E_VOTE::blank;
        }
    }
}

void Server_CTR::onPrint(QString message)
{
    mMessageLog += message + "\n";
    ui->log->setReadOnly(true);
    ui->log->setText(mMessageLog);
}

void Server_CTR::on_buttonRestartTurn_clicked()
{
    mGame = mTurnStateSaveMSG;

    // Remove potential player focus.
    mGame.playerFocus = E_IDENTIFIER::ID_none;
    // Remove potentiel power.
    for(int i = 0; i < mGame.players.size(); i++)
    {
        mGame.players[i].power = E_POWER::noPower;
    }
    // Remove player Chancelor status.
    for(int i = 0; i < mGame.players.size(); i++)
    {
        if(mGame.players[i].electionRole == E_ELECTION_ROLE::chancelor)
        {
            mGame.players[i].electionRole = E_ELECTION_ROLE::none;
        }
    }

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
    // Remove player Chancelor status.
    for(int i = 0; i < mGame.players.size(); i++)
    {
        if(mGame.players[i].electionRole == E_ELECTION_ROLE::chancelor)
        {
            mGame.players[i].electionRole = E_ELECTION_ROLE::none;
        }
    }

    refreshGameStatusDisplay();

    qDebug() << mGame;

    mTCP_API->setCommand(CMD_TO_PLAYER_ELECT_CHANCELOR);
    mTCP_API->send_MSG(mGame);
}
