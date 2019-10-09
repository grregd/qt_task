#ifndef EASINGCOLOR_H
#define EASINGCOLOR_H

#include <QColor>
#include <QEasingCurve>

class EasingColor
{
public:
    EasingColor();

    const QColor & getColorBegin() const;
    const QColor & getColorEnd() const;

    void setColorBegin(const QColor & newColor);
    void setColorEnd(const QColor & newColor);

    void setEasingCurveType(QEasingCurve::Type curveType);

    QColor colorForProgress(qreal progress);

private:
    QColor colorBegin;
    QColor colorEnd;
    QEasingCurve easing;
};

#endif // EASINGCOLOR_H
