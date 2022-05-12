#ifndef SERVER_CTR_H
#define SERVER_CTR_H

#include <QMainWindow>
#include <QTimer>
#include <QDebug>
#include "types.h"
#include "util_functions.h"
#include "tcp_api.h"
#include "widget_player.h"
#include "widget_card.h"
#include "widget_board.h"

extern char version[];

QT_BEGIN_NAMESPACE
namespace Ui { class Secret_Voldemort_Serveur; }
QT_END_NAMESPACE

class Server_CTR : public QMainWindow
{
    Q_OBJECT

private:
    typedef struct
    {
        QList<Widget_Player*> players;
        QList<Widget_Card*> pile;
        QList<Widget_Card*> boardFaciste;
        QList<Widget_Card*> boardLiberal;
        Widget_Board* boardMangemort;
        Widget_Board* boardPhenixOrder;

    }S_GAMEBOARD_DISPLAYER;

public:
    Server_CTR(QWidget *parent = nullptr);
    ~Server_CTR();

private slots:
    void gameState(S_MESSAGE MSG);

// ACTION state machine
private slots:
    void STATE_startTurn();
private:
    void STATE_startChancelorVote(S_MESSAGE MSG);
    void STATE_chancelorVoteResult(S_MESSAGE MSG);
    void STATE_presidentDiscarded(S_MESSAGE MSG);
    void STATE_chancelorDiscarded(S_MESSAGE MSG);
    void STATE_putLawOnBoard(S_MESSAGE MSG);
    void STATE_powerNewPresident(S_MESSAGE MSG);
    void STATE_powerKillPlayer(S_MESSAGE MSG);
    void STATE_changeName(S_MESSAGE MSG);
    void STATE_playerAskToJoinGame(S_MESSAGE MSG);
    void STATE_threeVoteCanceled();
    void STATE_sendMinisterVetoReply(S_MESSAGE MSG);

private:
    void initGame();
    void initDummyGame();
    void refreshPlayerStatus(S_MESSAGE MSG);
    bool addPlayerVote(E_IDENTIFIER identifier);
    E_VOTE getVoteResult();
    void killPlayer(E_IDENTIFIER identifier);
    void refreshPlayerIdentifier();
    void refreshGameStatusDisplay();
    void copyGameStatusDisplayedInto(S_GAME_STATUS* game);

private slots:
    void onStartGame();
    void onAddPlayer(QTcpSocket* socket);
    void onRemovePlayer(QTcpSocket* socket);
    void onHidePlayerVotes();
    void onPrint(QString message);
    void on_buttonRestartTurn_clicked();
    void on_buttonStartTurnWithInputs_clicked();
    void on_buttonAntiSpoil_stateChanged(int arg1);
    void on_buttonExpertMode_stateChanged(int arg1);
    void on_buttonRestartVote_clicked();

protected:
    void paintEvent(QPaintEvent*);

private:
    S_GAME_STATUS mGame;
    E_IDENTIFIER mPlayerFocus;
    TCP_API* mTCP_API;
    S_GAME_STATUS mTurnStateSaveMSG;
    QString mMessageLog;
    E_IDENTIFIER mSaveLastPresident;
    bool mRemoveAllRolesForNextTurn;

    S_GAMEBOARD_DISPLAYER mBoardStatusDisplay;

    bool mAntiSpoil;
    bool mExpertMode;

private:
    Ui::Secret_Voldemort_Serveur* ui;
};

#endif // SERVER_CTR_H
