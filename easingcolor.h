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
    MultiGradient(const MultiGradient&) = default;
    MultiGradient(MultiGradient&&) = default;

    QColor colorForProgress(qreal progress) const;
};

#endif // EASINGCOLOR_H
