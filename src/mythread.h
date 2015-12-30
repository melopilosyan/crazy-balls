#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include "myTypes.h"


class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject*, MyTypes type = BAseBall );
    void run();

signals:

public slots:

private:
    QObject* obj;
    MyTypes types;
};

#endif // MYTHREAD_H
