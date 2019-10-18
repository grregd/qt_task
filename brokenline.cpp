
#include "brokenline.h"

#include <algorithm>

#include <QtDebug>
#include <QLineF>
#include <QTransform>

QVector<qreal> calculateLength1(const QVector<BrokenLine::ControlPoint> & points)
{
    QVector<qreal> result;

    if (points.empty())
    {
        return result;
    }

    result.reserve(points.size());

    BrokenLine::ControlPoint prevPoint(*points.begin());
    qreal accLen = 0;

    std::transform(points.begin(), points.end(), std::back_inserter(result),
        [&prevPoint, &accLen](auto const & point)
        {
            accLen += QLineF(prevPoint.point(), point.point()).length();
            prevPoint = point;
            return accLen;
        }
    );

    return result;
}

qreal calculateLength(const QVector<BrokenLine::ControlPoint> & points)
{
    qreal result = 0;

    if (points.size() >= 2)
    {
        for (int i = 0; i < points.size()-1; ++i)
        {
            result += QLineF(points[i].point(), points[i+1].point()).length();
        }
    }

    return result;
}

//QPolygon calcBoundingBox(const QLineF & line, qreal margin, qreal penWidth)
//{
//    QRectF bb(line.p1(), QSizeF(line.length(), penWidth));

//    bb += QMarginsF(margin, margin, margin, margin);

//    bb.moveCenter(line.center());

//    return QTransform()
//            .translate(bb.center().x(), bb.center().y())
//            .rotate(-line.angle())
//            .translate(-bb.center().x(), -bb.center().y())
//            .mapToPolygon(bb.toRect());
//}


void BrokenLine::updateGradient()
{
    qDebug() << __PRETTY_FUNCTION__;

    gradient_.stops().clear();

    auto accIt = accLength_.begin();
    for (auto pointIt = points_.begin(); pointIt != points_.end(); ++pointIt, ++accIt)
    {
        if (pointIt->color_)
        {
            gradient_.stops().push_back(QGradientStop(*accIt/accLength_.back(), *pointIt->color_));
        }
//        else if (gradient_.stops().size() > 1)
//        {
//            gradient_.stops().back().first = *accIt/accLength_.back();
//            qDebug() << "no color";
//        }
        qDebug() << gradient_.stops().size() << ": " << gradient_.stops().back();
    }
}

void BrokenLine::updateLength()
{
    accLength_ = calculateLength1(points_);
    length_ = accLength_.empty() ? 0.0 : accLength_.back();
}

BrokenLine & BrokenLine::addPointAtEnd(const QPoint & point)
{
    qreal lastLength = 0;
    std::optional<QColor> color;

    if (!points_.empty())
    {
        lastLength = accLength_.back() + QLineF(points_.back().point(), point).length();
//        boundingBoxes_.push_back(calcBoundingBox(QLineF(points_.back(), point), 5.0, 5.0));

        if (points_.size() == 1)
        {
            color = Qt::magenta;
        }
        else
        {
            color = points_.back().color();
            points_.back().color().reset();
        }
    }
    else
    {
        color = Qt::cyan;
    }

    points_.push_back(ControlPoint(point, color));

    accLength_.push_back(lastLength);

    length_ = accLength_.back();

    for (auto i = points_.begin(); i != points_.end(); ++i)
    {
        qDebug() << std::distance(points_.begin(), i);
        qDebug() << "  point: " << i->point();
        if (i->color())
        {
            qDebug() << "  color: " << *(i->color());
        }
        else
        {
            qDebug() << "  color: none";
        }
    }

    updateGradient();

    return *this;
}

BrokenLine &BrokenLine::addPointAt(QVector<ControlPoint>::iterator where, const QPoint &point)
{
    points_.insert(where, point);

    length_ = calculateLength(points_);
    accLength_ = calculateLength1(points_);

    updateGradient();

    return *this;
}


BrokenLine & BrokenLine::removePoint(const QPoint & point)
{
    auto where = std::find_if( points_.begin(), points_.end(),
                [point](const auto& p) { return p.point() == point; } );

    if (where != points_.end())
    {
        if (points_.size() > 1)
        {
            if (std::distance(where, points_.rbegin().base()) == 1 )
            {
                (where-1)->color() = where->color();
            }
        }

        points_.erase(where);

        updateGradient();
    }

    length_ = calculateLength(points_);
    accLength_ = calculateLength1(points_);


    return *this;
}

BrokenLine &BrokenLine::removeAllPoints()
{
    points_.clear();
    accLength_.clear();
    length_ = 0;

    updateGradient();

    return *this;
}

QVector<BrokenLine::ControlPoint>::iterator BrokenLine::getPointRef(QPoint point)
{
    return std::find_if(points_.begin(), points_.end(), [point](const auto& p){ return p.point() == point; } );
}

void BrokenLine::setPointColor(QVector<ControlPoint>::iterator pointRef, const QColor & color)
{
    colorOfPoints[std::distance(points_.begin(), pointRef)] =
            std::make_optional<QColor>(color);
    updateGradient();
}

std::optional<QPoint> BrokenLine::findNearest(const QPoint & other, std::optional<QPoint> nearest) const
{
    int minManhattan = nearest ? (*nearest - other).manhattanLength() : 1000;
    for (const ControlPoint& p: points_)
    {
        auto newManhattan = (p.point() - other).manhattanLength();
        if (newManhattan < minManhattan)
        {
            minManhattan = newManhattan;
            nearest = p.point();
        }
    }

    return nearest;
}
