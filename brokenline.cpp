
#include "brokenline.h"

#include <QLineF>

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
