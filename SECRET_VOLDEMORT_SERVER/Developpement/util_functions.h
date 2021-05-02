#ifndef UTIL_FUNCTIONS_H
#define UTIL_FUNCTIONS_H

#include <QTcpSocket>
#include "types.h"

void initRole(S_GAME_STATUS* game);
void initBoard(S_GAME_STATUS* game);
void initPlayers(S_GAME_STATUS* game);

void setNextPresident(S_GAME_STATUS* game);
void setNextPresidentAfterSpecialTurn(E_IDENTIFIER lastPresident, E_IDENTIFIER specialPresident, S_GAME_STATUS* game);
void setNewPresident(S_GAME_STATUS* game);
void setChancelor(S_GAME_STATUS* game, E_IDENTIFIER player);

E_IDENTIFIER getAvailableIdentifier(S_GAME_STATUS* game);
E_ROLE getMissingRole(S_GAME_STATUS game);
E_ROLE_NAME getMissingRoleName(E_IDENTIFIER identifier, S_GAME_STATUS game);
quint8 getNumberPlayersPlaying(S_GAME_STATUS game);
quint8 getNumberPlayersAlive(S_GAME_STATUS game);

E_IDENTIFIER getPlayerIdentifier(QList<S_PLAYER> players, QTcpSocket* socket);
E_IDENTIFIER getPresidentIdentifier(S_GAME_STATUS game);

void copyNewPile(QList<E_CARD> & newPile, QList<E_CARD> pileToCopy);
void generateNewPile(QList<E_CARD> * pile);

QString getPowerString(E_POWER power);
QString getVoteString(E_VOTE vote);
QString getRoleString(E_ROLE role);
QString getCommandString(quint8 command);

void printPlayer(const S_PLAYER & player);
void printBoard(const S_BOARD & board);
void printGame(const S_GAME_STATUS & game);


#endif // UTIL_FUNCTIONS_H
