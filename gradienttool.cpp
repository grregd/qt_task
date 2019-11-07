#include "gradienttool.h"
#include "multigradient.h"

#include <QDebug>

#include <math.h>
#include <optional>
#include <algorithm>

#include <QPoint>
#include <QPainter>
#include <QVector3D>
#include <QEasingCurve>
#include <QGuiApplication>
#include <QStaticText>

static QTransform tra;

QPoint transform(const QPoint & p)
{
    return tra.map(p);
}

QPointF transform(const QPointF & p)
{
    return tra.map(p);
}

void GradientTool::setColorOfSelectedPoint(const QColor &color)
{
    if (selectedPointIterator != line_->points().end())
    {
        qDebug() << __PRETTY_FUNCTION__ << color;
        if (color.spec() != QColor::Invalid)
        {
            selectedPointIterator->color() = color;
            line_->updateGradient();
            update();
        }
    }
}

GradientTool::GradientTool()
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setFlag(ItemAcceptsInputMethod, true);

    setupInfoBox();
}

void GradientTool::setupInfoBox()
{
    infoBox.addInfoLine([this](){ return QString("scale: %1").arg(scale); });
    infoBox.addInfoLine([this](){ return QString("originOffset: %1, %2 (%3, %4)").
                                              arg(originOffset.rx()).
                                              arg(originOffset.ry()).
                                              arg(::transform(originOffset).rx()).
                                              arg(::transform(originOffset).ry()); });
    infoBox.addInfoLine([this](){ return QString("lastMouseMovePos: %1, %2 (%3, %4)").
                                              arg(lastMouseMovePos.rx()).
                                              arg(lastMouseMovePos.ry()).
                                              arg(::transform(lastMouseMovePos).rx()).
                                              arg(::transform(lastMouseMovePos).ry()); });
    infoBox.addInfoLine([this](){ return QString("mousePos: %1, %2 (%3, %4)").
                                              arg(mousePos.rx()).
                                              arg(mousePos.ry()).
                                              arg(::transform(mousePos).rx()).
                                              arg(::transform(mousePos).ry()); });
    infoBox.addInfoLine([this](){ return QString("width, height: %1, %2 (%3, %4)").
                                              arg(width()).
                                              arg(height()).
                                              arg(width()*scale).
                                              arg(height()*scale); });
}


QPointF GradientTool::hoverLinePointFromMouse() const
{
    auto l1 = QLineF(hoverSegment->p1(), ::transform(mousePos)).length();
    auto l2 = QLineF(hoverSegment->p2(), ::transform(mousePos)).length();
    return hoverSegment->pointAt(l1/(l1+l2));
}

void GradientTool::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->translate(originOffset);
    painter->scale(scale, scale);

    bool invertible = false;
    tra = painter->combinedTransform().inverted(&invertible);
    if (!invertible)
    {
        tra = QTransform();
    }

    std::for_each(lines.begin(), lines.end(),
                  [this, painter](auto & line){
                    GradientTool::paintBrokenLine(line, painter);});

    if (hoverPoint)
    {
        paintHoverSelectedControlPoint(hoverPoint->first, painter);
    }

    if (hoverSegment)
    {
        paintHoverSelectedSegment(*hoverSegment, painter);
    }

    if (selectedPointIterator != line_->points().end())
    {
        paintSelectedControlPoint(*selectedPointIterator, painter);
    }

    infoBox.paint(QPoint(std::lround(width()-200), 0), painter);
}

void GradientTool::finishCurrentLine()
{
    lines.push_back(BrokenLine());
    line_ = &lines.back();
    emit penWidthChanged();
}

void GradientTool::changeActiveLine(BrokenLine *line)
{
    line_ = line;
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

static const auto activeLineBorderWidth = 10;
static const auto activeLineBorderOffset = activeLineBorderWidth/2 + 0;

void GradientTool::paintHoverSelectedSegment(const QLineF &fragment, QPainter *painter) const
{
    painter->save();

    painter->setPen(QPen(hoverSelectColor, 4, Qt::DotLine));
    painter->drawPolygon(calcBoundingBox(fragment, activeLineBorderOffset, penWidth));

    QRectF r(hoverLinePointFromMouse(), QSizeF());
    r += QMarginsF(penWidth/2.0, penWidth/2.0,
                   penWidth/2.0, penWidth/2.0);
    painter->drawEllipse(r);

    painter->restore();
}

void GradientTool::paintHoverSelectedControlPoint(const QPoint &point, QPainter *painter) const
{
    painter->save();

    QRectF occ(QPointF(), QSizeF(
                  std::max(15.0, penWidth),
                  std::max(15.0, penWidth)));
    occ.moveCenter(point);
    painter->setPen(QPen(hoverSelectColor, 4, Qt::DotLine));
    painter->drawEllipse(occ);

    painter->restore();
}

void GradientTool::paintSelectedControlPoint(const BrokenLine::ControlPoint &ctrlPoint, QPainter *painter) const
{
    painter->save();

    QRectF occ(QPointF(), QSizeF(
                  std::max(15.0, penWidth),
                  std::max(15.0, penWidth)));
    occ.moveCenter(ctrlPoint.point());
    occ += QMargins(10, 10, 10, 10);

    painter->setPen(QPen(QColor(Qt::gray), 12));
    painter->drawEllipse(occ);

    if (ctrlPoint.color())
    {
        painter->setPen(QPen(*ctrlPoint.color(), 8, Qt::DashLine));
        painter->drawEllipse(occ);
    }

    painter->restore();
}

void GradientTool::paintLineBorder(const BrokenLine &line, QPainter *painter) const
{
    const QMarginsF arcMargin(activeLineBorderOffset, activeLineBorderOffset,
                              activeLineBorderOffset, activeLineBorderOffset);
    const QPen hoveredLinePen(QColor(0xff0090), activeLineBorderWidth,
                        Qt::SolidLine, Qt::MPenCapStyle, Qt::MPenJoinStyle);
    QVector<QPoint> boundingPolygon;
    QVector<QPoint>::iterator left = boundingPolygon.begin();
    QPolygon prevBb;

    for (int i = 0; i < line.points().size()-1; ++i)
    {
        auto bb = calcBoundingBox(line.fragment(i), activeLineBorderOffset, penWidth);
        painter->setPen(hoveredLinePen);

        if (prevBb.isEmpty())
        {
            left = boundingPolygon.insert(left, bb[0]);
        }
        else
        {
            QPointF intersectionPoint;
            auto itype = QLineF(bb[0], bb[1]).
                    intersect(QLineF(prevBb[0], prevBb[1]),
                    &intersectionPoint);
            if (itype == QLineF::BoundedIntersection)
            {
                *left = intersectionPoint.toPoint();
                left = boundingPolygon.insert(left, intersectionPoint.toPoint());
            }
            else if (itype == QLineF::UnboundedIntersection)
            {
                left = boundingPolygon.insert(left, bb[0]);
            }
            else
            {
                qDebug() << itype;
            }
        }
        left = boundingPolygon.insert(left, bb[1]);


        if (prevBb.isEmpty())
        {
            boundingPolygon.push_back(bb[3]);
        }
        else
        {
            QPointF intersectionPoint;
            auto itype = QLineF(bb[2], bb[3]).
                    intersect(QLineF(prevBb[2], prevBb[3]),
                    &intersectionPoint);
            if (itype == QLineF::BoundedIntersection)
            {
                boundingPolygon.back() = intersectionPoint.toPoint();
                boundingPolygon.push_back(intersectionPoint.toPoint());
            }
            else if (itype == QLineF::UnboundedIntersection)
            {
                boundingPolygon.push_back(bb[3]);
            }
            else
            {
                qDebug() << itype;
            }
        }
        boundingPolygon.push_back(bb[2]);

        prevBb = bb;
    }


    std::reverse(left, boundingPolygon.end());
//    painter->setPen(hoveredLinePen);
//    painter->setPen(QColor(Qt::black));
//    painter->drawPolyline(boundingPolygon);


//    painter->setPen(QPen(Qt::black, 2));

    for (auto itPolyPoint = boundingPolygon.begin(); itPolyPoint != boundingPolygon.end(); ++itPolyPoint)
    {
//        painter->drawText(*itPolyPoint, QString("%1").arg(itPolyPoint - boundingPolygon.begin()));

        if (itPolyPoint == boundingPolygon.begin() ||
            itPolyPoint == boundingPolygon.rbegin().base())
        {
            continue;
        }

        auto cntPolyPoint = boundingPolygon.size();
        auto cntHalfPolyPoint = cntPolyPoint / 2;
        const int indexPolyPoint = std::distance(boundingPolygon.begin(), itPolyPoint);
        const int indexCtrlPoint = ((indexPolyPoint < cntHalfPolyPoint)
                              ? (indexPolyPoint+1)/2
                              : (cntPolyPoint-indexPolyPoint)/2);

        if (indexPolyPoint % 2 == 1)
        {
            if (*itPolyPoint != *(itPolyPoint+1) &&
                indexCtrlPoint != 0 &&
                indexCtrlPoint < line.points().size()-1)
            {
                auto const & currentCtrlPoint = line.points()[indexCtrlPoint].point();
                auto const & prevCtrlPoint = line.points()[indexCtrlPoint-1].point();
                auto const & nextCtrlPoint = line.points()[indexCtrlPoint+1].point();
                const QLineF prevCtrlLine(currentCtrlPoint, prevCtrlPoint);
                const QLineF nextCtrlLine(currentCtrlPoint, nextCtrlPoint);

                QRectF arcRect(0, 0, penWidth, penWidth);
                arcRect.moveCenter(currentCtrlPoint);
                arcRect += arcMargin;

                if (nextCtrlLine.angleTo(prevCtrlLine) > 180)
                {
                    painter->drawArc(arcRect,
                                     std::lround((prevCtrlLine.angle()+270)*16),
                                     std::lround((180-nextCtrlLine.angleTo(prevCtrlLine))*16));
                }
                else
                {
                    painter->drawArc(arcRect,
                                     std::lround((nextCtrlLine.angle()+270)*16),
                                     std::lround((180-prevCtrlLine.angleTo(nextCtrlLine))*16));
                }
            }

            painter->drawLine(*(itPolyPoint-1), *itPolyPoint);
        }
    }
}

void GradientTool::paintBrokenLine(const BrokenLine &line, QPainter *painter) const
{
    if (line.points().size() >= 2)
    {
        if (line_ == &line)
        {
            paintLineBorder(line, painter);
        }

        for (int i = 0; i < line.points().size()-1; ++i)
        {
            painter->setPen(QPen(QLinearGradient(line.gradientInPoint(i)),
                                 penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->drawLine(line.fragment(i));
        }
    }

    if (showControlPoints)
    {
        painter->setPen(QPen(QColor(128, 128, 128), 3));
        QRectF r(0, 0, std::max(15.0, penWidth), std::max(15.0, penWidth));
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
//    qDebug() << __PRETTY_FUNCTION__;

    if (event->button() == Qt::LeftButton)
    {
        lastMouseMovePos = event->pos();

        if (event->modifiers() == Qt::ControlModifier)
        {
            return ;
        }

        if (hoverPoint)
        {
            mouseLeftPressed = true;
            selectedPointIterator =
                    hoverPoint->second->getPointRef(hoverPoint->first);
            changeActiveLine(hoverPoint->second);
            emit pointSelectionChanged(selectedPointIterator->color()
                                       ? *selectedPointIterator->color()
                                       : QColor());
            update();
        }
        else if (hoverSegment)
        {
            auto p1Ref = line_->getPointRef(hoverSegment->p1().toPoint());
            line_->insertPoint(p1Ref, hoverLinePointFromMouse().toPoint());
            update();
        }
    }
    else if (event->button() == Qt::RightButton && !hoverPoint)
    {
        finishCurrentLine();
    }
}

void GradientTool::mouseReleaseEvent(QMouseEvent *event)
{
//    qDebug() << __PRETTY_FUNCTION__;
    if (event->button() == Qt::LeftButton)
    {
        if (mouseDragging)
        {
            mouseDragging = false;
        }
        else if (!hoverPoint && !hoverSegment)
        {
            line_->addPoint(::transform(event->pos()));
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (hoverPoint)
        {
            if (event->modifiers() == Qt::ControlModifier)
            {
                if ( !(*hoverPointIterator == line_->points().first()) &&
                     !(*hoverPointIterator == line_->points().last()) )
                {
                    hoverPointIterator->color().reset();
                    line_->updateGradient();
                    update();
                }
            }
            else
            {
                hoverPoint->second->removePoint(hoverPoint->first);
                hoverPoint.reset();
            }
        }
    }

    mouseLeftPressed = false;

    update();
}

void GradientTool::mouseDoubleClickEvent(QMouseEvent *event)
{
//    qDebug() << __PRETTY_FUNCTION__;
    if (event->button() == Qt::LeftButton && hoverPoint)
    {
        emit requestColorChange(selectedPointIterator->color() ?
                                    *selectedPointIterator->color() : QColor());
    }
}

void GradientTool::mouseMoveEvent(QMouseEvent *event)
{
//    qDebug() << __PRETTY_FUNCTION__;
    if (event->modifiers() == Qt::ControlModifier &&
        event->buttons() & Qt::LeftButton/*mouseLeftPressed*/)
    {
        mouseDragging = true;
        originOffset += event->pos() - lastMouseMovePos;
        lastMouseMovePos = event->pos();

        update();
    }
    else if (mouseLeftPressed)
    {
        mouseDragging = true;
        selectedPointIterator->point() = ::transform(event->pos());
        line_->updateLength();
        line_->updateGradient();
        update();
    }
    mousePos = event->pos();
}

void GradientTool::hoverMoveEvent(QHoverEvent *event)
{
//    qDebug() << __PRETTY_FUNCTION__;
    if (!mouseLeftPressed)
    {
        if (HoverPoint h = findNearestPoint(::transform(event->pos())))
        {
            if (!hoverPoint || hoverPoint->first != h->first)
            {
                hoverPoint = h;
                hoverPointIterator = hoverPoint->second->getPointRef(hoverPoint->first);
                hoverSegment.reset();
                update();
            }
        }
        else
        {
            hoverPoint.reset();
            hoverSegment = findHoverLine(::transform(event->pos()));
            update();
        }

    }
    mousePos = event->pos();
}


void GradientTool::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        auto oldScale = scale;
        if (event->delta() < 0 && scale > 0.1)
        {
            scale /= 1.1;
        }
        else if (event->delta() > 0)
        {
            scale *= 1.1;
        }

        if (scale != oldScale)
        {
            originOffset = event->pos() - scale/oldScale * (event->pos()-originOffset);
            update();
        }
    }
    else
    {
        setPenWidth(penWidth + event->delta()/10.0);
        update();
    }
}

void GradientTool::componentComplete()
{
    QQuickPaintedItem::componentComplete();

    lines.push_back(BrokenLine());
    line_ = &lines.back();
    selectedPointIterator = line_->points().end();
}

void GradientTool::setPenWidth(qreal newValue)
{
    if (!qFuzzyCompare(newValue, penWidth))
    {
        penWidth = newValue > penWidthMax
                ? penWidthMax : newValue < 1
                ? 1 : newValue;
        emit penWidthChanged();
        update();
    }
}

qreal GradientTool::getPenWidth() const
{
    return penWidth;
}

bool GradientTool::getShowControlPoints() const
{
    return showControlPoints;
}

void GradientTool::setShowControlPoints(bool newValue)
{
    if (showControlPoints != newValue)
    {
        showControlPoints = newValue;
        emit showControlPointsChanged();
        update();
    }
}

