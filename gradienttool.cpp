#include "gradienttool.h"

#include <QDebug>

#include <math.h>
#include <QPainter>
#include <QVector3D>
#include <QEasingCurve>

qreal penWidth = 4.0;

GradientTool::GradientTool()
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setFlag(ItemAcceptsInputMethod, true);
//    setAcceptTouchEvents(true);

    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
//    QLinearGradient gradient(QPointF(0, 0), QPointF(0, 480));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));
//    gradient.setCoordinateMode(QGradient::StretchToDeviceMode);

    circlePen = QPen(/*Qt::black*/gradient, penWidth);
    circlePen.setWidth(5);

    circleBrush = QBrush(gradient);
}

static QVector<QPoint> lines;
static qreal linesLength = 0;
//{
//    QPoint{0, 0},
//    QPoint{100, 100},
//    QPoint{100, 200},
//    QPoint{200, 300},
//};

void paint5(QPainter *painter);

void GradientTool::paint(QPainter *painter)
{
    paint7(painter);
}

void GradientTool::paint6(QPainter *painter)
{
    if (lines.size()<2)
        return ;

    qreal prevColorLevel = 0;
    qreal color = 0;

    //valueForProgress
    for (size_t i = 0; i < lines.size()-1; ++i)
    {
        QLineF line(lines[i], lines[i+1]);
        qreal colorSpread = 255.0*line.length()/linesLength;
        qDebug() << "colorLevel: " << colorSpread;

        QLinearGradient gradient(line.p1(), line.p2());
        gradient.setColorAt(0, QColor(color, color, color));
        gradient.setColorAt(1, QColor(color+colorSpread, color+colorSpread, color+colorSpread));

        painter->setPen(QPen(gradient, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawLine(line);

        prevColorLevel = colorSpread;
        color += colorSpread;
    }
}

void GradientTool::paint7(QPainter *painter)
{
    qDebug() << "GradientTool::paint7";

    if (lines.size()<2)
        return ;

    QEasingCurve easingColor(QEasingCurve::InOutQuad);
    qreal accLength = 0;

    for (size_t i = 0; i < lines.size()-1; ++i)
    {
        QLineF line(lines[i], lines[i+1]);
        qreal curLength = line.length()/linesLength;
        qreal colorStart = 255.0*easingColor.valueForProgress(accLength);
        qreal colorStop = 255.0*easingColor.valueForProgress(accLength+curLength);
        qDebug() << "colorStart: " << colorStart;
        qDebug() << "colorStop: " << colorStop;

        QLinearGradient gradient(line.p1(), line.p2());
        gradient.setColorAt(0, QColor(colorStart, colorStart, colorStart));
        gradient.setColorAt(1, QColor(colorStop, colorStop, colorStop));

        painter->setPen(QPen(gradient, 10, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawLine(line);

        accLength += curLength;
    }

}


void GradientTool::myDrawLine(QPainter *painter, const QPoint &p1, const QPoint &p2)
{
    QColor color(0, 0, 0);
    QBrush colorBrush(color);

    QPoint prevP = p1;
    qreal length = (p2.x() - p1.x());

    QVector3D colorComponents(0, 0, 0);


    for (qreal x = p1.x(); x <= p2.x(); ++x)
    {
        qreal y = p1.y() + (x - p1.x())/(p2.x() - p1.x())*(p2.y() - p1.y());

        colorComponents += QVector3D(1, 1, 1);

        painter->setBrush(colorBrush);
        painter->setPen(QPen(QColor(colorComponents.x(), colorComponents.y(), colorComponents.z()), 5));
//        painter->drawEllipse(x, y, 5, 5);
        painter->drawLine(prevP, QPoint(x, y));

        prevP = QPoint(x, y);
    }
}


void GradientTool::paint0(QPainter *painter)
{
    if (lines.size() < 2)
        return;

    painter->setRenderHint(QPainter::Antialiasing);

    for (size_t i = 0; i < lines.size()-1; ++i )
    {
        myDrawLine(painter, lines[i], lines[i+1]);
    }
}

void GradientTool::paint1(QPainter *painter)
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

void GradientTool::paint2(QPainter *painter)
{
    painter->save();

    QLinearGradient gradient(QPointF(8, 10), QPointF(8, 30));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0x0e, 0x39));


    painter->drawLine(10, 0, 10, 40);
    painter->drawLine(20, 0, 20, 40);


    QPen pen(gradient, 10.0);
    painter->setPen(pen);

    painter->drawLine(10, 10, 10, 30);
    painter->drawLine(20, 10, 20, 30);
    painter->drawLine(30, 10, 30, 30);

    painter->rotate(60);

    painter->drawLine(40, 10, 40, 30);

    painter->restore();
}

void GradientTool::paint3(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QColor color(0, 0, 0);
    QBrush colorBrush(color);

    for (int i = 0; i < max; ++i)
    {
        color.setRgb(i*255/max, i*255/max, i*255/max);
        colorBrush.setColor(color);

        painter->setBrush(colorBrush);
        painter->setPen(color);

        qreal r = i/25;
        qreal x = r * std::sin(i/100.0);
        qreal y = r * std::cos(i/100.0);

        painter->drawEllipse(x+100, y+100, 10, 10);

    }
}




void paint5(QPainter *painter)
{
    {
        QLinearGradient gradient(QPointF(0, 0), QPointF(1, 100));
        gradient.setColorAt(0, QColor(100, 100, 100));
        gradient.setColorAt(1, QColor(20, 20, 20));

        QBrush brush(gradient);
        painter->setBrush(brush);
        painter->translate(200, 200);
        painter->setPen(QPen(gradient, 30, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    //    painter->rotate(-45);
    //    painter->rotate(-45);

    //    painter->drawRect(-100, -100, 400, 400);
        painter->drawLine(0, 0, 1, 100);
    }

    {
        QLinearGradient gradient(QPointF(0, 0), QPointF(200, 200));
        gradient.setColorAt(0, QColor(30, 30, 30));
        gradient.setColorAt(1, QColor(0, 0, 0));

        painter->setPen(QPen(gradient, 30, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawLine(1, 100, 200, 200);
    }

}




//void GradientTool::addPoint(int x, int y)
//{
//    lines.push_back(QPoint{x, y});
//    update();
//}

void GradientTool::mousePressEvent(QMouseEvent *event)
{
    if (!lines.empty())
    {
        linesLength += QLineF(lines.back(), event->pos()).length();
    }

    lines.push_back(event->pos());
    update();
}

void GradientTool::wheelEvent(QWheelEvent* event)
{
    qDebug() << "GradientTool::wheelEvent(QWheelEvent* event): "
             << max << " + "  << event->delta();
    max += event->delta();
    qDebug() << " = " << max;

    update();
}

