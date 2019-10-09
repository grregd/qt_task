#ifndef EASINGCOLOR_H
#define EASINGCOLOR_H

#include <QColor>

class EasingColor
{
public:
    EasingColor();

    const QColor & getColorBegin() const;
    const QColor & getColorEnd() const;

    void setColorBegin(const QColor & newColor);
    void setColorEnd(const QColor & newColor);

private:
    QColor colorBegin;
    QColor colorEnd;
};

#endif // EASINGCOLOR_H
