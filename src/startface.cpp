#include <QtGui>
#include "startface.h"

// Button size
static const int Width = 250;
static const int Height = 70;

StartFace::StartFace( QSize size ):QObject( 0 ),
                                   m_visible( false ),
                                   font( new QFont("Serif") ),
                                   image( new QPixmap(":/images/Billiard.jpg") ),
                                   button( new QPolygon[2]),
                                   thread( NULL ),
                                   m_pixmap( new QPixmap( size ) ),
                                   callNum( NoCall ),
                                   gradient( new QLinearGradient[2] ),
                                   newGameButtonX( 0 ),
                                   newGameButtonY( 0 ),
                                   exitButtonX( 0 ),
                                   exitButtonY( 0 ),
                                   mouseOverExitButton( false ),
                                   mouseOverNewGameButton( false )
{
    newGameButtonX = size.width() - ( Width + Height - 10 );
    newGameButtonY = size.height() - ( Height * 4 - 30 );
    exitButtonX = newGameButtonX - 130;
    exitButtonY = newGameButtonY + ( Height * 2 - 20 );

    button[NewGame] = buttonShape( QPoint( newGameButtonX, newGameButtonY) );
    button[Exit] = buttonShape( QPoint( exitButtonX, exitButtonY) );

    initGradients();
}

StartFace::~StartFace()
{
    delete font;
    delete image;
    delete []button;
    delete []gradient;
}

void StartFace::initGradients()
{
    QColor color(Qt::cyan);
    gradient[Free].setSpread( QGradient::ReflectSpread );
    gradient[Free].setColorAt( 0, color.darker(190) );
    gradient[Free].setColorAt( 0.4, color.darker(110) );
    gradient[Free].setColorAt( 0.5, color.darker(90) );
    gradient[Free].setColorAt( 0.6, color.darker(110) );
    gradient[Free].setColorAt( 1, color.darker(250) );


    gradient[Touched].setSpread( QGradient::ReflectSpread );
    gradient[Touched].setColorAt( 0.1, color.darker(125) );
    gradient[Touched].setColorAt( 0.45, color.darker(195) );
    gradient[Touched].setColorAt( 0.5, color.darker(200) );
    gradient[Touched].setColorAt( 0.6, color.darker(195) );
    gradient[Touched].setColorAt( 0.9, color.darker(130) );
}

QPolygon StartFace::buttonShape(QPoint leftTop)
{
    QVector<QPoint> pVec;
    int x = 0;
    register int y = 0;
    int a = ( Height / 2 ) * ( Height / 2 );

    // left side
    leftTop += QPoint( Height / 2, Height / 2 );
    for( y = Height/2; y >= 0; y--) {
        x = sqrt( a - y * y);
        pVec << leftTop + QPoint(-x, -y);
    }
    for( y = 0; y <= Height/2; y++) {
        x = sqrt( a - y * y);
        pVec << leftTop + QPoint(-x, y);
    }
    // right side
    leftTop.rx() += Width - Height;
    for( y = Height/2; y >= 0; y-- ) {
        x = sqrt( a - y * y );
        pVec << leftTop + QPoint(x, y);
    }
    for( y = 0; y <= Height/2; y++) {
        x = sqrt( a - y * y );
        pVec << leftTop + QPoint(x, -y);
    }
    return QPolygon( pVec );
}

void StartFace::onMouseMove( Position pos )
{
    static const QCursor badHand(QCursor(QPixmap(":/images/bad_hand.png")));
    static const QCursor goodHand(QCursor(QPixmap(":/images/good_hand.png")));
    if( callNum == NoCall ) return;

    if( button[NewGame].containsPoint( pos.toPoint(), Qt::OddEvenFill ) )
    {
        if( mouseOverNewGameButton ) return;
        else {
            QApplication::setOverrideCursor(goodHand);//Qt::PointingHandCursor);
            mouseOverNewGameButton = true;
            emit needsUpdate();
        }
    }
    else if( button[Exit].containsPoint( pos.toPoint(), Qt::OddEvenFill ) )
    {
        if( mouseOverExitButton ) return;
        else {
            QApplication::setOverrideCursor(badHand);//Qt::PointingHandCursor);
            mouseOverExitButton = true;
            emit needsUpdate();
        }
    }
    else
    {
        if( mouseOverExitButton ) {
            QApplication::restoreOverrideCursor();
            mouseOverExitButton = false;
            emit needsUpdate();
        }

        if( mouseOverNewGameButton ) {
            QApplication::restoreOverrideCursor();
            mouseOverNewGameButton = false;
            emit needsUpdate();
        }
    }
}

void StartFace::onMousePressed( Position pos )
{
    if( callNum == NoCall ) return;

    if( button[NewGame].containsPoint( pos.toPoint(), Qt::OddEvenFill ) ) {
        emit newGame();
        emit needsUpdate();
    }
    if( button[Exit].containsPoint( pos.toPoint(), Qt::OddEvenFill ) ) {
        emit exit();
        emit needsUpdate();
    }
}

void StartFace::setGradient( QPainter*    painter,
                             GradientType gradType,
                             ButtonType   bType )
{
    int X = ( bType == NewGame ) ? newGameButtonX: exitButtonX;
    int Y = ( bType == NewGame ) ? newGameButtonY: exitButtonY;
    X += Width / 2;

    gradient[gradType].setStart( X, Y );
    gradient[gradType].setFinalStop( X, Y + Height );

    painter->setBrush(gradient[gradType]);
}

void StartFace::drawButton( QPainter*       painter,
                            bool            mouseOverButton,
                            ButtonType      buttonType,
                            const QString&  text )
{
    static const QRect rect[] =  {
        QRect( newGameButtonX, newGameButtonY, Width, Height ),
        QRect( exitButtonX, exitButtonY, Width, Height )
    };

    GradientType gradType = ( mouseOverButton ) ? Touched: Free;

    setGradient( painter, gradType, buttonType );

    painter->setPen( Qt::NoPen );
    painter->drawPolygon( button[buttonType] );

    font->setPixelSize( ( mouseOverButton ) ? 34: 35 );

    painter->setFont( *font );
    painter->setPen( Qt::black );
    painter->drawText( rect[buttonType], Qt::AlignCenter, text );
}

void StartFace::show( QPainter* painter )
{
    if( callNum == NoCall ) return;

    painter->drawPixmap( m_pixmap->rect(), *image, image->rect() );
    QString text = ( callNum == FirstCall ) ? "Start": "New Game";

    drawButton(painter, mouseOverNewGameButton, NewGame, text);
    drawButton(painter, mouseOverExitButton, Exit, "Exit");
}

void StartFace::createNextPixmap()
{
    m_pixmap->fill( QColor( Qt::transparent ) );
    QPainter painter( m_pixmap );

    painter.drawPixmap( m_pixmap->rect(), *image, image->rect() );
    QString text = ( callNum == FirstCall ) ? "Start": "New Game";

    drawButton(&painter, mouseOverNewGameButton, NewGame, text);
    drawButton(&painter, mouseOverExitButton, Exit, "Exit");
}
