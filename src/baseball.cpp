#include <QtGui>
#include "baseball.h"
QPoint computePupil (const QPoint&, const QPoint&, int, int);

BaseBall::BaseBall(QObject* parent):QObject(parent),
                                    m_id(-1),
                                    m_status(NoBomB),
                                    font( new QFont( "Serif", 14 )),
                                    m_color(new QColor()),
                                    m_pos(new Position(0, 0)),
                                    m_pixmap( new QImage ),
                                    m_thread( new MyThread(this) ),
                                    leftEyeDirection(new Position(0, 0)),
                                    rightEyeDirection(new Position(0, 0)),
                                    m_frozen( false )
{
    font->setItalic( true );
}

BaseBall::BaseBall(QSize sz, Position pos, Id id, QColor color, Status status,
                             QObject* parent):QObject(parent),
                                              m_id( id ),
                                              m_status(status),
                                              font( new QFont( "Serif", 14 )),
                                              m_color(new QColor(color)),
                                              m_pos(new Position(pos)),
                                              m_pixmap( new QImage(sz, QImage::Format_MonoLSB) ),
                                              m_thread( new MyThread(this) ),
                                              leftEyeDirection(new Position(0, 0)),
                                              rightEyeDirection(new Position(0, 0)),
                                              m_frozen( false )
{
    font->setItalic( true );
}

BaseBall::~BaseBall()
{
    delete m_pos;
    delete m_color;
    delete m_pixmap;
    delete m_thread;
    delete leftEyeDirection;
    delete rightEyeDirection;
}

void BaseBall::show(QPainter* painter)
{
    static int UpDownChanger = 0;
    static UpDown upDown = Up;


    Position leftEyePos = *m_pos + Position( -15, -4 );
    Position rightEyePos = *m_pos + Position( 15, -4 );

    // draw body
    QRadialGradient radialGradient( *m_pos, BallRadius );
    if( m_frozen ) {
        radialGradient.setColorAt( 0, QColor(Qt::gray).lighter(150) );
        radialGradient.setColorAt( 0.5, QColor(Qt::gray) );
        radialGradient.setColorAt( 1, QColor(Qt::gray).darker() );
    } else {
        radialGradient.setColorAt( 0, m_color->lighter(100) );
        radialGradient.setColorAt( 0.5, *m_color );
        radialGradient.setColorAt( 1, m_color->darker() );
    }
    radialGradient.setSpread( QGradient::ReflectSpread );

    painter->setPen( Qt::NoPen );
    painter->setBrush( radialGradient );
    painter->drawEllipse( *m_pos, BallRadius, BallRadius );

    // draw eyes
    painter->setBrush(Qt::white);
    painter->setPen( ( m_frozen ) ? QColor(Qt::gray):
                                    m_color->darker(120) );
    painter->drawEllipse(leftEyePos, 10, 13);
    painter->drawEllipse(rightEyePos, 10, 13);

    // draw pupils
    painter->setPen(Qt::NoPen);
    painter->setBrush( ( m_frozen ) ? Qt::darkGray: Qt::black );
    painter->drawEllipse( (leftEyePos + (*leftEyeDirection) * 7), 5, 6.5 );
    painter->drawEllipse( (rightEyePos + (*rightEyeDirection) * 7), 5, 6.5 );

    // draw smile
    if( !m_frozen ) {
        QPen pen( Qt::black, 2);
        painter->setPen(pen);
        static int angle = 45;
        painter->drawArc( m_pos->x()-20, m_pos->y()-20,
                          40, 45, -(90-angle)*16, -2*angle*16 );
    }

    // draw text and arrow
    if( ( status() == BomB ) && !m_frozen ) {
        if( ( ++UpDownChanger % 12 ) > 7) {
            upDown = Up;
        } else {
            upDown = Down;
        }

        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::white);
        painter->drawPolygon(arrow(upDown));

        painter->setFont( *font );
        painter->setPen( Qt::white );
        painter->drawText( position() + Position(-49, +50),
                           QString("Pump me!") );
    }
}

void BaseBall::moveEyes(Position mousePos)
{
    if( m_frozen ) {
        *leftEyeDirection = Position( 0, 0 );
        *rightEyeDirection = Position( 0, 0 );
        return;
    }

    Position leftEyePos = *m_pos + Position(-16, -4);
    Position rightEyePos = *m_pos + Position(16, -4);

    Position delta = mousePos - leftEyePos;
    *leftEyeDirection = delta / delta.manhattanLength();
    delta = mousePos - rightEyePos;
    *rightEyeDirection = delta / delta.manhattanLength();
}

void BaseBall::onFrozen(bool frozen)
{
    m_frozen = frozen;
}

QPolygonF BaseBall::arrow(UpDown upDown)
{
    int x = m_pos->x();
    int y = m_pos->y();
    QPolygonF arrow;
    if( upDown == Up) {
        y -= 50;
        arrow = QPolygonF() << Position(x, y)
                                << Position(x-8, y-10)
                                << Position(x-4, y-10)
                                << Position(x-4, y-25)
                                << Position(x+4, y-25)
                                << Position(x+4, y-10)
                                << Position(x+8, y-10);
    }
    else {
        y -= 40;
        arrow = QPolygonF() << Position(x, y)
                                << Position(x-8, y-10)
                                << Position(x-4, y-10)
                                << Position(x-4, y-25)
                                << Position(x+4, y-25)
                                << Position(x+4, y-10)
                                << Position(x+8, y-10);
    }
    return arrow;
}

void BaseBall::createNextPixmap()
{
    //m_pixmap->fill(QColor(Qt::transparent));
    //QPainter painter(m_pixmap);
    static int UpDownChanger = 0;
    static UpDown upDown = Up;


    m_pixmap->fill(0);
    QPainter painter(m_pixmap);

    Position leftEyePos = *m_pos + Position( -15, -4 );
    Position rightEyePos = *m_pos + Position( 15, -4 );

    // draw body
    QRadialGradient radialGradient( *m_pos, BallRadius );
    if( m_frozen ) {
        radialGradient.setColorAt( 0, QColor(Qt::gray).lighter(150) );
        radialGradient.setColorAt( 0.5, QColor(Qt::gray) );
        radialGradient.setColorAt( 1, QColor(Qt::gray).darker() );
    } else {
        radialGradient.setColorAt( 0, m_color->lighter(100) );
        radialGradient.setColorAt( 0.5, *m_color );
        radialGradient.setColorAt( 1, m_color->darker() );
    }
    radialGradient.setSpread( QGradient::ReflectSpread );

    painter.setPen( Qt::NoPen );
    painter.setBrush( radialGradient );
    painter.drawEllipse( *m_pos, BallRadius, BallRadius );

    // draw eyes
    painter.setBrush(Qt::white);
    painter.setPen( ( m_frozen ) ? QColor(Qt::gray):
                                    m_color->darker(120) );
    painter.drawEllipse(leftEyePos, 10, 13);
    painter.drawEllipse(rightEyePos, 10, 13);

    // draw pupils
    painter.setPen(Qt::NoPen);
    painter.setBrush( ( m_frozen ) ? Qt::darkGray: Qt::black );
    painter.drawEllipse( (leftEyePos + (*leftEyeDirection) * 7), 5, 6.5 );
    painter.drawEllipse( (rightEyePos + (*rightEyeDirection) * 7), 5, 6.5 );

    // draw smile
    if( !m_frozen ) {
        QPen pen( Qt::black, 2);
        painter.setPen(pen);
        static int angle = 45;
        painter.drawArc( m_pos->x()-20, m_pos->y()-20,
                          40, 45, -(90-angle)*16, -2*angle*16 );
    }

    // draw text and arrow
    if( ( status() == BomB ) && !m_frozen ) {
        if( ( ++UpDownChanger % 12 ) > 7) {
            upDown = Up;
        } else {
            upDown = Down;
        }

        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::white);
        painter.drawPolygon(arrow(upDown));

        painter.setFont( *font );
        painter.setPen( Qt::white );
        painter.drawText( position() + Position(-49, +50),
                           QString("Pump me!") );
    }
}

#include <cmath>
QPoint computePupil( const  QPoint& center,
                     const  QPoint& focus,
                     int            eyeWidth,
                     int            eyeHeight )
{
    QPointF dPoint = focus-center;
    if (dPoint.x() == 0 && dPoint.y() == 0) {
        return center;
    } else {
        double angle = atan2(dPoint.y(), dPoint.x());
        double cosa = cos(angle);
        double sina = sin(angle);
        double h = hypot(eyeHeight * cosa, eyeWidth * sina);
        double x = (eyeWidth * eyeHeight) * cosa / h;
        double y = (eyeWidth * eyeHeight) * sina / h;
        double dist = hypot(x*0.7, y*0.7); //?
        if (dist > hypot (dPoint.x(), dPoint.y())) {
            return dPoint.toPoint()+center;
        } else {
            return QPoint(dist*cosa+center.x(), dist*sina+center.y());
        }
    }
}
