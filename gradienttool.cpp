#include "gradienttool.h"
#include "easingcolor.h"

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

//    MultiGradient test;
//    test.colorForProgress(0);
//    test.colorForProgress(1);
//    test.colorForProgress(0.5);
}


QPointF GradientTool::hoverLinePointFromMouse()
{
    auto l1 = QLineF(hoverSegment->p1(), mousePos).length();
    auto l2 = QLineF(hoverSegment->p2(), mousePos).length();
    return hoverSegment->pointAt(l1/(l1+l2));
}

void GradientTool::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

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

    if (hoverSegment)
    {
        painter->setPen(QPen(QColor(0xff1493), 4, Qt::DotLine));
        paintBoundingBox(*hoverSegment, painter);

        QRectF r(hoverLinePointFromMouse(), QSizeF());
        r += QMarginsF(penWidth/2.0, penWidth/2.0,
                       penWidth/2.0, penWidth/2.0);

        painter->drawEllipse(r);
    }
}

void GradientTool::finishCurrentLine()
{
    lines.push_back(BrokenLine());
    line_ = &lines.back();
    line_->colors()
            .setColorBegin(defaultColorBegin)
            .setColorEnd(defaultColorEnd);
    emit penWidthChanged();
    emit colorBeginChanged();
    emit colorEndChanged();
}

void GradientTool::changeActiveLine(BrokenLine *line)
{
    line_ = line;
    emit colorBeginChanged();
    emit colorEndChanged();
}

void GradientTool::removeLastPoint()
{
    if (!line_->points().empty())
    {
        if (line_->points().size() >= 2)
        {
            line_->length() -= QLineF(line_->points().back().point(), (line_->points().rbegin()+1)->point()).length();
        }

//        undoPoints.push_back(line_->points().back());
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

QPolygon calcBoundingBox(const QLineF & line, qreal margin, qreal penWidth)
{
    QRectF bb(line.p1(), QSizeF(line.length(), penWidth));

    bb += QMarginsF(margin, margin, margin, margin);

    bb.moveCenter(line.center());

    return QTransform()
            .translate(bb.center().x(), bb.center().y())
            .rotate(-line.angle())
            .translate(-bb.center().x(), -bb.center().y())
            .mapToPolygon(bb.toRect());
}

GradientTool::HoverPoint GradientTool::findNearestPoint(const QPoint &eventPos)
{
    std::optional<QPoint> nearest;
    for (auto it = lines.rbegin(); it != lines.rend(); ++it)
    {
        BrokenLine & line = *it;
        if ((nearest = line.findNearest(eventPos, nearest)))
        {
            if ((*nearest - eventPos).manhattanLength() <= std::max(15.0, penWidth))
            {
                return std::make_pair(*nearest, &line);
            }
        }
    }

    return HoverPoint();
}

std::optional<QLineF> GradientTool::findHoverLine(const QPoint &checkPos)
{
    {
    for (auto line = lines.rbegin(); line != lines.rend(); ++line )
    {
        auto point = std::adjacent_find(line->points().rbegin(), line->points().rend(),
           [this, &checkPos](const auto & p1, const auto & p2)
           {
               return calcBoundingBox(QLineF(p1.point(), p2.point()), 0, penWidth)
                     .containsPoint(checkPos, Qt::OddEvenFill);
           });

        if (point != line->points().rend())
        {
            return std::make_optional<QLineF>(point->point(), (point+1)->point());
        }

    }
    return std::optional<QLineF>();
    }
}


void GradientTool::paintBoundingBox(const QLineF &fragment, QPainter *painter) const
{
    painter->drawPolygon(calcBoundingBox(fragment, 5.0, penWidth));
}

void GradientTool::paintBrokenLine(const BrokenLine &line, QPainter *painter) const
{
    if (line.points().size() >= 2)
    {
        QPen hoveredLinePen(QColor(0xff0090), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

        for (int i = 0; i < line.points().size()-1; ++i)
        {
            if (line_ == &line)
            {
                painter->setPen(hoveredLinePen);
                paintBoundingBox(QLineF(line.points()[i].point(), line.points()[i+1].point()), painter);
            }
        }

        for (int i = 0; i < line.points().size()-1; ++i)
        {
            QLineF fragment(line.points()[i].point(), line.points()[i+1].point());

            QLinearGradient gradient(fragment.p1(), fragment.p2());

            gradient.setColorAt(0, line.gradient().colorForProgress(line.accLength()[i]/line.length()));
            gradient.setColorAt(1, line.gradient().colorForProgress(line.accLength()[i+1]/line.length()));
//            gradient.setColorAt(0, line.colors().colorForProgress(line.accLength()[i]/line.length()));
//            gradient.setColorAt(1, line.colors().colorForProgress(line.accLength()[i+1]/line.length()));

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
                    r.moveCenter(point.point());
                    if (point.color())
                        painter->setPen(QPen(Qt::black, 6));
                    else
                        painter->setPen(QPen(QColor(128, 128, 128), 3));

                    painter->drawEllipse(r);
                });
    }

}

void GradientTool::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (hoverPoint)
        {
            mouseLeftPressed = true;
            hoverPointIterator = hoverPoint->second->getPointRef(hoverPoint->first);
            changeActiveLine(hoverPoint->second);
        }
        else if (hoverSegment)
        {
            auto p1Ref = line_->getPointRef(hoverSegment->p1().toPoint());
            line_->addPointAt(p1Ref, hoverLinePointFromMouse().toPoint());
        }
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

        if (mouseDragging)
        {
            mouseDragging = false;
        }
        else if (!hoverPoint && !hoverSegment)
        {
            line_->addPointAtEnd(event->pos());
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (hoverPoint)
        {
            hoverPoint->second->removePoint(hoverPoint->first);
            hoverPoint.reset();
        }
    }

    mouseLeftPressed = false;

    update();
}

void GradientTool::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && hoverPoint)
    {
        if (hoverPointIterator->color())
        {
            hoverPointIterator->color().reset();
        }
        else
        {
            hoverPointIterator->color() = QColor(rand()%255, rand()%255, rand()%255, rand()%255);
        }

        line_->updateGradient();
        update();
//        emit requestColorChange(Qt::cyan);
//        hoverPoint->second->setPointColor(
//            hoverPoint->second->getPointRef(
//                    hoverPoint->first), Qt::cyan);
    }
}

void GradientTool::mouseMoveEvent(QMouseEvent *event)
{
    if (mouseLeftPressed)
    {
        mouseDragging = true;
        hoverPointIterator->point() = event->pos();
        line_->updateLength();
        line_->updateGradient();
        update();
    }
    mousePos = event->pos();
}

void GradientTool::hoverMoveEvent(QHoverEvent *event)
{
    if (!mouseLeftPressed)
    {
        if (HoverPoint h = findNearestPoint(event->pos()))
        {
            if (!hoverPoint || hoverPoint->first != h->first)
            {
                hoverPoint = h;
                hoverSegment.reset();
                update();
            }
        }
        else
        {
            hoverPoint.reset();
            hoverSegment = findHoverLine(event->pos());
            update();
        }

    }
    mousePos = event->pos();
}


void GradientTool::wheelEvent(QWheelEvent* event)
{
    setPenWidth(penWidth + event->delta()/10.0);
    update();
}

void GradientTool::componentComplete()
{
    QQuickPaintedItem::componentComplete();

    lines.push_back(BrokenLine());
    line_ = &lines.back();
    line_->colors()
            .setColorBegin(defaultColorBegin)
            .setColorEnd(defaultColorEnd);
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

