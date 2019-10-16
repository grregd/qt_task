#ifndef BROKENLINE_H
#define BROKENLINE_H

#include "easingcolor.h"

#include <optional>

#include <QVector>
#include <QPoint>
#include <QPolygon>


class BrokenLine
{
public:
    QVector<QPoint> points_;
    QVector<qreal> accLength_;
//    QVector<QPolygon> boundingBoxes_;
    EasingColor colors_;
    qreal length_ = 0;

public:
    QVector<QPoint>& points() { return points_; }
    QVector<QPoint> const & points() const { return points_; }

    QVector<qreal>& accLength() { return accLength_; }
    QVector<qreal> const & accLength() const { return accLength_; }

    EasingColor& colors() { return colors_; }
    EasingColor const & colors() const { return colors_; }

    qreal & length() { return length_ ; }
    qreal const & length() const { return length_ ; }

    BrokenLine & addPointAtEnd(const QPoint & point);
    BrokenLine & removePoint(const QPoint & point);
    BrokenLine & removeAllPoints();

    QVector<QPoint>::iterator getPoint(QPoint point);

    std::optional<QPoint> findNearest(const QPoint & other, std::optional<QPoint> nearest) const;
};

#endif // BROKENLINE_H
