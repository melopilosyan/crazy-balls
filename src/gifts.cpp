#include <QPainter>
#include "gifts.h"
#include <QTimer>
#include "myTypes.h"


Ice::Ice(const Position& pos, bool visible):QObject(0),
                                            m_visible( visible ),
                                            ice( new QPixmap(":/images/ice.png") ),
                                            m_pos( new Position(pos) )
{
    iceRadius = ice->height() / 2;
}

Ice::~Ice()
{
    delete ice;
    delete m_pos;
}

void Ice::init(const Position& pos, bool visible)
{
    *m_pos = pos;
    m_visible = visible;
    QTimer::singleShot(2500, this, SIGNAL(deleteMe()));
}

void Ice::onGamerPosChange(Position gamerPos)
{
    if( (*m_pos - gamerPos).manhattanLength() <  BallRadius + iceRadius ) {
        m_visible = false;
        emit frozen( true );
        QTimer::singleShot( 3500, this, SLOT( emitFrozenWithFalse() ) );
    }
}

void Ice::emitFrozenWithFalse()
{
    emit frozen( false );
}

void Ice::show(QPainter* painter)
{
    QRect destination( m_pos->toPoint() + QPoint(-iceRadius/2, -iceRadius/2),
                       ice->size() );
    painter->drawPixmap( destination, *ice, ice->rect() );
}
