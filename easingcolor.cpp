#include "easingcolor.h"


EasingColor::EasingColor()
{
};

const QColor & EasingColor::getColorBegin() const
{
    return colorBegin;
}

const QColor & EasingColor::getColorEnd() const
{
    return colorEnd;
}

EasingColor & EasingColor::setColorBegin(const QColor & newColor)
{
    colorBegin = newColor;
    return *this;
}

EasingColor & EasingColor::setColorEnd(const QColor & newColor)
{
    colorEnd = newColor;
    return *this;
}

void EasingColor::setEasingCurveType(QEasingCurve::Type curveType)
{
    easing.setType(curveType);
}

QColor EasingColor::colorForProgress(qreal progress) const
{
    auto easingValue = easing.valueForProgress(progress);

    auto componentR = colorBegin.red() + easingValue*(colorEnd.red() - colorBegin.red());
    auto componentG = colorBegin.green() + easingValue*(colorEnd.green() - colorBegin.green());
    auto componentB = colorBegin.blue() + easingValue*(colorEnd.blue() - colorBegin.blue());
    auto componentA = colorBegin.alpha() + easingValue*(colorEnd.alpha() - colorBegin.alpha());

    return QColor(componentR, componentG, componentB, componentA);
}
