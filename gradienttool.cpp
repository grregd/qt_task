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
    if (!line.points().empty())
    {
        qDebug() << __PRETTY_FUNCTION__ << "addPointAtEnd: "
                 << QLineF(line.points().back(), point).length();
        linesLength += QLineF(line.points().back(), point).length();
        qDebug() << "linesLength: " << linesLength;
    }

    line.points().push_back(point);
}

void GradientTool::removeLastPoint()
{
    if (!line.points().empty())
    {
        if (line.points().size() >= 2)
        {
            qDebug() << __PRETTY_FUNCTION__ << "removeLastPoint: "
                     << QLineF(line.points().back(), *(line.points().rbegin()+1)).length();
            linesLength -= QLineF(line.points().back(), *(line.points().rbegin()+1)).length();
            qDebug() << "linesLength: " << linesLength;
        }

        undoPoints.push_back(line.points().back());
        line.points().pop_back();
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

    line.colors().setEasingCurveType(QEasingCurve::Linear);

    qreal accLength = 0;

    if (line.points().size() >= 2)
    {
        for (size_t i = 0; i < line.points().size()-1; ++i)
        {
            QLineF fragment(line.points()[i], line.points()[i+1]);

            qreal curLength = fragment.length()/linesLength;

            QLinearGradient gradient(fragment.p1(), fragment.p2());

            gradient.setColorAt(0, line.colors().colorForProgress(accLength));
            gradient.setColorAt(1, line.colors().colorForProgress(accLength+curLength));

            painter->setPen(QPen(gradient, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->drawLine(fragment);


            accLength += curLength;
        }
    }

    if (showControlPoints)
    {
        painter->setPen(QPen(QColor(128, 128, 128), 3));
        QRect r(0, 0, std::max(15.0, penWidth), std::max(15.0, penWidth));
        std::for_each(line.points().begin(), line.points().end(), [painter, &r](auto & point){
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
    line.colors().setColorBegin(newColor);
    update();
}

void GradientTool::setColorEnd(const QColor & newColor)
{
    line.colors().setColorEnd(newColor);
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
    return line.colors().getColorBegin();
}

QColor GradientTool::getColorEnd() const
{
    return line.colors().getColorEnd();
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
    for (const QPoint& p: line.points())
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
