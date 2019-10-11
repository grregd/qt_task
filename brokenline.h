#ifndef BROKENLINE_H
#define BROKENLINE_H

#include "easingcolor.h"

#include <QVector>
#include <QPoint>


class BrokenLine
{
public:
    QVector<QPoint> points_;
    EasingColor colors_;
    qreal length_ = 0;

public:
    QVector<QPoint>& points() { return points_; }
    QVector<QPoint> const & points() const { return points_; }

    EasingColor& colors() { return colors_; }
    EasingColor const & colors() const { return colors_; }

    qreal & length() { return length_ ; }
    qreal const & length() const { return length_ ; }

    BrokenLine & removePoint(const QPoint & point);
};

#endif // BROKENLINE_H
