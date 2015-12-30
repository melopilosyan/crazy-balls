#include "startface.h"
#include "baseball.h"
#include "mythread.h"
#include "bomb.h"
#include <QMutex>

MyThread::MyThread(QObject* o, MyTypes type):QThread(0),
                                             obj(o),
                                             types( type )
{
}


void MyThread::run()
{
    static int a = 0;
    qDebug() << "MyThread::run" << ++a;
    switch( types ) {
        case STartFace: qDebug() << "STartFace"; break;
        case BOmb: qDebug() << "BOmb"; break;
        default: qDebug() << "BaseBall"; break;
    }

    switch( types ) {
        case STartFace: ((StartFace*)obj)->createNextPixmap(); break;
        case BOmb: ((Bomb*)obj)->createNextPixmap(); break;
        default: ((BaseBall*)obj)->createNextPixmap(); break;
    }

    qDebug() << "MyThread::run" << ++a;
}
