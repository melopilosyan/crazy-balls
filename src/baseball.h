#ifndef BASEBALL_H
#define BASEBALL_H

#include "myTypes.h"
#include "mythread.h"

class BaseBall : public QObject
{
    Q_OBJECT

public:
    explicit BaseBall(QObject* parent = 0);
    BaseBall(QSize, Position, Id       id     = -1,
                              QColor   clr    = QColor(),
                              Status   status = NoBomB,
                              QObject* parent = 0);
    ~BaseBall();

    void setId(Id);
    void setColor(QColor);
    void setStatus(Status);
    void setPosition(Position);
    void setColor(Qt::GlobalColor);

    Id id() const;
    QPixmap pixmap();
    bool frozen() const;
    QColor color() const;
    Status status() const;
    Position position() const;

    void createNextPixmap();
    void show(QPainter*);

public slots:
    void onFrozen(bool);
    void moveEyes(Position);

signals:
    void gameOver();
    void positionChanged(Position, Id, Status);


private:
    enum UpDown{ Up = 0, Down };

    Id m_id;
    Status m_status;
    QFont* font;
    QColor* m_color;
    Position* m_pos;
    QImage* m_pixmap;
    MyThread* m_thread;
    Position* leftEyeDirection;
    Position* rightEyeDirection;
    bool m_frozen;

    QPolygonF arrow(UpDown);
    void drawPupils(QPainter*, const Position&, int, int);
};

inline void BaseBall::setColor(Qt::GlobalColor color) { *m_color = color; }
inline void BaseBall::setStatus(Status status) { m_status = status; }
inline void BaseBall::setColor(QColor color) { *m_color = color; }
inline void BaseBall::setPosition(Position pos) { *m_pos = pos; }
inline void BaseBall::setId(Id id) { m_id = id; }

inline Position BaseBall::position() const { return *m_pos; }
inline Status BaseBall::status() const { return m_status; }
inline QColor BaseBall::color() const { return *m_color; }
inline bool BaseBall::frozen() const { return m_frozen; }
inline Id BaseBall::id() const { return m_id; }
inline QPixmap BaseBall::pixmap()
{
    m_thread->start();
    return QPixmap::fromImage( *m_pixmap );
}


#endif // BASEBALL_H
