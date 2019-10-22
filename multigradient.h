#ifndef MULTIGRADIENT_H
#define MULTIGRADIENT_H

#include <QColor>
#include <QEasingCurve>
#include <QGradientStops>


class MultiGradient
{
    QGradientStops gradients;

public:
    MultiGradient() = default;
    MultiGradient(const MultiGradient&) = default;
    MultiGradient(MultiGradient&&) = default;
    MultiGradient(const QGradientStops & stops);

    MultiGradient & operator=(const MultiGradient&) = default;
    MultiGradient & operator=(MultiGradient&&) = default;

    QGradientStops& stops() { return gradients; }
    void push_back(const QGradientStop & stop);
    QColor colorForProgress(qreal progress) const;
};

#endif // MULTIGRADIENT_H
