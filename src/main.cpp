#include <QApplication>
#include "stupidgame.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("Stupid game");
    StupidGame w;
    w.show();

    return a.exec();
}
