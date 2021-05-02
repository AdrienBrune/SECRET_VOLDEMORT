#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QMainWindow>
#include "data.h"
#include "dialog_menu.h"
#include "widget_gameboard.h"
#include "tcp_api.h"
#include "soundmanager.h"
#include "widget_popupmessage.h"

namespace Ui {
class Controller;
}

class Controller : public QMainWindow
{
    Q_OBJECT

public:
    typedef enum
    {
        seeGameFinished,
        seePlayerRole,
        seeLawCards,
        seePowerUnlocked,
        seeGameBoard

    }E_SCREEN;

    typedef struct
    {
        E_VOTE votes[10];
        QTimer* t_displayer;

    }S_VOTE_HANDLER;

private:
    typedef struct
    {
       bool notifications;
       bool sound;
       bool music;

    }S_PARAMETERS;

public:
    Controller(QMainWindow* parent = nullptr);
    ~Controller();

signals:
    void sig_playSound(SoundManager::E_SOUND);
    void sig_playMusic(SoundManager::E_MUSIC);

private slots:
    void gameState(S_MESSAGE MSG);

// ACTION state machine
private:
    void STATE_gameStarted();
    void STATE_DirectorNomination();
    void STATE_DirectorVote();
    void STATE_MinisterAllowToDraw();
    void STATE_ministerDiscard();
    void STATE_DirectorDiscard();
    void STATE_MinisterReplyToVeto();
    void STATE_forceDirectorToVote();
    void STATE_putLawOnBoard();
    void STATE_powerCheckPile();
    void STATE_powerCheckPlayerRole();
    void STATE_powerStartVoteToKill();
    void STATE_showNewPlayerVote();
    void STATE_endGame();

private slots:
    void EVENT_draw();
    void EVENT_voted(E_VOTE);
    void EVENT_discard(int);
    void EVENT_playerSelected(E_IDENTIFIER);
    void EVENT_startUsingPower(E_POWER);
    void EVENT_gameStatusChanged(E_END_GAME);
    void EVENT_playerDie(E_IDENTIFIER);
    void EVENT_vetoAsked();

    void onTimeoutHideVotesReveal();
    void onTimeoutCheckPlayerRole();
    void onTimeoutHideCheckPile();
    void onTimeoutReturnToGameBoard();

    void onShowMenu();
    void onShowBoard();
    void onJoinGame();    

private slots:
    void on_actionSounds_triggered();
    void on_actionNotifications_triggered();


    void on_actionActivateMusic_triggered();

private:
    void initDummyGameStatus();
    void initTurn();
    void enableVote(bool toggle);
    void enableDraw(bool toggle);
    void revealVotes();
    void notifyPlayerHasToPlay();

    bool catchSpecificCommand(S_MESSAGE*);
    void detectGameEvent(S_MESSAGE* newMSG);

    void updateBoard();
    void setScreenInCenter(E_SCREEN screen);

private:
    TCP_API* mTCP_API;
    Dialog_Menu* wMenu;
    Widget_Gameboard* wBoard;
    SoundManager* mSoundManager;

    Widget_PopupMessage* wPopupMessage;
    Widget_ScreenCheckRole* wScreenSeePlayerRole;
    Widget_ScreenLawCards* wScreenSeeLawCards;
    Widget_ScreenPowerUnlocked* wScreenSeePowerUnlocked;
    Widget_ScreenEndGame* wScreenSeeEndGame;

    S_VOTE_HANDLER mHandlerVotes;
    S_PARAMETERS mParameters;

private:
    S_MESSAGE mMSG;

public:
    Ui::Controller *ui;
};
#endif // CONTROLLER_H
