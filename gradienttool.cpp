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
        selectedPointIterator->color() = color;
        line_->updateGradient();
        update();
    }
}

GradientTool::GradientTool()
{
    setAcceptedMouseButtons(Qt::AllButtons);
    setAcceptHoverEvents(true);
    setFlag(ItemAcceptsInputMethod, true);
}


QPointF GradientTool::hoverLinePointFromMouse()
{
    auto l1 = QLineF(hoverSegment->p1(), ::transform(mousePos)).length();
    auto l2 = QLineF(hoverSegment->p2(), ::transform(mousePos)).length();
    return hoverSegment->pointAt(l1/(l1+l2));
}

void GradientTool::paintInfo(QPainter *painter)
{
    const int infoBoxWidth = 200;
    const int infoLineHeight = 15;

    painter->save();
    painter->resetTransform();
    painter->setPen(QPen());

    painter->drawStaticText(width()-infoBoxWidth, 0*infoLineHeight, QStaticText(
                                (QString("scale: %1").arg(scale))));

    painter->drawStaticText(width()-infoBoxWidth, 1*infoLineHeight, QStaticText(
                                (QString("originOffset: %1, %2 (%3, %4)").
                                 arg(originOffset.rx()).
                                 arg(originOffset.ry()).
                                 arg(::transform(originOffset).rx()).
                                 arg(::transform(originOffset).ry()))));

    painter->drawStaticText(width()-infoBoxWidth, 2*infoLineHeight, QStaticText(
                                (QString("lastMouseMovePos: %1, %2 (%3, %4)").
                                    arg(lastMouseMovePos.rx()).
                                    arg(lastMouseMovePos.ry()).
                                    arg(::transform(lastMouseMovePos).rx()).
                                    arg(::transform(lastMouseMovePos).ry()))));

    painter->drawStaticText(width()-infoBoxWidth, 3*infoLineHeight, QStaticText(
                                (QString("mousePos: %1, %2 (%3, %4)").
                                 arg(mousePos.rx()).
                                 arg(mousePos.ry()).
                                 arg(::transform(mousePos).rx()).
                                 arg(::transform(mousePos).ry())
                                )));

    painter->drawStaticText(width()-infoBoxWidth, 4*infoLineHeight, QStaticText(
                                (QString("width, height: %1, %2 (%3, %4)").
                                 arg(width()).
                                 arg(height()).
                                 arg(width()*scale).
                                 arg(height()*scale)
                                )));

    painter->restore();
}

void GradientTool::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing, true);

    {
        tra.reset();
        tra.translate(originOffset.rx(), originOffset.ry()).scale(scale, scale);
        bool invertible = false;
        tra = tra.inverted(&invertible);
        if (!invertible)
        {
            tra = QTransform();
        }
    }


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
        QRect occ(QPoint(), QSize(
                      std::max(15.0, penWidth),
                      std::max(15.0, penWidth)));
        occ.moveCenter(hoverPoint->first);
        painter->setPen(QPen(hoverSelectColor, 4, Qt::DotLine));
        painter->drawEllipse(occ);
    }

    if (hoverSegment)
    {
        painter->setPen(QPen(hoverSelectColor, 4, Qt::DotLine));
        paintBoundingBox(*hoverSegment, painter);

        QRectF r(hoverLinePointFromMouse(), QSizeF());
        r += QMarginsF(penWidth/2.0, penWidth/2.0,
                       penWidth/2.0, penWidth/2.0);

        painter->drawEllipse(r);
    }

    if (selectedPointIterator != line_->points().end())
    {
        QRect occ(QPoint(), QSize(
                      std::max(15.0, penWidth),
                      std::max(15.0, penWidth)));
        occ.moveCenter(selectedPointIterator->point());
        occ += QMargins(10, 10, 10, 10);

        painter->setPen(QPen(QColor(Qt::gray), 12));
        painter->drawEllipse(occ);

        if (selectedPointIterator->color())
        {
            painter->setPen(QPen(*selectedPointIterator->color(), 8, Qt::DashLine));
            painter->drawEllipse(occ);
        }
    }

    paintInfo(painter);
}

void GradientTool::finishCurrentLine()
{
    lines.push_back(BrokenLine());
    line_ = &lines.back();
    emit penWidthChanged();
//    emit colorBeginChanged();
//    emit colorEndChanged();
}

void GradientTool::changeActiveLine(BrokenLine *line)
{
    line_ = line;
//    emit colorBeginChanged();
//    emit colorEndChanged();
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

static const auto activeLineBorderWidth = 10;
static const auto activeLineBorderOffset = activeLineBorderWidth/2 + 2;

void GradientTool::paintBoundingBox(const QLineF &fragment, QPainter *painter) const
{
    painter->drawPolygon(calcBoundingBox(fragment, activeLineBorderOffset, penWidth));
}

void GradientTool::paintLineBorder(const BrokenLine &line, QPainter *painter) const
{
    const QPen hoveredLinePen(QColor(0xff0090), activeLineBorderWidth,
                        Qt::SolidLine, Qt::MPenCapStyle, Qt::MPenJoinStyle);
    QVector<QPoint> boundingPolygon;
    QVector<QPoint>::iterator left = boundingPolygon.begin();
    QPolygon prevBb;

    for (int i = 0; i < line.points().size()-1; ++i)
    {
        if (line_ == &line)
        {
            painter->setPen(hoveredLinePen);
            auto bb = calcBoundingBox(line.fragment(i), activeLineBorderOffset, penWidth);

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
    }

    std::reverse(left, boundingPolygon.end());
    painter->setPen(hoveredLinePen);
    painter->drawPolyline(boundingPolygon);
}

void GradientTool::paintBrokenLine(const BrokenLine &line, QPainter *painter) const
{
    if (line.points().size() >= 2)
    {

        paintLineBorder(line, painter);

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
    qDebug() << __PRETTY_FUNCTION__;

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
            emit pointSelectionChanged(*selectedPointIterator->color());
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
    qDebug() << __PRETTY_FUNCTION__;
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
    qDebug() << __PRETTY_FUNCTION__;
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
        scale += event->delta()/1000.0;
        if (scale < 0.25)
        {
            scale = 0.25;
        }

        originOffset = event->pos() - scale/oldScale * (event->pos()-originOffset);
    }
    else
    {
        setPenWidth(penWidth + event->delta()/10.0);
    }
    update();
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

