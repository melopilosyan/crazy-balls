#ifndef STUPIDGAME_H
#define STUPIDGAME_H

#include "bomb.h"
#include "gifts.h"
#include "gamer.h"
#include "opponent.h"
#include "startface.h"

class StupidGame : public QWidget
{
    Q_OBJECT

public:
    StupidGame(QWidget *parent = 0);
    ~StupidGame();

signals:
    void mouseMoved(Position);
    void mousePressed(Position);

private slots:
    void onNewGame();
    void backToMenu();
    void onGameOver();
    void onDeleteIce();
    void onFrozen(bool);
    void onDeleteBomb(Id);
    void moveGameOverLabel();
    void onDeleteOpponent(Id);

protected:
    void paintEvent(QPaintEvent*);
    void keyPressEvent(QKeyEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mousePressEvent(QMouseEvent*);

private:
    Id* idsPtr;
    Bomb* bombs;
    Ice* iceGift;
    Gamer* gamer;
    QTimer* timer;
    QColor* colors;
    StartFace* startFace;
    QLabel* gameOverLabel;
    Opponent** opponents;
    QPixmap* gamePixmap;
    QPixmap* backgroundFirst;
    QPixmap* backgroundSecond;
    bool playMode;
    bool frozenMode;
    int opponentNum;
    int bombNum;
    bool bombSet;
    bool m_gameOver;

    void addOpponent();
    void disconnectIce();
    void deleteOpponents();
    void disconnectGamer();
    void createGamePixmap();
    void makeIceConnections();
    void setBomb(Position, Id);
    void disConnectStartFace();
    void makeGamerConnections();
    void startPreparingNewGame();
    void makeConnectionsWithStartFace();
    void initBackgroundPixmaps(QColor, QColor);
    void initBackgroundPixmaps(Qt::GlobalColor, Qt::GlobalColor);
};

#endif // STUPIDGAME_H
