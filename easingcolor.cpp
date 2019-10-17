#include "easingcolor.h"

#include <QDebug>
#include <QGradientStops>

#include <algorithm>


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

QColor colorForProgress(qreal progress, const QColor & colorBegin, const QColor & colorEnd)
{
    auto componentR = colorBegin.red() + progress*(colorEnd.red() - colorBegin.red());
    auto componentG = colorBegin.green() + progress*(colorEnd.green() - colorBegin.green());
    auto componentB = colorBegin.blue() + progress*(colorEnd.blue() - colorBegin.blue());
    auto componentA = colorBegin.alpha() + progress*(colorEnd.alpha() - colorBegin.alpha());

    return QColor(componentR, componentG, componentB, componentA);

}

QColor EasingColor::colorForProgress(qreal progress) const
{
    return ::colorForProgress(easing.valueForProgress(progress), colorBegin, colorEnd);
}



MultiGradient::MultiGradient()
{
    gradients.push_back(QGradientStop(0.0, QColor(Qt::black)));
    gradients.push_back(QGradientStop(0.33, QColor(Qt::red)));
    gradients.push_back(QGradientStop(1.0, QColor(Qt::white)));
}

MultiGradient::MultiGradient(const QGradientStops &stops)
    : gradients(stops)
{
}

//void MultiGradient::push_back(const QGradientStop &stop)
//{
//    gradients.push_back(stop);
//}

QColor MultiGradient::colorForProgress(qreal progress) const
{
    if (progress == 0.0)
    {
        return ::colorForProgress(progress, gradients.begin()->second, (gradients.begin()+1)->second);
    }

    auto stopPointIt =
            std::lower_bound(gradients.begin(),
                gradients.end(), progress,
                []( const auto & pair, const auto & value )
                {
                    return pair.first < value;
                });

    if (stopPointIt == gradients.end())
    {
        return gradients.back().second;
    }

    auto startPointIt =
            stopPointIt == gradients.begin()
            ? gradients.begin()
            : stopPointIt - 1;

    auto width = stopPointIt->first - startPointIt->first;
    auto ratio = (progress - startPointIt->first) / width;


    return ::colorForProgress(ratio, startPointIt->second, stopPointIt->second);
}
