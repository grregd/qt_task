
#include "brokenline.h"

#include <algorithm>

#include <QtDebug>
#include <QLineF>
#include <QTransform>

QVector<qreal> calculateLength1(const QVector<QPoint> & points)
{
    QVector<qreal> result;

    if (points.empty())
    {
        return result;
    }

    result.reserve(points.size());

    QPoint prevPoint(*points.begin());
    qreal accLen = 0;

    std::transform(points.begin(), points.end(), std::back_inserter(result),
        [&prevPoint, &accLen](auto const & point)
        {
            accLen += QLineF(prevPoint, point).length();
            prevPoint = point;
            return accLen;
        }
    );

    return result;
}

qreal calculateLength(const QVector<QPoint> & points)
{
    qreal result = 0;

    if (points.size() >= 2)
    {
        for (int i = 0; i < points.size()-1; ++i)
        {
            result += QLineF(points[i], points[i+1]).length();
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


BrokenLine & BrokenLine::addPointAtEnd(const QPoint & point)
{
    qreal lastLength = 0;

    if (!points_.empty())
    {
        lastLength = accLength_.back() + QLineF(points_.back(), point).length();
//        boundingBoxes_.push_back(calcBoundingBox(QLineF(points_.back(), point), 5.0, 5.0));
    }

    points_.push_back(point);
    accLength_.push_back(lastLength);

    length_ = accLength_.back();

    qDebug() << "points_.size(): " << points_.size();
    qDebug() << "accLength_.size(): " << accLength_.size();
//    qDebug() << "boundingBoxes_.size(): " << boundingBoxes_.size();
    qDebug() << "length_: " << length_;

    return *this;
}

BrokenLine &BrokenLine::addPointAt(QVector<QPoint>::iterator where, const QPoint &point)
{
    points_.insert(where, point);

    length_ = calculateLength(points_);
    accLength_ = calculateLength1(points_);

    return *this;
}


BrokenLine & BrokenLine::removePoint(const QPoint & point)
{
    points_.erase(
        std::remove(points_.begin(),
                    points_.end(),
                    point),
        points_.end());

    length_ = calculateLength(points_);
    accLength_ = calculateLength1(points_);

    return *this;
}

BrokenLine &BrokenLine::removeAllPoints()
{
    points_.clear();
    accLength_.clear();
    length_ = 0;

    return *this;
}

QVector<QPoint>::iterator BrokenLine::getPointRef(QPoint point)
{
    return std::find(points_.begin(), points_.end(), point);
}

std::optional<QPoint> BrokenLine::findNearest(const QPoint & other, std::optional<QPoint> nearest) const
{
    int minManhattan = nearest ? (*nearest - other).manhattanLength() : 1000;
//    std::optional<QPoint> nearest;
    for (const QPoint& p: points_)
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
