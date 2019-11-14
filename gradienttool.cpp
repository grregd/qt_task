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
    if (selectedPointRef != activeLine->points().end())
    {
        qDebug() << __PRETTY_FUNCTION__ << color;
        if (color.spec() != QColor::Invalid)
        {
            selectedPointRef->color() = color;
            activeLine->updateGradient();
            update();
        }
    }
}

GradientTool::GradientTool()
    : activeLine(nullptr)
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
    infoBox.addInfoLine([this](){ return QString("frame time: %1").
                                              arg(elapsedTime); });
}


QPointF GradientTool::hoverLinePointFromMouse() const
{
    auto l1 = QLineF(hoverSegment->p1(), ::transform(mousePos)).length();
    auto l2 = QLineF(hoverSegment->p2(), ::transform(mousePos)).length();
    return hoverSegment->pointAt(l1/(l1+l2));
}

void GradientTool::paint(QPainter *painter)
{
    elapsedTimer.restart();

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
                  [this, painter](auto & line)
                  {
                    brokenLinePainter.paint(line, activeLine == &line, showControlPoints, penWidth, painter);
                  });

    if (hoverPoint)
    {
        paintControlPointHovered(hoverPoint->first, painter);
    }

    if (hoverSegment)
    {
        painSelectedSegmenttHovered(*hoverSegment, painter);
    }

    if (selectedPointRef != activeLine->points().end())
    {
        paintControlPointSelected(*selectedPointRef, painter);
    }

    elapsedTime = elapsedTimer.restart();
    infoBox.paint(QPoint(std::lround(width()-200), 0), painter);
}

void GradientTool::componentComplete()
{
    QQuickPaintedItem::componentComplete();

    startNewLine();
}

void GradientTool::startNewLine()
{
    lines.push_back(BrokenLine());
    activeLine = &lines.back();
    selectedPointRef = activeLine->points().end();
}

void GradientTool::changeActiveLine(BrokenLine *line)
{
    if (activeLine != line)
    {
        activeLine = line;
        selectedPointRef = activeLine->points().end();
    }
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
    if (activeLine)
    {
        auto line = activeLine;
        auto point = std::adjacent_find(line->points().rbegin(), line->points().rend(),
           [this, &checkPos](const auto & p1, const auto & p2)
           {
               return BrokenLinePainter::calcBoundingBox(QLineF(p1.point(), p2.point()), 0, penWidth)
                     .containsPoint(checkPos, Qt::OddEvenFill);
           });

        if (point != line->points().rend())
        {
            return std::make_optional<QLineF>(point->point(), (point+1)->point());
        }
    }

    return std::optional<QLineF>();
}

void GradientTool::painSelectedSegmenttHovered(const QLineF &fragment, QPainter *painter) const
{
    painter->save();

    painter->setPen(createHoverMarkerPen(hoverMarkerSpec, fragment.center().toPoint()));
    painter->drawPolygon(BrokenLinePainter::calcBoundingBox(fragment, BrokenLinePainter::activeLineBorderOffset, penWidth));

    QRectF r(hoverLinePointFromMouse(), QSizeF());
    r += QMarginsF(penWidth/2.0, penWidth/2.0,
                   penWidth/2.0, penWidth/2.0);
    painter->drawEllipse(r);

    painter->restore();
}

void GradientTool::paintControlPointHovered(const QPoint &point, QPainter *painter) const
{
    painter->save();

    painter->setPen(createHoverMarkerPen(hoverMarkerSpec, point));

    QRectF occ(QPointF(), QSizeF(
                  std::max(15.0, penWidth),
                  std::max(15.0, penWidth)));
    occ.moveCenter(point);
    painter->drawEllipse(occ);

    painter->restore();
}

void GradientTool::paintControlPointSelected(const BrokenLine::ControlPoint &ctrlPoint, QPainter *painter) const
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
            selectedPointRef =
                    hoverPoint->second->getPointRef(hoverPoint->first);
            changeActiveLine(hoverPoint->second);
            emit pointSelectionChanged(selectedPointRef->color()
                                       ? *selectedPointRef->color()
                                       : QColor());
            update();
        }
        else if (hoverSegment)
        {
            auto p1Ref = activeLine->getPointRef(hoverSegment->p1().toPoint());
            activeLine->insertPoint(p1Ref, hoverLinePointFromMouse().toPoint());
            selectedPointRef = p1Ref;
            update();
        }
    }
    else if (event->button() == Qt::RightButton && !hoverPoint)
    {
        startNewLine();
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
            activeLine->addPoint(::transform(event->pos()), defaultColorBegin, defaultColorEnd);
            selectedPointRef = activeLine->points().end()-1;
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (hoverPoint)
        {
            if (event->modifiers() == Qt::ControlModifier)
            {
                if ( !(*hoveredPointRef == activeLine->points().first()) &&
                     !(*hoveredPointRef == activeLine->points().last()) )
                {
                    hoveredPointRef->color().reset();
                    activeLine->updateGradient();
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
        emit requestColorChange(selectedPointRef->color() ?
                                    *selectedPointRef->color() : QColor());
    }
}

void GradientTool::mouseMoveEvent(QMouseEvent *event)
{
//    qDebug() << __PRETTY_FUNCTION__;
    if (event->modifiers() == Qt::ControlModifier &&
        event->buttons() & Qt::LeftButton)
    {
        mouseDragging = true;
        originOffset += event->pos() - lastMouseMovePos;
        lastMouseMovePos = event->pos();

        update();
    }
    else if (mouseLeftPressed)
    {
        mouseDragging = true;
        selectedPointRef->point() = ::transform(event->pos());
        activeLine->updateLength();
        activeLine->updateGradient();
        update();
    }
    mousePos = event->pos();
}

void GradientTool::hoverMoveEvent(QHoverEvent *event)
{
//    qDebug() << __PRETTY_FUNCTION__;
    if (HoverPoint h = findNearestPoint(::transform(event->pos())))
    {
        if (!hoverPoint || hoverPoint->first != h->first)
        {
            hoverPoint = h;
            hoveredPointRef = hoverPoint->second->getPointRef(hoverPoint->first);
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

    mousePos = event->pos();
}


void GradientTool::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() == Qt::ControlModifier)
    {
        changeScale(event->delta(), event->pos());
    }
    else
    {
        setPenWidth(penWidth + event->delta()/10.0);
    }
}

void GradientTool::changeScale(int upDown, const QPoint &scalePosition)
{
    auto oldScale = scale;
    if (upDown < 0 && scale > 0.1)
    {
        scale /= 1.1;
    }
    else if (upDown > 0)
    {
        scale *= 1.1;
    }

    if (!qFuzzyCompare(scale, oldScale))
    {
        originOffset = scalePosition - scale/oldScale * (scalePosition-originOffset);
        update();
    }
}

void GradientTool::setPenWidth(qreal newValue)
{
    if (!qFuzzyCompare(newValue, penWidth))
    {
        auto oldPenWidth = penWidth;

        penWidth = newValue > penWidthMax
                ? penWidthMax : newValue < 1
                ? 1 : newValue;

        if (!qFuzzyCompare(penWidth, oldPenWidth))
        {
            emit penWidthChanged();
            update();
        }
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

QPen GradientTool::createHoverMarkerPen(const QString &penSpec, const QPoint &point) const
{
    if (penSpec.compare("gradient", Qt::CaseInsensitive))
    {
        return QPen(QColor(penSpec), 5, Qt::DotLine);
    }
    else
    {
        QConicalGradient gradient(point, 0);
        gradient.setStops({QGradientStop{0, Qt::black},
                           QGradientStop{1, Qt::white}});
        return QPen(gradient, 5, Qt::DotLine);
    }
}
