#include "infobox.h"

#include <QStaticText>

#include <cmath>


InfoBox &InfoBox::addInfoLine(std::function<QString ()> fn)
{
    fns.push_back(fn);

    return *this;
}

void InfoBox::paint(QPoint pos, QPainter *painter)
{
    if (!visible_)
        return;

    const int infoBoxWidth = 200;
    const int infoLineHeight = 15;

    painter->save();
    painter->resetTransform();
    painter->setPen(QPen());

    for (auto fn: fns)
    {
        painter->drawStaticText(pos, QStaticText(fn()));
        pos.ry() += infoLineHeight;
    }

    painter->restore();
}
