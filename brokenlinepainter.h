#ifndef BROKENLINEPAINTER_H
#define BROKENLINEPAINTER_H

#include <QtGlobal>

class BrokenLine;
class QPainter;
class QPolygon;
class QLineF;

class BrokenLinePainter
{
public:
    constexpr static const qreal activeLineBorderWidth = 10;
    constexpr static const qreal activeLineBorderOffset = activeLineBorderWidth/2 + 0;

public:
    BrokenLinePainter();

    static QPolygon calcBoundingBox(const QLineF & line, qreal margin, qreal penWidth);

    void paint(const BrokenLine & brokenLine, bool isActive, bool showControlPoints, qreal penWidth, QPainter * painter) const;
    void paintLineBorder(const BrokenLine &line, qreal penWidth, QPainter *painter) const;
};

#endif // BROKENLINEPAINTER_H
