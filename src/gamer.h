#ifndef GAMER_H
#define GAMER_H

#include "baseball.h"

class Gamer : public BaseBall
{
    Q_OBJECT

public:
    explicit Gamer();
    Gamer(QSize, Position);
    ~Gamer();


signals:
    void deleteOppenent(Id);

public slots:
    void move();
    void onMouseMoved(Position);
    void onOpponentPosChanged(Position, Id, Status);

private:
    Position* m_mousePos;
    QVector<QPointF>* steps;
};


#endif // GAMER_H
