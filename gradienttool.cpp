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

    lines.push_back(BrokenLine());
    line_ = &lines.back();
}


void GradientTool::paint(QPainter *painter)
{
    std::for_each(lines.begin(), lines.end(),
                  [this, painter](auto & line){
                    GradientTool::paintBrokenLine(line, painter);});

    if (hoverPoint)
    {
        QRect occ(QPoint(), QSize(
                      std::max(15.0, penWidth),
                      std::max(15.0, penWidth)));
        occ.moveCenter(hoverPoint->first);
        painter->setPen(QPen(QColor(0xff1493), 4, Qt::DotLine));
        painter->drawEllipse(occ);
    }
}

void GradientTool::finishCurrentLine()
{
    lines.push_back(BrokenLine());
    line_ = &lines.back();
    emit penWidthChanged();
    emit colorBeginChanged();
    emit colorEndChanged();
}

void GradientTool::removeLastPoint()
{
    if (!line_->points().empty())
    {
        if (line_->points().size() >= 2)
        {
            line_->length() -= QLineF(line_->points().back(), *(line_->points().rbegin()+1)).length();
        }

        undoPoints.push_back(line_->points().back());
        line_->points().pop_back();
        update();
    }
}

void GradientTool::redoLastPoint()
{
    if (!undoPoints.empty())
    {
        line_->addPointAtEnd(undoPoints.back());
        undoPoints.pop_back();
    }
    update();
}

void GradientTool::paintBrokenLine(const BrokenLine & line, QPainter *painter) const
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
        std::for_each(
                line.points().begin(), line.points().end(),
                [painter, &r](auto & point)
                {
                    r.moveCenter(point);
                    painter->drawEllipse(r);
                });
    }

}

void GradientTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && hoverPoint)
    {
        // startDragging();
        qDebug() << "dragging <- true";
        dragging = true;
        hoverPointIterator = hoverPoint->second->getPoint(hoverPoint->first);
    }
    else if (event->button() == Qt::RightButton && !hoverPoint)
    {
        finishCurrentLine();
    }
}

void GradientTool::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        undoPoints.clear();

        if (!dragging)
        {
            line_->addPointAtEnd(event->pos());
        }
        else
        {
            qDebug() << "dragging <- false";
            dragging = false;
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (hoverPoint)
        {
            hoverPoint->second->removePoint(hoverPoint->first);
            hoverPoint.reset();
            update();
        }
    }
    update();
}

void GradientTool::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging)
    {
        qDebug() << "dragging from" << *hoverPointIterator << " to " << event->pos();
        *hoverPointIterator = event->pos();
        update();
    }
}

void GradientTool::hoverMoveEvent(QHoverEvent *event)
{
    if (!dragging)
    {
        std::optional<QPoint> nearest;
        for (auto it = lines.begin(); it != lines.end(); ++it)
        {
            BrokenLine & line = *it;
            if ((nearest = line.findNearest(event->pos(), nearest)))
            {
                if ((*nearest - event->pos()).manhattanLength() <= penWidth)
                {
                    if (!hoverPoint || hoverPoint->first != nearest)
                    {
                        hoverPoint = std::make_pair(*nearest, &line);
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
    //        else
    //        {
    //            hoverPoint.reset();
    //        }
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
    line_->colors().setColorBegin(newColor);
    update();
}

void GradientTool::setColorEnd(const QColor & newColor)
{
    line_->colors().setColorEnd(newColor);
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
    return line_->colors().getColorBegin();
}

QColor GradientTool::getColorEnd() const
{
    return line_->colors().getColorEnd();
}

void GradientTool::setShowControlPoints(bool newValue)
{
    showControlPoints = newValue;
    emit showControlPointsChanged();
    update();
}

