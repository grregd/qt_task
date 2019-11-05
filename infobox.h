#ifndef INFOBOX_H
#define INFOBOX_H

#include <QPainter>
#include <QPoint>
#include <QString>

#include <functional>
#include <list>


class InfoBox
{
public:
    InfoBox & addInfoLine(std::function<QString()> fn);

    void paint(QPoint pos, QPainter *painter);

private:
    std::list<std::function<QString()>> fns;
};

#endif // INFOBOX_H
