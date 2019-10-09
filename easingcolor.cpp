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

void EasingColor::setColorBegin(const QColor & newColor)
{
    colorBegin = newColor;
}

void EasingColor::setColorEnd(const QColor & newColor)
{
    colorEnd = newColor;
}
