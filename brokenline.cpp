
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

QLineF BrokenLine::fragment(int startPointIndex) const
{
    return QLineF(points_[startPointIndex].point(), points_[startPointIndex+1].point());
}

qreal BrokenLine::normalizedLength(int startPointIndex) const
{
    return accLength_[startPointIndex] / length_;
}

QLinearGradient BrokenLine::gradient(int startPointIndex) const
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
            gradient_.stops().push_back(QGradientStop(*accIt/accLength_.back(), *pointIt->color_));
        }
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
