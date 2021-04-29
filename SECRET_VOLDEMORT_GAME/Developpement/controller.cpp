#include "controller.h"
#include "ui_controller.h"
#include <QFontDatabase>
#include <QMessageBox>
#include <QCursor>
#include <QTimer>

QString g_RoleNames[] = {
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
    ""
};

Controller::Controller(QMainWindow *parent):
    QMainWindow(parent),
    mHandlerVotes(S_VOTE_HANDLER()),
    mParameters({true, true, true}),
    mMSG(S_MESSAGE()),
    ui(new Ui::Controller)
{
    ui->setupUi(this);
    ui->toolBar->setMovable(false);
    ui->toolBar->setStyleSheet("QToolBar{border-bottom:3px solid black; background-color:#191919;}"
                               "QToolButton{border:no border;spacing:0px;}"
                               "QToolButton::hover{background-color:#10202020;}"
                               );
    hide();

    initDummyGameStatus();

    QFontDatabase::addApplicationFont(":/fonts/OLDGATEL.ttf");
    QFontDatabase::addApplicationFont(":/fonts/HyliaSerif.ttf");
    QFontDatabase::addApplicationFont(":/fonts/BELL.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Germania.ttf");

    mHandlerVotes.t_displayer = new QTimer(this);
    connect(mHandlerVotes.t_displayer, SIGNAL(timeout()), this, SLOT(onTimeoutHideVotesReveal()));

    wBoard = new Widget_Gameboard(this, &mMSG, mHandlerVotes.votes);
    ui->layout->addWidget(wBoard);
    wMenu = new Dialog_Menu(this);
    wScreenSeePlayerRole = new Widget_ScreenCheckRole(this, &mMSG);
    wScreenSeeLawCards = new Widget_ScreenLawCards(this, &mMSG);
    wScreenSeePowerUnlocked = new Widget_ScreenPowerUnlocked(this);
    wScreenSeeEndGame = new Widget_ScreenEndGame(this, &mMSG);
    mSoundManager = new SoundManager(this, &mParameters.sound, &mParameters.music);
    wPopupMessage = new Widget_PopupMessage(wBoard);

    wMenu->show();
    wScreenSeePlayerRole->hide();
    wScreenSeeLawCards->hide();
    wScreenSeePowerUnlocked->hide();
    wPopupMessage->hide();

    mTCP_API = new TCP_API(this, wMenu->getName(), wMenu->getIp(), wMenu->getPort());

    connect(wMenu, SIGNAL(sig_connection()), mTCP_API, SLOT(connectToServer()));
    connect(wMenu, SIGNAL(sig_showBoard()), this, SLOT(onShowBoard()));
    connect(wMenu, SIGNAL(sig_joinGame()), this, SLOT(onJoinGame()));
    connect(mTCP_API, SIGNAL(sig_connectionResult(QAbstractSocket::SocketState)), wMenu, SLOT(onShowConnectionResult(QAbstractSocket::SocketState)));
    connect(mTCP_API, SIGNAL(sig_MSG_received(S_MESSAGE)), this, SLOT(gameState(S_MESSAGE)));
    connect(wBoard, SIGNAL(sig_voted(E_VOTE)), this, SLOT(EVENT_voted(E_VOTE)));
    connect(wBoard, SIGNAL(sig_menu()), this, SLOT(onShowMenu()));
    connect(wBoard, SIGNAL(sig_draw()), this, SLOT(EVENT_draw()));
    connect(wBoard, SIGNAL(sig_clickOnPlayer(E_IDENTIFIER)), this, SLOT(EVENT_playerSelected(E_IDENTIFIER)));
    connect(wScreenSeePowerUnlocked, SIGNAL(sig_startUsingPower(E_POWER)), this, SLOT(EVENT_startUsingPower(E_POWER)));
    connect(wScreenSeeLawCards, SIGNAL(sig_lawCardClicked(int)), this, SLOT(EVENT_discard(int)));
    connect(this, SIGNAL(sig_playMusic(SoundManager::E_MUSIC)), mSoundManager, SLOT(playMusic(SoundManager::E_MUSIC)));
    connect(this, SIGNAL(sig_playSound(SoundManager::E_SOUND)), mSoundManager, SLOT(playSound(SoundManager::E_SOUND)));
    connect(wMenu, SIGNAL(sig_playSound(SoundManager::E_SOUND)), mSoundManager, SLOT(playSound(SoundManager::E_SOUND)));
    connect(wBoard, SIGNAL(sig_playSound(SoundManager::E_SOUND)), mSoundManager, SLOT(playSound(SoundManager::E_SOUND)));
    connect(wScreenSeeEndGame, SIGNAL(sig_playSound(SoundManager::E_SOUND)), mSoundManager, SLOT(playSound(SoundManager::E_SOUND)));
    connect(wScreenSeeLawCards, SIGNAL(sig_playSound(SoundManager::E_SOUND)), mSoundManager, SLOT(playSound(SoundManager::E_SOUND)));
    connect(wScreenSeePlayerRole, SIGNAL(sig_playSound(SoundManager::E_SOUND)), mSoundManager, SLOT(playSound(SoundManager::E_SOUND)));
    connect(wScreenSeePowerUnlocked, SIGNAL(sig_playSound(SoundManager::E_SOUND)), mSoundManager, SLOT(playSound(SoundManager::E_SOUND)));
    connect(wPopupMessage, SIGNAL(sig_playSound(SoundManager::E_SOUND)), mSoundManager, SLOT(playSound(SoundManager::E_SOUND)));

    emit sig_playMusic(SoundManager::E_MUSIC::waitingGame);
}

Controller::~Controller()
{
    delete wPopupMessage;
}

void Controller::gameState(S_MESSAGE MSG)
{
    if(catchSpecificCommand(&MSG))
        return;

    revealVotes();
    detectGameEvent(&MSG);
    mMSG = MSG;

    updateBoard();

    // State machine
    switch(MSG.command)
    {
        case CMD_TO_PLAYER_START_GAME:
            STATE_gameStarted();
            initTurn();
            STATE_chancelorNomination();
            break;

        case CMD_TO_PLAYER_ELECT_CHANCELOR:
            initTurn();
            STATE_chancelorNomination();
            break;

        case CMD_TO_PLAYER_START_VOTE:
            onTimeoutHideVotesReveal();
            STATE_chancelorVote();
            break;

        case CMD_TO_PLAYER_PRESIDENT_DRAW:
            STATE_presidentAllowToDraw();
            break;

        case CMD_TO_PLAYER_CHANCELOR_DISCARD:
            STATE_chancelorDiscard();
        break;

        case CMD_TO_PLAYER_PUT_LAW_ON_BOARD:
            STATE_putLawOnBoard();
            break;

        case CMD_TO_PLAYER_VOTE_TO_KILL:
            STATE_powerStartVoteToKill();
            break;

        case CMD_TO_PLAYER_PLAYER_VOTED:
        case CMD_TO_PLAYER_PLAYER_VOTED_TO_KILL:
            STATE_showNewPlayerVote();
            break;

        case CMD_TO_PLAYER_END_GAME:
            STATE_endGame();
            break;

        default:
            printf("erreur, commande non reconnue");
            break;
    }

    updateBoard();
}

void Controller::STATE_gameStarted()
{
    wBoard->displayFacistTeam();
    emit sig_playMusic(SoundManager::E_MUSIC::none);
}

void Controller::STATE_chancelorNomination()
{
    if(mMSG.gameStatus.players[mMSG.identifier].electionRole == E_ELECTION_ROLE::minister)
    {
        notifyPlayerHasToPlay();
        wBoard->enableClickOnPlayers(true);
    }
}

void Controller::STATE_chancelorVote()
{
    notifyPlayerHasToPlay();
    enableVote(true);
}

void Controller::STATE_presidentAllowToDraw()
{
    if(mMSG.gameStatus.players[mMSG.identifier].electionRole == E_ELECTION_ROLE::minister)
    {
        notifyPlayerHasToPlay();
        enableDraw(true);
    }
}

void Controller::STATE_presidentDiscard()
{
    if(mMSG.gameStatus.players[mMSG.identifier].electionRole == E_ELECTION_ROLE::minister)
    {
        wScreenSeeLawCards->Setup_PresidentDiscard();
        setScreenInCenter(E_SCREEN::seeLawCards);
    }
}

void Controller::STATE_chancelorDiscard()
{
    if(mMSG.gameStatus.players[mMSG.identifier].electionRole == E_ELECTION_ROLE::director)
    {
        wScreenSeeLawCards->Setup_ChancelorDiscard();
        setScreenInCenter(E_SCREEN::seeLawCards);
    }
}

void Controller::STATE_putLawOnBoard()
{
    E_IDENTIFIER identifier = E_IDENTIFIER::ID_none;

    // Get President identifier.
    for(int i = 0; i < mMSG.gameStatus.players.size(); i++)
    {
        if(mMSG.gameStatus.players[i].electionRole == E_ELECTION_ROLE::minister)
        {
            identifier = static_cast<E_IDENTIFIER>(i);
            break;
        }
    }
    if(identifier == E_IDENTIFIER::ID_none)
        qDebug() << "erreur président non trouvé";

    // Check if there is a power to activate.
    if(mMSG.gameStatus.players[identifier].power != E_POWER::noPower)
    {
        // Start power reveal animation.
        setScreenInCenter(E_SCREEN::seePowerUnlocked);
        wScreenSeePowerUnlocked->startPowerAnimation(mMSG.gameStatus.players[identifier].power);
    }
    else
    {
        if(mMSG.identifier == identifier)
        {
            mMSG.command = CMD_TO_SERVER_START_NEW_TURN;
            mTCP_API->send_MSG(mMSG);
        }
    }
}

void Controller::STATE_powerCheckPile()
{
    wScreenSeeLawCards->Setup_PowerCheckPile();
    setScreenInCenter(E_SCREEN::seeLawCards);
    QTimer::singleShot(5000, this, SLOT(onTimeoutHideCheckPile()));
}

void Controller::STATE_powerCheckPlayerRole()
{
    setScreenInCenter(E_SCREEN::seePlayerRole);
    QTimer::singleShot(5000, this, SLOT(onTimeoutCheckPlayerRole()));
}

void Controller::STATE_powerStartVoteToKill()
{
    enableVote(true);
}

void Controller::STATE_showNewPlayerVote()
{
    for(int i = 0; i < mMSG.gameStatus.players.size(); i++)
    {
        mHandlerVotes.votes[i] = mMSG.gameStatus.players[i].vote;
    }
}

void Controller::STATE_endGame()
{
    wBoard->onHideFacistTeam();
    wBoard->updateBoard();
    emit sig_playMusic(SoundManager::E_MUSIC::waitingGame);
}

void Controller::EVENT_draw()
{
    emit sig_playSound(SoundManager::E_SOUND::handleCard);

    if(mMSG.gameStatus.players[mMSG.identifier].electionRole != E_ELECTION_ROLE::minister)
        printf("erreur joeur qui a plioché n'est pas président");

    if(mMSG.gameStatus.players[mMSG.identifier].power == E_POWER::checkPile)
    {
        STATE_powerCheckPile();
    }
    else
    {
        STATE_presidentDiscard();
    }
}

void Controller::EVENT_voted(E_VOTE vote)
{
    emit sig_playSound(SoundManager::E_SOUND::vote);

    mMSG.gameStatus.players[mMSG.identifier].vote = vote;

    if(mMSG.command == CMD_TO_PLAYER_START_VOTE || mMSG.command == CMD_TO_PLAYER_PLAYER_VOTED)
    {
        mMSG.command = CMD_TO_SERVER_PLAYER_VOTED;
        mTCP_API->send_MSG(mMSG);
    }
    else
    {
        mMSG.command = CMD_TO_SERVER_PLAYER_VOTED_TO_KILL;
        mTCP_API->send_MSG(mMSG);
    }
}

void Controller::EVENT_discard(int identifier)
{
    emit sig_playSound(SoundManager::E_SOUND::handleCard);

    setScreenInCenter(E_SCREEN::seeGameBoard);

    switch(mMSG.command)
    {
        case CMD_TO_PLAYER_PRESIDENT_DRAW:
            mMSG.gameStatus.pile.removeAt(identifier);
            mMSG.command = CMD_TO_SERVER_PRESIDENT_DISCARDED;
            mTCP_API->send_MSG(mMSG);
            break;

        case CMD_TO_PLAYER_CHANCELOR_DISCARD:
            mMSG.gameStatus.pile.removeAt(identifier);
            mMSG.command = CMD_TO_SERVER_CHANCELOR_DISCARDED;
            mTCP_API->send_MSG(mMSG);
            break;

        default:
            qDebug() << "errreur event discard : joueur non autorisé à defausser";
            break;
    }
}

void Controller::EVENT_playerSelected(E_IDENTIFIER identifier)
{
    emit sig_playSound(SoundManager::E_SOUND::select);

    if(mMSG.gameStatus.players[mMSG.identifier].electionRole != E_ELECTION_ROLE::minister)
    {
        qDebug() << "erreur player clicked, le joueur n'est pas président";
    }

    // Set the player focus.
    mMSG.gameStatus.playerFocus = identifier;

    wBoard->enableClickOnPlayers(false);

    switch(mMSG.command)
    {
        case CMD_TO_PLAYER_START_GAME:
        case CMD_TO_PLAYER_ELECT_CHANCELOR:
            mMSG.command = CMD_TO_SERVER_GIVE_CHANCELOR;
            mTCP_API->send_MSG(mMSG);
            break;

        case CMD_TO_PLAYER_PUT_LAW_ON_BOARD:
            switch(mMSG.gameStatus.players[mMSG.identifier].power)
            {
                case E_POWER::checkRole:
                    STATE_powerCheckPlayerRole();
                    break;

                case E_POWER::chooseMinister:
                    mMSG.command = CMD_TO_SERVER_NEW_PRESIDENT;
                    mTCP_API->send_MSG(mMSG);
                    break;

                case E_POWER::kill:
                    mMSG.command = CMD_TO_SERVER_KILL_PLAYER;
                    mTCP_API->send_MSG(mMSG);
                    break;

                case E_POWER::voteToKill:
                    onTimeoutHideVotesReveal();
                    mMSG.command = CMD_TO_SERVER_START_VOTE_TO_KILL;
                    mTCP_API->send_MSG(mMSG);
                    break;

                default:
                    qDebug() << "erreur joueur cliqué mais rien a faire";
                    break;
            }
            break;
    }
}

void Controller::EVENT_startUsingPower(E_POWER power)
{
    setScreenInCenter(E_SCREEN::seeGameBoard);

    if(mMSG.gameStatus.players[mMSG.identifier].electionRole == E_ELECTION_ROLE::minister)
    {
        notifyPlayerHasToPlay();
        switch(power)
        {
            case E_POWER::checkPile:
                enableDraw(true);
                break;

            case E_POWER::checkRole:
                wBoard->enableClickOnPlayers(true);
                break;

            case E_POWER::chooseMinister:
                wBoard->enableClickOnPlayers(true);
                break;

            case E_POWER::kill:
                wBoard->enableClickOnPlayers(true);
                break;

            case E_POWER::voteToKill:
                wBoard->enableClickOnPlayers(true);
                break;

            default:
                qDebug() << "erreur apres écran de révélation du pouvoir";
                break;
        }
    }
}

void Controller::EVENT_gameStatusChanged(E_END_GAME status)
{
    switch(status)
    {
        case E_END_GAME::notFinished:
            //STATE_gameStarted();
            break;

        default:
            setScreenInCenter(E_SCREEN::seeGameFinished);
            emit sig_playSound(SoundManager::E_SOUND::winner);
            break;
    }
}

void Controller::EVENT_playerDie(E_IDENTIFIER)
{
    if(mMSG.gameStatus.endGame == E_END_GAME::notFinished)
         emit sig_playSound(SoundManager::E_SOUND::pistol);
}

void Controller::onTimeoutHideVotesReveal()
{
    if(mHandlerVotes.t_displayer->isActive())
        mHandlerVotes.t_displayer->stop();

    wBoard->revealVotes(false);

    for(int i = 0; i < mMSG.gameStatus.players.size(); i++)
    {
        if(mMSG.gameStatus.players[i].vote == E_VOTE::nox || mMSG.gameStatus.players[i].vote == E_VOTE::lumos)
        {
            mMSG.gameStatus.players[i].vote = E_VOTE::blank;
            mHandlerVotes.votes[i] = E_VOTE::blank;
        }
    }

    updateBoard();
}

void Controller::onTimeoutCheckPlayerRole()
{
    setScreenInCenter(E_SCREEN::seeGameBoard);
    mMSG.command = CMD_TO_SERVER_START_NEW_TURN;
    mTCP_API->send_MSG(mMSG);
}

void Controller::onTimeoutHideCheckPile()
{
    setScreenInCenter(E_SCREEN::seeGameBoard);
    mMSG.command = CMD_TO_SERVER_START_NEW_TURN;
    mTCP_API->send_MSG(mMSG);
}

void Controller::onTimeoutReturnToGameBoard()
{
    setScreenInCenter(E_SCREEN::seeGameBoard);
}

void Controller::onShowMenu()
{
    wMenu->OpenMenu(mTCP_API->mSocket);
}

void Controller::onShowBoard()
{
    wMenu->hide();
    show();
}

void Controller::onJoinGame()
{
    S_MESSAGE MSG = mMSG;
    MSG.command = CMD_TO_SERVER_JOIN_GAME;
    mTCP_API->send_MSG(MSG);
}

void Controller::initDummyGameStatus()
{
    for(int i = 0; i < 10; i++)
    {
        mHandlerVotes.votes[i] = E_VOTE::blank;
    }
    mMSG.command = 0;
    mMSG.garbage = 0;
    mMSG.identifier = E_IDENTIFIER::ID_none;
    S_GAME_STATUS dummyGame = {
        QList<E_CARD>(),
        QList<S_PLAYER>(),
        E_IDENTIFIER::ID_none,
        { 0, 0, { E_POWER::noPower, E_POWER::noPower, E_POWER::noPower, E_POWER::noPower, E_POWER::noPower, E_POWER::noPower } },
        E_END_GAME::notStarted
    };
    mMSG.gameStatus = dummyGame;
}

void Controller::initTurn()
{
    enableDraw(false);
    enableVote(false);
    setScreenInCenter(E_SCREEN::seeGameBoard);
}

void Controller::enableVote(bool toggle)
{
    wBoard->enableVote(toggle);
}

void Controller::enableDraw(bool toggle)
{
    wBoard->enableDraw(toggle);
}

void Controller::revealVotes()
{
    // If the votes are displaying, return.
    if(mHandlerVotes.t_displayer->isActive())
        return;

    // If the votes have already been displayed, return.
    for(int i = 0; i < mMSG.gameStatus.players.size(); i++)
    {
        if(mMSG.gameStatus.players[i].vote == E_VOTE::blank && mMSG.gameStatus.players[i].status == E_PLAYER_STATUS::alive)
        {
            return;
        }
    }

    wBoard->revealVotes(true);
    mHandlerVotes.t_displayer->start(20000);
}

void Controller::notifyPlayerHasToPlay()
{
    // Notify player if the notifications are allowed.
    if(mParameters.notifications)
    {
        wPopupMessage->setText("ACTION À RÉALISER");
        wPopupMessage->setGeometry(0, 0, wBoard->width(), wBoard->height());
        wPopupMessage->show();
        wPopupMessage->startAnimation();
    }
}

bool Controller::catchSpecificCommand(S_MESSAGE* MSG)
{
    switch(MSG->command)
    {
        case CMD_TO_PLAYER_SET_NEW_IDENTIFIER:
            mMSG.identifier = static_cast<E_IDENTIFIER>(MSG->garbage);
            mMSG.garbage = 0;
            mMSG.gameStatus = MSG->gameStatus;
            updateBoard();
            return true;

        case CMD_TO_PLAYER_NEW_CONNECTION:
        case CMD_TO_PLAYER_PLAYER_JOINED:
        case CMD_TO_PLAYER_PLAYER_LEFT_GAME:
            mMSG.gameStatus = MSG->gameStatus;
            updateBoard();
            return true;
    }

    return false;
}

void Controller::detectGameEvent(S_MESSAGE* newMSG)
{
    // Check end game.
    if(newMSG->gameStatus.endGame != mMSG.gameStatus.endGame && newMSG->gameStatus.endGame != E_END_GAME::notStarted)
    {
        EVENT_gameStatusChanged(newMSG->gameStatus.endGame);
    }

    // Induce other player to freeze when a player has disconected.
    if(newMSG->gameStatus.players.size() != mMSG.gameStatus.players.size())
    {
        QMessageBox* popupmessage = new QMessageBox(this);
        popupmessage->setIcon(QMessageBox::Icon::Warning);
        popupmessage->setText("Un joueur a été déconnecté, peut être devriez-vous attendre sa reconnection");
    }

    // Check player dead.
    for(int i = 0; i < newMSG->gameStatus.players.size(); i++)
    {
        if(newMSG->gameStatus.players[i].status == E_PLAYER_STATUS::dead && mMSG.gameStatus.players[i].status == E_PLAYER_STATUS::alive)
        {
            EVENT_playerDie(static_cast<E_IDENTIFIER>(i));
        }
    }
}

void Controller::setScreenInCenter(Controller::E_SCREEN screen)
{
    switch(screen)
    {
        case E_SCREEN::seeGameBoard:
            wBoard->switchCurrentScreenWith(wBoard->wWidgetCenter);
            break;

        case E_SCREEN::seeLawCards:
            wBoard->switchCurrentScreenWith(wScreenSeeLawCards);
            break;

        case E_SCREEN::seePlayerRole:
            wBoard->switchCurrentScreenWith(wScreenSeePlayerRole);
            break;

        case E_SCREEN::seeGameFinished:
            wBoard->switchCurrentScreenWith(wScreenSeeEndGame);
            QTimer::singleShot(15000, this, SLOT(onTimeoutReturnToGameBoard()));
            break;

        case E_SCREEN::seePowerUnlocked:
            wBoard->switchCurrentScreenWith(wScreenSeePowerUnlocked);
            break;

    }

    updateBoard();
}

void Controller::updateBoard()
{
    wBoard->updateBoard();
    wScreenSeeEndGame->update();
    wScreenSeeLawCards->update();
    wScreenSeePlayerRole->update();
    wScreenSeePowerUnlocked->update();
}

void Controller::on_actionSounds_triggered()
{
    mParameters.sound = !mParameters.sound;

    // Get all actions.
    QList<QAction*> actions = ui->toolBar->actions();

    // Refresh sound icon.
    if(mParameters.sound)
    {
        actions[0]->setIcon(QIcon(":/icons/sound_on.png"));
    }
    else
    {
        actions[0]->setIcon(QIcon(":/icons/sound_off.png"));
    }
}

void Controller::on_actionNotifications_triggered()
{
    mParameters.notifications = !mParameters.notifications;

    // Get all actions.
    QList<QAction*> actions = ui->toolBar->actions();

    // Refresh notifications parameter.
    if(mParameters.notifications)
    {
        actions[1]->setIcon(QIcon(":/icons/notification_on.png"));
    }
    else
    {
        actions[1]->setIcon(QIcon(":/icons/notification_off.png"));
    }
}

void Controller::on_actionActivateMusic_triggered()
{
    mParameters.music = !mParameters.music;
    if(mMSG.gameStatus.endGame != E_END_GAME::notFinished)
    {
        emit sig_playMusic(SoundManager::E_MUSIC::waitingGame);
    }

    // Get all actions.
    QList<QAction*> actions = ui->toolBar->actions();

    // Refresh music parameter.
    if(mParameters.music)
    {
        actions[2]->setIcon(QIcon(":/icons/music_on.png"));
    }
    else
    {
        actions[2]->setIcon(QIcon(":/icons/music_off.png"));
    }
}