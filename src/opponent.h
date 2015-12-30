#ifndef OPPENENT_H
#define OPPENENT_H

#include "baseball.h"

class Opponent : public BaseBall
{
    Q_OBJECT

public:
    explicit Opponent( QSize, Position, Id id = -5, QColor clr = QColor());
    ~Opponent();
    void setGamerPos(Position pos) { *gamerPos = pos; }

signals:
    void deleteMe(Id);

public slots:
    void move();
    void onBombBlowUp(Position, Radius);
    void onGamerPosChanged(Position);

private:
    Position* gamerPos;
};


#endif // OPPENENT_H
