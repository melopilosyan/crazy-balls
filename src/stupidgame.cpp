#include <QtGui>
#include <QLabel>
#include "stupidgame.h"


static const int OpponentsMaxNum = 50;
/*==============================================================
----------------------------------------------------------------
                    Constructor and Destructor
----------------------------------------------------------------
---------------------------------------------------------------*/
StupidGame::StupidGame( QWidget *parent ):QWidget( parent ),
                                          idsPtr( new Id[OpponentsMaxNum]),
                                          bombs( new Bomb[3]),
                                          iceGift( new Ice(Position()) ),
                                          gamer( NULL ),
                                          timer( new QTimer(this) ),
                                          colors( new QColor[4] ),
                                          startFace( NULL ),
                                          gameOverLabel( new QLabel(this)),
                                          opponents( new Opponent*[OpponentsMaxNum] ),
                                          gamePixmap( NULL ),
                                          backgroundFirst( NULL ),
                                          backgroundSecond( NULL ),
                                          playMode( false ),
                                          frozenMode( false ),
                                          opponentNum( 0 ),
                                          bombNum( 0 ),
                                          bombSet( false ),
                                          m_gameOver( false )
{
    setFixedSize( 1000, 700 );
    setMouseTracking( true );

    gamer = new Gamer( size(), Position( width() / 2, height() / 2 ) );
    gameOverLabel->setGeometry( width()/2 - 90, height()/2 + 100, 180, 50 );
    gameOverLabel->setText( "<font size=20 color=red>Game Over</font>" );
    gameOverLabel->setVisible( false );

    // make connects with bombs and iceGift
    for( int i = 0; i < 3; i++ ) {
        connect( &bombs[i], SIGNAL(deleteMe(Id)),
                 this, SLOT(onDeleteBomb(Id)) );
        bombs[i].setPixmapSize( size() );
    }
    connect( iceGift, SIGNAL(frozen(bool)), this, SLOT(onFrozen(bool)));
    connect( iceGift, SIGNAL(deleteMe()), this, SLOT(onDeleteIce()));


    // init objects that needs window size
    QSize sz = size();
    gamePixmap = new QPixmap( sz );
    startFace = new StartFace( sz );
    backgroundFirst = new QPixmap( sz );
    backgroundSecond = new QPixmap( sz );
    initBackgroundPixmaps(Qt::darkBlue, Qt::blue);

    for( register int i = 0; i < OpponentsMaxNum; i++ )
        opponents[i] = NULL;

    // Save colors
    colors[0] = Qt::cyan;
    colors[1] = Qt::green;
    colors[2] = Qt::magenta;
    colors[3] = QColor("#dd9900");

    makeConnectionsWithStartFace();
    startFace->setVisible( true );
}

StupidGame::~StupidGame()
{
    delete gamer;
    delete timer;
    delete []bombs;
    delete []colors;
    delete []idsPtr;
    delete opponents;
    delete startFace;
    delete gamePixmap;
    delete gameOverLabel;
    delete backgroundFirst;
    delete backgroundSecond;
}


/*==============================================================
----------------------------------------------------------------
                   Auxiliary functions
----------------------------------------------------------------
---------------------------------------------------------------*/

Position randomPos( int xMax, int yMax, bool whatYouWant = false )
{
    static long int randNum = QTime::currentTime().second();

    randNum += 111;
    int y = randNum % yMax;
    int x = randNum % xMax;

    int rule = randNum % 4;

    if( whatYouWant ) {
        return Position( x, y );
    } else {
        switch( rule ) {
            case 0: return Position( 0, y );
            case 1: return Position( x, 0 );
            case 2: return Position( x, yMax );
            case 3: return Position( xMax, y );
        }
    }
    return Position();
}

Id nextId(Id* ptr)
{
    for( register int i = 0; i < OpponentsMaxNum; i++ )
    {
        if( !ptr[i] )
        {
            ptr[i] = i+1;
            return i+1;
        }
    }
    return 0;
}

bool noOpponents(Id* ptr, int opponentNum)
{
    bool yesNo = true;
    for( register int i = 0; i < opponentNum; i++ )
        if( ptr[i] )
            yesNo = false;
    return yesNo;
}
//                                                               Auxiliary functions


/*==============================================================
----------------------------------------------------------------
                        Make connections
----------------------------------------------------------------
---------------------------------------------------------------*/
void StupidGame::makeGamerConnections()
{
    connect( this, SIGNAL( mouseMoved( Position ) ),
             gamer, SLOT( onMouseMoved( Position ) ) );
    connect( timer, SIGNAL(timeout()),
             gamer, SLOT(move()) );

    connect( gamer, SIGNAL(gameOver()),
             this, SLOT(onGameOver()) );
    connect( gamer, SIGNAL(deleteOppenent(Id)),
             this, SLOT(onDeleteOpponent(Id)) );
    gamer->setId( 1 );
}

void StupidGame::addOpponent()
{
    Position pos = randomPos( width(), height() );
    Id id = nextId(idsPtr);

    // Create a new opponent
    Opponent* opponent = new Opponent( size(), pos, id,
                                       colors[((int)pos.manhattanLength()) % 4] );
    opponent->setGamerPos( gamer->position() );
    opponent->moveEyes( gamer->position() );
    if(!(id % 5))
        opponent->setStatus( BomB );

    // make connects
    connect( gamer, SIGNAL( positionChanged( Position, Id, Status ) ),
             opponent, SLOT( onGamerPosChanged( Position ) ) );
    connect( iceGift, SIGNAL( frozen(bool) ),
             opponent, SLOT( onFrozen(bool)) );
    connect( timer, SIGNAL( timeout() ),
             opponent, SLOT( move() ) );

    connect( opponent, SIGNAL( positionChanged( Position, Id, Status ) ),
             gamer, SLOT( onOpponentPosChanged( Position, Id, Status ) ) );
    connect( opponent, SIGNAL( gameOver() ),
             this, SLOT( onGameOver() ) );
    connect( opponent, SIGNAL( deleteMe(Id) ),
             this, SLOT( onDeleteOpponent(Id) ) );
    for( int i = 0; i < 2; i++ )
        connect( &bombs[i], SIGNAL( blowUp( Position, Radius ) ),
                 opponent, SLOT( onBombBlowUp( Position, Radius ) ) );

    //
    opponents[id-1] = opponent;
    opponentNum = (id  > opponentNum) ? id: opponentNum;
}

void StupidGame::makeConnectionsWithStartFace()
{
    connect( this, SIGNAL( mouseMoved( Position ) ),
             startFace, SLOT( onMouseMove( Position ) ) );
    connect( this, SIGNAL( mousePressed( Position ) ),
             startFace, SLOT( onMousePressed( Position ) ) );
    connect( startFace, SIGNAL( needsUpdate() ),
             this, SLOT( update() ) );
    connect( startFace, SIGNAL( exit() ),
             this, SLOT( close() ) );
    connect( startFace, SIGNAL(newGame() ),
             this, SLOT( onNewGame() ) );
}

void StupidGame::makeIceConnections()
{
    connect( gamer, SIGNAL(positionChanged(Position, Id, Status)),
             iceGift, SLOT(onGamerPosChange(Position)) );
}
//                                                               Make connections



/*==============================================================
----------------------------------------------------------------
                    Disconnect objects
----------------------------------------------------------------
---------------------------------------------------------------*/
void StupidGame::disconnectGamer()
{
    disconnect( this, SIGNAL( mouseMoved( Position ) ),
                gamer, SLOT( onMouseMoved( Position ) ) );
    connect( timer, SIGNAL( timeout() ),
             gamer, SLOT( move()) );

    disconnect( gamer, SIGNAL(gameOver()),
                this, SLOT(onGameOver()) );
    disconnect( gamer, SIGNAL(deleteOppenent(Id)),
                this, SLOT(onDeleteOpponent(Id)) );
    gamer->setId( -1 );
}

void StupidGame::disConnectStartFace()
{
    disconnect( this, SIGNAL( mouseMoved( Position ) ),
                startFace, SLOT( onMouseMove( Position ) ) );
    disconnect( this, SIGNAL( mousePressed( Position ) ),
                startFace, SLOT( onMousePressed( Position ) ) );
    disconnect( startFace, SIGNAL( needsUpdate() ),
                this, SLOT( update() ) );
    disconnect( startFace, SIGNAL( exit() ),
                this, SLOT( close() ) );
    disconnect( startFace, SIGNAL( newGame() ),
                this, SLOT( onNewGame() ) );
}

void StupidGame::disconnectIce()
{
    disconnect( gamer, SIGNAL(positionChanged(Position, Id, Status)),
                iceGift, SLOT(onGamerPosChange(Position)) );
}

//                                                               Disconnect objects


/*==============================================================
----------------------------------------------------------------
                         Private SLOTS
----------------------------------------------------------------
---------------------------------------------------------------*/
void StupidGame::backToMenu()
{
    if( timer->isActive() )
        timer->stop();
    playMode = false;
    disconnectGamer();
    disconnectIce();
    makeConnectionsWithStartFace();
    startFace->setVisible( true );
    update();
}

void StupidGame::onFrozen(bool frozen)
{
    frozenMode = frozen;
}

void StupidGame::onNewGame()
{
    QApplication::restoreOverrideCursor();
    startFace->setVisible( false );
    disConnectStartFace();
    startPreparingNewGame();
}

void StupidGame::onDeleteIce()
{
    disconnectIce();
    iceGift->setVisible( false );
}

void StupidGame::onGameOver()
{
    timer->stop();
    playMode = false;
    disconnectIce();
    disconnectGamer();
    disconnect( timer, SIGNAL(timeout()),
                this, SLOT(update()) );
    connect( timer, SIGNAL(timeout()),
             this, SLOT(moveGameOverLabel()) );
    update();
    timer->start(65);
}

void StupidGame::onDeleteBomb(Id id)
{
    bombs[id].setId(-1);
    int count = 0;
    for( register int i = 0; i < 3; i++ ) {
        if( bombs[i].id() == -1) {
            count++;
        }
    }
    bombSet = ( count == 3 ) ? false: true;
}

void StupidGame::onDeleteOpponent(Id id)
{
    if( idsPtr[id-1] ) {
        idsPtr[id-1] = 0;
        delete opponents[id-1];
        opponents[id-1] = NULL;
        opponentNum = ( opponentNum == id ) ? id - 1: opponentNum;
    }
}

void StupidGame::moveGameOverLabel()
{
    static int y = height() / 2 + 10;
    static const int x = width() / 2 - 90;

    gameOverLabel->setGeometry( x, y, 180, 50 );
    gameOverLabel->setVisible( true );
    y -= 10;

    if( gameOverLabel->pos().y() < 20 ) {
        timer->stop();
        y = height() / 2 + 10;
        gameOverLabel->setVisible( false );
        disconnect( timer, SIGNAL(timeout()),
                    this, SLOT(moveGameOverLabel()) );
        backToMenu();
    }
}



/*==============================================================
----------------------------------------------------------------
                     Overloaded functions
----------------------------------------------------------------
---------------------------------------------------------------*/
void StupidGame::paintEvent( QPaintEvent* )
{
    static unsigned opponentAddNum = 2;
    static long long unsigned updateNum = 0;

    if( playMode && !frozenMode ) {
        // Every 55 update add opponents
        if( !( ++updateNum % 55 ) ) {
            for( unsigned i = 0; i < opponentAddNum; i++ ) {
                addOpponent();
            }
        }

        // Every 500 update increase opponentAddNum
        if( !( updateNum % 500 ) ) opponentAddNum += 1;

        // Every 350 update give an ice gift
        if( !( updateNum % 350 ) ) {
            iceGift->init( randomPos( width(), height(), true ), true );
            makeIceConnections();
        }
    }
    if( frozenMode ) {
        if( noOpponents( idsPtr, opponentNum) ) {
            frozenMode = false;
            opponentAddNum = 3;
            updateNum = 0;
        }
    }

    QPainter p( this );
    createGamePixmap();
    p.drawPixmap( 0, 0, *gamePixmap );
}

void StupidGame::keyPressEvent(QKeyEvent* ev)
{
    if( !playMode ) return;

    switch( ev->key() ) {
        case Qt::Key_Escape:
            backToMenu();
            break;
        default:
            break;
    }
}

void StupidGame::mouseMoveEvent( QMouseEvent* ev )
{
    emit mouseMoved( ev->posF() );
}

void StupidGame::mousePressEvent(QMouseEvent* ev)
{
    if(Qt::RightButton == ev->button()) {
        if( (gamer->position() - ev->posF()).manhattanLength() < 30 ) {
            setBomb(gamer->position(), bombNum);
            bombNum++;
            // only 2 bombs can use user
            bombNum = bombNum % 2;
        }
    }
    emit mousePressed( ev->posF() );
}
//                                                               Overloaded functions


/*==============================================================
----------------------------------------------------------------
                         Private mothods
----------------------------------------------------------------
---------------------------------------------------------------*/
void StupidGame::startPreparingNewGame()
{
    deleteOpponents();
    makeGamerConnections();

    for( int i = 0; i < 3; i++ )
        bombs[i].setId(-1);

    opponentNum   =  0;
    for( int i = 0; i < 5; i++ )
        addOpponent();

    bombSet = false;
    playMode = true;
    connect( timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(45);
}

void StupidGame::createGamePixmap()
{
    gamePixmap->fill( QColor( Qt::transparent ) );
    QPainter painter( gamePixmap );

    painter.drawPixmap( 0, 0, *backgroundFirst );

    if( playMode ) {
        if( iceGift->visible() ) {
            iceGift->show(&painter);
        }
        if( bombSet )
            for( register int i = 0; i < 2; i++ )
                if( bombs[i].id() != -1)
                    bombs[i].show(&painter);

        for( register int i = 0; i < opponentNum; i++ )
            if( idsPtr[i] )
                opponents[i]->show(&painter);

        if( 1 == gamer->id() )
            gamer->show(&painter);
    }
    else {
        if( startFace->visible() )
            startFace->show(&painter);
    }
}

void StupidGame::setBomb(Position pos, Id id)
{
    bombSet = true;
    bombs[id].setPosition(pos);
    bombs[id].setId(id);
    bombs[id].tikTak();
}

void StupidGame::deleteOpponents()
{
    for( register int i = 0; i < OpponentsMaxNum; i++ ) {
        if( opponents[i] ) {
            delete opponents[i];
        }
        opponents[i] = NULL;
        idsPtr[i] = 0;
    }
}

void StupidGame::initBackgroundPixmaps( QColor first,
                                        QColor second
                                       )
{
    QRadialGradient radialGrad( QPoint( width() / 2,
                                        height() / 2
                                       ),
                                width() / 2 + 100
                               );
    radialGrad.setSpread( QGradient::RepeatSpread );
{ // just for skipping
    radialGrad.setColorAt( 0, first );
    radialGrad.setColorAt( 0.05, second );
    radialGrad.setColorAt( 0.1, first );
    radialGrad.setColorAt( 0.15, second );
    radialGrad.setColorAt( 0.2, first );
    radialGrad.setColorAt( 0.25, second );
    radialGrad.setColorAt( 0.3, first );
    radialGrad.setColorAt( 0.35, second );
    radialGrad.setColorAt( 0.4, first );
    radialGrad.setColorAt( 0.45, second );
    radialGrad.setColorAt( 0.5, first );
    radialGrad.setColorAt( 0.55, second );
    radialGrad.setColorAt( 0.6, first );
    radialGrad.setColorAt( 0.65, second );
    radialGrad.setColorAt( 0.7, first );
    radialGrad.setColorAt( 0.75, second );
    radialGrad.setColorAt( 0.8, first );
    radialGrad.setColorAt( 0.85, second );
    radialGrad.setColorAt( 0.9, first );
    radialGrad.setColorAt( 0.95, second );
    radialGrad.setColorAt( 1, first );
}
    QPainter painter(backgroundFirst);
    painter.setBrush( radialGrad );
    painter.drawRect( 0, 0, width(), height() );
    painter.end();
{
    radialGrad.setColorAt( 0, second );
    radialGrad.setColorAt( 0.05, first );
    radialGrad.setColorAt( 0.1, second );
    radialGrad.setColorAt( 0.15, first );
    radialGrad.setColorAt( 0.2, second );
    radialGrad.setColorAt( 0.25, first );
    radialGrad.setColorAt( 0.3, second );
    radialGrad.setColorAt( 0.35, first );
    radialGrad.setColorAt( 0.4, second );
    radialGrad.setColorAt( 0.45, first );
    radialGrad.setColorAt( 0.5, second );
    radialGrad.setColorAt( 0.55, first );
    radialGrad.setColorAt( 0.6, second );
    radialGrad.setColorAt( 0.65, first );
    radialGrad.setColorAt( 0.7, second );
    radialGrad.setColorAt( 0.75, first );
    radialGrad.setColorAt( 0.8, second );
    radialGrad.setColorAt( 0.85, first );
    radialGrad.setColorAt( 0.9, second );
    radialGrad.setColorAt( 0.95, first );
    radialGrad.setColorAt( 1, second );
}

    painter.begin(backgroundSecond);
    painter.setBrush( radialGrad );
    painter.drawRect( 0, 0, width(), height() );
}

void StupidGame::initBackgroundPixmaps( Qt::GlobalColor first,
                                        Qt::GlobalColor second
                                       )
{
    initBackgroundPixmaps( QColor( first ), QColor( second ) );
}//                                                              Private mothods




