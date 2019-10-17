#ifndef BROKENLINE_H
#define BROKENLINE_H

#include "easingcolor.h"

#include <optional>

#include <QVector>
#include <QPoint>
#include <QPolygon>


class BrokenLine
{
    void updateGradient();

public:
    QVector<QPoint> points_;
    QVector<qreal> accLength_;
//    QVector<QPolygon> boundingBoxes_;
    EasingColor colors_;
    qreal length_ = 0;
    // colorOfPoints vector corresponds to points_ vector
    QVector<std::optional<QColor>> colorOfPoints;
    MultiGradient gradient_;

public:
    QVector<QPoint>& points() { return points_; }
    QVector<QPoint> const & points() const { return points_; }

    QVector<qreal>& accLength() { return accLength_; }
    QVector<qreal> const & accLength() const { return accLength_; }

    EasingColor& colors() { return colors_; }
    EasingColor const & colors() const { return colors_; }

    qreal & length() { return length_; }
    qreal const & length() const { return length_; }

    MultiGradient const & gradient() const { return gradient_; }

    BrokenLine & addPointAtEnd(const QPoint & point);
    BrokenLine & addPointAt(QVector<QPoint>::iterator where, const QPoint & point);
    BrokenLine & removePoint(const QPoint & point);
    BrokenLine & removeAllPoints();

    QVector<QPoint>::iterator getPointRef(QPoint point);
    void setPointColor(QVector<QPoint>::iterator pointRef, const QColor &color);

    std::optional<QPoint> findNearest(const QPoint & other, std::optional<QPoint> nearest) const;
};

#endif // BROKENLINE_H
