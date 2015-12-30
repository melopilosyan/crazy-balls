#ifndef MYTYPES_H
#define MYTYPES_H

#include <QWidget>
#include <QPointF>
#include <QPixmap>

#include <QtGlobal>
#include <QDebug>

#define BallRadius 35

enum MyTypes { BAseBall, GAmer, OPponent, BOmb, STartFace };
enum Status { NoBomB = 0, BomB = 1 };

typedef QPointF Position;
typedef int Radius;
typedef int Id;

class QLinearGradient;
class QPainter;
class QLabel;
class QImage;


#endif // MYTYPES_H
