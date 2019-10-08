#include "gradienttool.h"

#include <QPainter>

qreal penWidth = 4.0;

GradientTool::GradientTool()
{
    setAcceptedMouseButtons(Qt::AllButtons);
//    setAcceptTouchEvents(true);

//    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    QLinearGradient gradient(QPointF(0, 0), QPointF(0, 480));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));
//    gradient.setCoordinateMode(QGradient::StretchToDeviceMode);

    circlePen = QPen(/*Qt::black*/gradient, penWidth);
    circlePen.setWidth(5);

//    circleBrush = QBrush(gradient);
}

static QVector<QPoint> lines;
//{
//    QPoint{0, 0},
//    QPoint{100, 100},
//    QPoint{100, 200},
//    QPoint{200, 300},
//};

void GradientTool::paint(QPainter *painter)
{
    if (lines.empty())
        return;

    painter->save();

    painter->setRenderHint(QPainter::Antialiasing);

//    painter->translate(100, 100);

//    painter->setBrush(circleBrush);
    painter->setPen(circlePen);
    painter->drawPolyline(lines);
//    painter->drawPoints(lines.data(), lines.size());

    painter->restore();
}


//void GradientTool::addPoint(int x, int y)
//{
//    lines.push_back(QPoint{x, y});
//    update();
//}

void GradientTool::mousePressEvent(QMouseEvent *event)
{
    lines.push_back(event->pos());
    update();
}


