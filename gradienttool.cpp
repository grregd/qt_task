#include "gradienttool.h"

#include <QDebug>

#include <math.h>
#include <QPainter>
#include <QVector3D>
#include <QEasingCurve>


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

//    circlePen = QPen(/*Qt::black*/gradient, penWidth);
//    circlePen.setWidth(5);

    circleBrush = QBrush(gradient);
}


void GradientTool::paint(QPainter *painter)
{
    paint7(painter);
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

        painter->setPen(QPen(gradient, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawLine(line);

        accLength += curLength;
    }

}



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
    penWidth += event->delta()/10.0;
    qDebug() << " = " << max;

    update();
}

