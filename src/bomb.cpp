#include <QtGui>
#include "bomb.h"

Bomb::Bomb( QObject *parent ):QObject( parent ),
                              m_id( -1 ),
                              radius( 30 ),
                              m_blowUp( false ),
                              mine( new QPixmap(":/images/mine.png")),
                              thread( NULL ),
                              m_pixmap( new QPixmap() ),
                              m_pos( new Position() )
{
    thread = new MyThread( this, BOmb );
}

Bomb::Bomb(QSize size, Position pos, Id id):QObject( 0 ),
                                m_id( id ),
                                radius( 30 ),
                                m_blowUp( false ),
                                mine( new QPixmap(":/images/mine.png")),
                                thread( NULL ),
                                m_pixmap( new QPixmap( size ) ),
                                m_pos( new Position(pos) )
{
    thread = new MyThread( this, BOmb );
}

Bomb::~Bomb()
{
    delete mine;
    delete m_pos;
    delete m_pixmap;
}

void Bomb::tikTak()
{
    if( m_id != -1)
        QTimer::singleShot(1300, this, SLOT(pullRadius()));
}

void Bomb::pullRadius()
{
    if( m_id == -1) return;
    m_blowUp = true;
    radius += 25;
    emit blowUp(*m_pos, radius);
    emit needsUpdate();
}

void Bomb::onBallPosChange(Position pos)
{
    if( m_id == -1) return;
    static Radius r = mine->width() / 2;

    if( (pos - *m_pos).manhattanLength() < (r + 35) ) {
        m_blowUp = true;
        emit blowUp(*m_pos, radius);
    }
}

void Bomb::show(QPainter* painter)
{
    if( m_id == -1) return;
    if( m_blowUp ) {
        if( radius <= 170 ) {
            QTimer::singleShot(40, this, SLOT(pullRadius()) );
        } else {
            radius = 35;
            m_blowUp = false;
            emit deleteMe(m_id);
            return;
        }
        QRadialGradient radGrad(*m_pos, radius);
        radGrad.setColorAt(0.7, Qt::yellow);
        radGrad.setColorAt(1, Qt::red);

        painter->setBrush(radGrad);
        painter->drawEllipse(*m_pos, radius, radius);

        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(*m_pos, radius-15, radius-15);
    } else {
        int r = mine->size().width();
        QRectF targetRect( *m_pos + Position(-r/2, -r/2), mine->size() );
        painter->drawPixmap( targetRect, *mine, mine->rect() );
    }
}

void Bomb::createNextPixmap()
{
    if( m_id == -1) return;
qDebug() << "Bomb::createNextPixmap";

    m_pixmap->fill( QColor( Qt::transparent ) );
    QPainter painter( m_pixmap );

qDebug() << "Bomb::createNextPixmap create painter";

    if( m_blowUp ) {
        if( radius <= 170 ) {
            QTimer::singleShot(45, this, SLOT(pullRadius()) );
        } else {
            radius = 35;
            m_blowUp = false;
            emit deleteMe(m_id);
            return;
        }
        QRadialGradient radGrad(*m_pos, radius);
        radGrad.setColorAt(0.6, Qt::yellow);
        radGrad.setColorAt(1, Qt::red);

        painter.setBrush(radGrad);
        painter.drawEllipse(*m_pos, radius, radius);

qDebug() << "Bomb::createNextPixmap drawEllipse";

        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(*m_pos, radius-15, radius-15);
    } else {
        int r = mine->size().width();
        QRectF targetRect( *m_pos + Position(-r/2, -r/2), mine->size() );
        painter.drawPixmap( targetRect, *mine, mine->rect() );
    }
}
