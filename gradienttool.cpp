#include "gradienttool.h"

#include <QDebug>

#include <math.h>
#include <optional>

#include <QPainter>
#include <QVector3D>
#include <QEasingCurve>
#include <QGuiApplication>


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

    Qt::KeyboardModifiers mods = QGuiApplication::queryKeyboardModifiers();

    easingColor.setEasingCurveType(QEasingCurve::OutBounce);

    qreal accLength = 0;

    for (size_t i = 0; i < lines.size()-1; ++i)
    {
        QLineF line(lines[i], lines[i+1]);

        qreal curLength = line.length()/linesLength;

        QLinearGradient gradient(line.p1(), line.p2());

        gradient.setColorAt(0, easingColor.colorForProgress(accLength));
        gradient.setColorAt(1, easingColor.colorForProgress(accLength+curLength));

        painter->setPen(QPen(gradient, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawLine(line);

//        if (mods & Qt::ControlModifier)
        if (showControlPoints)
        {
            painter->setPen(QPen(QColor(128, 128, 128), 3));
            QRect r(0, 0, penWidth, penWidth);
            r.moveCenter(lines[i]);
            painter->drawEllipse(r);
        }

        accLength += curLength;
    }

    if (hoverPoint)
    {
        QRect occ(QPoint(), QSize(penWidth*1.1, penWidth*1.1));
        occ.moveCenter(*hoverPoint);
        painter->setPen(QPen(QColor(0xff1493), 3));
        painter->drawEllipse(occ);
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

void GradientTool::hoverMoveEvent(QHoverEvent *event)
{
//    std::for_each(lines.begin(), lines.end(), [&event](const QPoint & point)
//    {
//        auto l = event->pos() - point;
//        qDebug() << ": l = " << l;
//        qDebug() << "m = " << l.manhattanLength();
//    });


    if (std::optional<QPoint> nearest = findNearest(event->pos()))
    {
//        qDebug() << "nearest of " << event->pos() << ": " << *nearest
//                 << "manhattan: " << (*nearest - event->pos()).manhattanLength();

        if ((*nearest - event->pos()).manhattanLength() <= penWidth)
        {
            if (hoverPoint != nearest)
            {
                hoverPoint = nearest;
                update();
            }
        }
        else
        {
            if (hoverPoint)
            {
                hoverPoint.reset();
                update();
            }
        }
    }
}


void GradientTool::wheelEvent(QWheelEvent* event)
{
    setPenWidth(penWidth + event->delta()/10.0);
    update();
}

void GradientTool::setPenWidth(qreal newValue)
{
    penWidth = newValue > penWidthMax
            ? penWidthMax : newValue < 1
            ? 1 : newValue;
    emit penWidthChanged();
    update();
}

void GradientTool::setColorBegin(const QColor & newColor)
{
    easingColor.setColorBegin(newColor);
    update();
}

void GradientTool::setColorEnd(const QColor & newColor)
{
    easingColor.setColorEnd(newColor);
    update();
}

qreal GradientTool::getPenWidth() const
{
    return penWidth;
}

bool GradientTool::getShowControlPoints() const
{
    return showControlPoints;
}

QColor GradientTool::getColorBegin() const
{
    return easingColor.getColorBegin();
}

QColor GradientTool::getColorEnd() const
{
    return easingColor.getColorEnd();
}

void GradientTool::setShowControlPoints(bool newValue)
{
    showControlPoints = newValue;
    emit showControlPointsChanged();
    update();
}

std::optional<QPoint> GradientTool::findNearest(const QPoint & other) const
{
    int minManhattan = 1000;
    std::optional<QPoint> nearest;
    for (const QPoint& p: lines)
    {
        auto newManhattan = (p - other).manhattanLength();
        if (newManhattan < minManhattan)
        {
            minManhattan = newManhattan;
            nearest = p;
        }
    }

    return nearest;
}
