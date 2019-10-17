#ifndef EASINGCOLOR_H
#define EASINGCOLOR_H

#include <QColor>
#include <QEasingCurve>
#include <QGradientStops>

class EasingColor
{
public:
    EasingColor();

    const QColor & getColorBegin() const;
    const QColor & getColorEnd() const;

    EasingColor &setColorBegin(const QColor & newColor);
    EasingColor &setColorEnd(const QColor & newColor);

    void setEasingCurveType(QEasingCurve::Type curveType);

    QColor colorForProgress(qreal progress) const;

private:
    QColor colorBegin;
    QColor colorEnd;
    QEasingCurve easing;
};

class MultiGradient
{
    QGradientStops gradients;

public:
    MultiGradient();
    MultiGradient(const QGradientStops & stops);
    MultiGradient(const MultiGradient&) = default;
    MultiGradient(MultiGradient&&) = default;
    MultiGradient & operator=(const MultiGradient&) = default;
    MultiGradient & operator=(MultiGradient&&) = default;

    QGradientStops& stops() { return gradients; }

//    void push_back(const QGradientStop & stop);

    QColor colorForProgress(qreal progress) const;
};

#endif // EASINGCOLOR_H
