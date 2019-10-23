
#include "brokenline.h"

#include <algorithm>

#include <QtDebug>
#include <QLineF>
#include <QTransform>

QVector<qreal> calculateAccLength(const QVector<BrokenLine::ControlPoint> & points)
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

QLineF BrokenLine::fragment(int startPointIndex) const
{
    return QLineF(points_[startPointIndex].point(), points_[startPointIndex+1].point());
}

qreal BrokenLine::normalizedLength(int startPointIndex) const
{
    return accLength_[startPointIndex] / length_;
}

QLinearGradient BrokenLine::gradientInPoint(int startPointIndex) const
{
    QLineF f(fragment(startPointIndex));
    QLinearGradient result(f.p1(), f.p2());

    result.setColorAt(0, gradient_.colorForProgress(normalizedLength(startPointIndex)));
    result.setColorAt(1, gradient_.colorForProgress(normalizedLength(startPointIndex+1)));

    return result;
}

void BrokenLine::updateGradient()
{
    gradient_.stops().clear();

    auto accIt = accLength_.begin();
    for (auto pointIt = points_.begin(); pointIt != points_.end(); ++pointIt, ++accIt)
    {
        if (pointIt->color_)
        {
            gradient_.push_back(QGradientStop(*accIt/accLength_.back(), *pointIt->color_));
        }
    }
}

void BrokenLine::updateLength()
{
    accLength_ = calculateAccLength(points_);
    length_ = accLength_.empty() ? 0.0 : accLength_.back();
}

void BrokenLine::addPoint(const QPoint & point)
{
    qreal lastLength = 0;
    std::optional<QColor> color;

    if (points_.empty())
    {
        color = Qt::cyan;
    }
    else
    {
        lastLength = accLength_.back() + QLineF(points_.back().point(), point).length();

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

    points_.push_back(ControlPoint(point, color));
    accLength_.push_back(lastLength);
    length_ = accLength_.empty() ? 0.0 : accLength_.back();

    updateGradient();
}

void BrokenLine::insertPoint(QVector<ControlPoint>::iterator where, const QPoint &point)
{
    points_.insert(where, point);

    accLength_ = calculateAccLength(points_);
    length_ = accLength_.empty() ? 0.0 : accLength_.back();

    updateGradient();
}


void BrokenLine::removePoint(const QPoint & point)
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

    accLength_ = calculateAccLength(points_);
    length_ = accLength_.empty() ? 0.0 : accLength_.back();
}

void BrokenLine::removeAllPoints()
{
    points_.clear();
    accLength_.clear();
    length_ = 0;

    updateGradient();
}

QVector<BrokenLine::ControlPoint>::iterator BrokenLine::getPointRef(QPoint point)
{
    return std::find_if(points_.begin(), points_.end(), [point](const auto& p){ return p.point() == point; } );
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
