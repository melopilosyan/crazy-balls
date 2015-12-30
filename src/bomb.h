#ifndef H_BOMB_H
#define H_BOMB_H

#include "myTypes.h"
#include "mythread.h"


class Bomb : public QObject
{
    Q_OBJECT
public:
    explicit Bomb(QObject *parent = 0);
    Bomb(QSize, Position, Id id = -1);
    ~Bomb();

    void setId(Id id);
    void setPosition(Position pos);
    void setPixmapSize(const QSize&);

    void createNextPixmap();
    void show(QPainter*);

    Id id() const;
    QPixmap pixmap();

signals:
    void deleteMe(Id);
    void needsUpdate();
    void blowUp(Position, Radius);

public slots:
    void tikTak();
    void onBallPosChange(Position);

private slots:
    void pullRadius();

private:
    Id m_id;
    Radius radius;
    bool m_blowUp;
    QPixmap* mine;
    MyThread* thread;
    QPixmap* m_pixmap;
    Position* m_pos;

};

inline Id Bomb::id() const { return m_id; }
inline void Bomb::setPosition(Position pos) { *m_pos = pos; }
inline void Bomb::setId(Id id) { m_id = id; radius = 30; m_blowUp = false; }
inline void Bomb::setPixmapSize(const QSize& size)
{
    delete m_pixmap;
    m_pixmap = new QPixmap(size);
    m_pixmap->fill( QColor( Qt::transparent ) );
}
inline QPixmap Bomb::pixmap()
{
    thread->start();
    return *m_pixmap;
}

#endif // H_BOMB_H
