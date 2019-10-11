
#include "brokenline.h"

#include <algorithm>

#include <QtDebug>
#include <QLineF>

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
        for (size_t i = 0; i < points.size()-1; ++i)
        {
            result += QLineF(points[i], points[i+1]).length();
        }
    }

    return result;
}

BrokenLine & BrokenLine::addPointAtEnd(const QPoint & point)
{
    if (!points_.empty())
    {
        length_ += QLineF(points_.back(), point).length();
    }

    points_.push_back(point);

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
    qDebug() << "length: " << length();
    calculateLength1(points_);

    return *this;
}

BrokenLine &BrokenLine::removeAllPoints()
{
    points_.clear();
    length_ = 0;
}

std::optional<QPoint> BrokenLine::findNearest(const QPoint & other) const
{
    int minManhattan = 1000;
    std::optional<QPoint> nearest;
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
