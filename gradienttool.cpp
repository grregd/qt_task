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
}


void GradientTool::paint(QPainter *painter)
{
    paint7(painter);
}

void GradientTool::finishCurrentLine()
{
    lines.push_back(line);
}

void GradientTool::removeLastPoint()
{
    if (!line.points().empty())
    {
        if (line.points().size() >= 2)
        {
            line.length() -= QLineF(line.points().back(), *(line.points().rbegin()+1)).length();
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
        line.addPointAtEnd(undoPoints.back());
        undoPoints.pop_back();
    }
    update();
}

void GradientTool::paint7(QPainter *painter)
{
    if (line.points().size() >= 2)
    {
        for (int i = 0; i < line.points().size()-1; ++i)
        {
            QLineF fragment(line.points()[i], line.points()[i+1]);

            QLinearGradient gradient(fragment.p1(), fragment.p2());

            gradient.setColorAt(0, line.colors().colorForProgress(line.accLength()[i]/line.length()));
            gradient.setColorAt(1, line.colors().colorForProgress(line.accLength()[i+1]/line.length()));

            painter->setPen(QPen(gradient, penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->drawLine(fragment);
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
    if (event->button() == Qt::LeftButton && hoverPoint)
    {
        // startDragging();
        dragging = true;
    }
    else if (event->button() == Qt::RightButton)
    {
        finishCurrentLine();
    }
}

void GradientTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        undoPoints.clear();

        line.addPointAtEnd(event->pos());
    }
    else if (event->button() == Qt::RightButton)
    {
        if (hoverPoint)
        {
            line.removePoint(*hoverPoint);
        }
    }
    update();
}

void GradientTool::hoverMoveEvent(QHoverEvent *event)
{
    if (auto nearest = line.findNearest(event->pos()))
    {
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

