#ifndef BROKENLINE_H
#define BROKENLINE_H

#include "multigradient.h"

#include <optional>

#include <QVector>
#include <QPoint>
#include <QPolygon>


class BrokenLine
{

public:
    struct ControlPoint
    {
        QPoint point_;
        std::optional<QColor> color_;

        ControlPoint(const QPoint & point)
            : point_(point)
        {}

        ControlPoint(const QPoint & point, const std::optional<QColor> & color)
            : point_(point)
            , color_(color)
        {}

        bool operator==(const ControlPoint & other) { return point_ == other.point() && color_ == other.color(); }

        QPoint& point() { return point_; }
        const QPoint& point() const { return point_; }

        std::optional<QColor>& color() { return color_; }
        const std::optional<QColor>& color() const { return color_; }
    };

    QVector<ControlPoint> points_;
    QVector<qreal> accLength_;
    qreal length_ = 0;
    MultiGradient gradient_;

public:
    QVector<ControlPoint>& points() { return points_; }
    QVector<ControlPoint> const & points() const { return points_; }

    QLineF fragment(int startPointIndex) const;
    qreal normalizedLength(int startPointIndex) const;
    QLinearGradient gradientInPoint(int startPointIndex) const;

//    MultiGradient const & gradient() const { return gradient_; }
    void updateGradient();
    void updateLength();

    void addPoint(const QPoint & point);
    void insertPoint(QVector<ControlPoint>::iterator where, const QPoint & point);
    void removePoint(const QPoint & point);
    void removeAllPoints();

    QVector<ControlPoint>::iterator getPointRef(QPoint point);

    std::optional<QPoint> findNearest(const QPoint & other, std::optional<QPoint> nearest) const;
};

#endif // BROKENLINE_H
