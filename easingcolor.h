#ifndef EASINGCOLOR_H
#define EASINGCOLOR_H

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

#endif // EASINGCOLOR_H
