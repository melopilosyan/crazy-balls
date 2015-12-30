#include "opponent.h"
#include <QPainter>
#include <QTimer>


Opponent::Opponent(QSize sz, Position pos, Id id, QColor clr):
                                                    BaseBall(sz, pos, id, clr),
                                                    gamerPos(new Position())
{   }
Opponent::~Opponent()
{
    delete gamerPos;
}

void Opponent::move()
{
    if( frozen() ) return;

    static qreal speed = 2;
    static long int speedChangeCount = 0;

    if( ++speedChangeCount == 300 )
        speed += 0.2;

    Position deltaP = *gamerPos - position();
    setPosition( position() + (deltaP / deltaP.manhattanLength()) * speed );
    emit positionChanged( position(), id(), status() );
}

void Opponent::onGamerPosChanged(Position pos)
{
    if( frozen() ) {
        if( (pos - position()).manhattanLength() < BallRadius * 2 ) {
            if( frozen() ) {
                emit deleteMe( id() );
                return;
            } else if( status() == BomB ) {
                emit deleteMe( id() );
            } else {
                emit gameOver();
            }
        }
    }

    *gamerPos = pos;
    moveEyes( pos );
}

void Opponent::onBombBlowUp(Position pos, Radius radius)
{
    if( frozen() ) return;

    if( status() == BomB ) return;
    if( ( pos - position() ).manhattanLength() < ( radius + 30 ) ) {
        emit deleteMe( id() );
    }
}

