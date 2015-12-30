#ifndef STARTFACE_H
#define STARTFACE_H

#include "myTypes.h"
#include "mythread.h"

class StartFace : public QObject
{
    Q_OBJECT
public:
    StartFace(QSize);
    ~StartFace();

    void show(QPainter*);
    void setVisible(bool);
    void createNextPixmap();

    bool visible();
    QPixmap pixmap();

signals:
    void exit();
    void newGame();
    void needsUpdate();

public slots:
    void onMouseMove(Position);
    void onMousePressed(Position);

private:
    enum CallNumber { NoCall, FirstCall, NextCalls };
    enum GradientType { Free = 0, Touched = 1 };
    enum ButtonType { NewGame = 0, Exit = 1 };

    bool m_visible;

    QFont* font;
    QPixmap* image;
    QPolygon* button;
    MyThread* thread;
    QPixmap* m_pixmap;
    CallNumber callNum;
    QLinearGradient* gradient;

    int newGameButtonX;
    int newGameButtonY;

    int exitButtonX;
    int exitButtonY;

    bool mouseOverExitButton;
    bool mouseOverNewGameButton;

    void initGradients();
    QPolygon buttonShape(QPoint);
    void setGradient(QPainter*, GradientType, ButtonType);
    void drawButton( QPainter*, bool/*mouseOverButton*/, ButtonType, const QString&);
};

inline bool StartFace::visible() { return m_visible; }
inline void StartFace::setVisible(bool vsbl)
{
    m_visible = vsbl;
    mouseOverExitButton = false;
    mouseOverNewGameButton = false;
    callNum = ( callNum == NoCall ) ? FirstCall: NextCalls;
}
inline QPixmap StartFace::pixmap()
{
    thread->start();
    return *m_pixmap;
}


#endif // STARTFACE_H


