#include "brokenline.h"

BrokenLine & BrokenLine::removePoint(const QPoint & point)
{
    points_.erase(
        std::remove(points_.begin(),
                    points_.end(),
                    point),
        points_.end());

    return *this;
}
