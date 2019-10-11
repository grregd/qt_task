#include "gradienttool.h"

#include <QDebug>

#include <math.h>
#include <optional>
#include <algorithm>

#include <QPainter>
#include <QVector3D>
#include <QEasingCurve>
#include <QGuiApplication>


GradientTool::GradientTool()
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setFlag(ItemAcceptsInputMethod, true);

    QLinearGradient gradient(QPointF(50, -20), QPointF(80, 20));
    gradient.setColorAt(0.0, Qt::white);
    gradient.setColorAt(1.0, QColor(0xa6, 0xce, 0x39));
}


void GradientTool::paint(QPainter *painter)
{
    paint7(painter);
}

void GradientTool::addPointAtEnd(const QPoint & point)
{
    if (!lines_points.empty())
    {
        qDebug() << __PRETTY_FUNCTION__ << "addPointAtEnd: "
                 << QLineF(lines_points.back(), point).length();
        linesLength += QLineF(lines_points.back(), point).length();
        qDebug() << "linesLength: " << linesLength;
    }

    lines_points.push_back(point);
}

void GradientTool::removeLastPoint()
{
    if (!lines_points.empty())
    {
        if (lines_points.size() >= 2)
        {
            qDebug() << __PRETTY_FUNCTION__ << "removeLastPoint: "
                     << QLineF(lines_points.back(), *(lines_points.rbegin()+1)).length();
            linesLength -= QLineF(lines_points.back(), *(lines_points.rbegin()+1)).length();
            qDebug() << "linesLength: " << linesLength;
        }

        undoPoints.push_back(lines_points.back());
        lines_points.pop_back();
        update();
    }
}

void GradientTool::redoLastPoint()
{
    if (!undoPoints.empty())
    {
        addPointAtEnd(undoPoints.back());
        undoPoints.pop_back();
    }
    update();
}

void GradientTool::paint7(QPainter *painter)
{

    easingColor.setEasingCurveType(QEasingCurve::Linear);

    qreal accLength = 0;

    if (lines_points.size() >= 2)
    {
        for (size_t i = 0; i < lines_points.size()-1; ++i)
        {
            QLineF line(lines_points[i], lines_points[i+1]);

            qreal curLength = line.length()/linesLength;

            QLinearGradient gradient(line.p1(), line.p2());

            gradient.setColorAt(0, easingColor.colorForProgress(accLength));
            gradient.setColorAt(1, easingColor.colorForProgress(accLength+curLength));

            painter->setPen(QPen(gradient, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->drawLine(line);


            accLength += curLength;
        }
    }

    if (showControlPoints)
    {
        painter->setPen(QPen(QColor(128, 128, 128), 3));
        QRect r(0, 0, std::max(15.0, penWidth), std::max(15.0, penWidth));
        std::for_each(lines_points.begin(), lines_points.end(), [painter, &r](auto & point){
            r.moveCenter(point);
            painter->drawEllipse(r);
        });
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
    if (hoverPoint)
        dragging = true;
}

void GradientTool::mouseReleaseEvent(QMouseEvent *event)
{
    undoPoints.clear();

    addPointAtEnd(event->pos());
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
    for (const QPoint& p: lines_points)
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
