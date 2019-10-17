
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


void BrokenLine::updateGradient()
{
    qDebug() << __PRETTY_FUNCTION__;

    gradient_.stops().clear();

    for (auto it = colorOfPoints.begin(); it != colorOfPoints.end(); ++it)
    {
        if (*it)
        {
            auto i = std::distance(colorOfPoints.begin(), it);
            gradient_.stops().push_back(QGradientStop(accLength_[i]/accLength_.back(), **it));
            qDebug() << i << ": " << gradient_.stops().back();
        }
    }
}

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

//    auto color = colorOfPoints.empty()
//            ? std::make_optional<QColor>(Qt::yellow) : colorOfPoints.size() == 1
//            ? std::make_optional<QColor>(Qt::blue)
//            : std::make_optional<QColor>();
    colorOfPoints.push_back(/*color*/QColor());
//    qDebug() << "addPointAtEnd - color: " << *color;

    if (colorOfPoints.size() > 2)
    {
        // swap two last elements to keep old last color as last still
        std::iter_swap(*colorOfPoints.rbegin(), *(colorOfPoints.rbegin()-1));
    }

    updateGradient();

    return *this;
}

BrokenLine &BrokenLine::addPointAt(QVector<QPoint>::iterator where, const QPoint &point)
{
    colorOfPoints.insert(std::distance(points_.begin(), where), std::make_optional<QColor>());
    points_.insert(where, point);

    length_ = calculateLength(points_);
    accLength_ = calculateLength1(points_);

    updateGradient();

    return *this;
}


BrokenLine & BrokenLine::removePoint(const QPoint & point)
{
    auto where = std::find(
                points_.begin(),
                points_.end(),
                point);

    if (where != points_.end())
    {
        colorOfPoints.remove(std::distance(points_.begin(), where));
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
    colorOfPoints.clear();
    updateGradient();
    length_ = 0;

    return *this;
}

QVector<QPoint>::iterator BrokenLine::getPointRef(QPoint point)
{
    return std::find(points_.begin(), points_.end(), point);
}

void BrokenLine::setPointColor(QVector<QPoint>::iterator pointRef, const QColor & color)
{
    colorOfPoints[std::distance(points_.begin(), pointRef)] =
            std::make_optional<QColor>(color);
    updateGradient();
}

std::optional<QPoint> BrokenLine::findNearest(const QPoint & other, std::optional<QPoint> nearest) const
{
    int minManhattan = nearest ? (*nearest - other).manhattanLength() : 1000;
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
