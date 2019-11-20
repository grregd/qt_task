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

    void setVisible(bool v) { visible_ = v; }
    bool visible() const { return visible_; }

private:
    std::list<std::function<QString()>> fns;

private:
    bool visible_ = true;
};

#endif // INFOBOX_H
