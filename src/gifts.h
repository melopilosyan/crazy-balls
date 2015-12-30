#ifndef GIFTS_H
#define GIFTS_H

#include "myTypes.h"

class Ice : public QObject
{
    Q_OBJECT
public:
    explicit Ice(const Position&, bool visible = false);
    ~Ice();

    bool visible() const;
    void show(QPainter*);
    void setVisible(bool);
    void init(const Position&, bool);

signals:
    void deleteMe();
    void frozen(bool);

public slots:
    void onGamerPosChange(Position);

private slots:
    void emitFrozenWithFalse();

private:
    bool m_visible;
    QPixmap* ice;
    Position* m_pos;
    Radius iceRadius;
};

inline bool Ice::visible() const { return m_visible; }
inline void Ice::setVisible(bool visible) { m_visible = visible; }

#endif // GIFTS_H
