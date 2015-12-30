#include "gamer.h"
#include <QPainter>
#include <QVector>
#include <QTimer>


Gamer::Gamer(QSize sz, Position pos):BaseBall( sz, pos, 1,
                                               QColor(Qt::yellow).darker(110)),
                                     m_mousePos(new Position(pos)),
                                     steps(new QVector<Position>())
{   }

Gamer::~Gamer()
{
    delete steps;
    delete m_mousePos;
}

void Gamer::onMouseMoved( Position mousePos )
{
    *m_mousePos = mousePos;
    moveEyes( mousePos );
}

void Gamer::onOpponentPosChanged(Position pos, Id id, Status status)
{
    if( (int)( ( pos - position() ).manhattanLength() ) < BallRadius * 2 ) {
        if( status == BomB )
            emit deleteOppenent( id );
        else
            emit gameOver();
    }
}

void Gamer::move()
{
    Position deltaP( *m_mousePos - position() );
    if( deltaP.manhattanLength() > 10) {
        setPosition( Position( deltaP.x() / 4 ,deltaP.y() / 4) + position() );
    }
    else {
        setPosition( *m_mousePos );
    }
    emit positionChanged(position(), id(), NoBomB);
}

