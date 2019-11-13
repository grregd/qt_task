#include "brokenlinepainter.h"

#include "brokenline.h"

#include <math.h>

#include <QPainter>
#include <QRectF>
#include <QLineF>
#include <QDebug>


BrokenLinePainter::BrokenLinePainter()
{

}

QPolygon BrokenLinePainter::calcBoundingBox(const QLineF & line, qreal margin, qreal penWidth)
{
    QRectF bb(line.p1(), QSizeF(line.length(), penWidth));

    bb += QMarginsF(margin, margin, margin, margin);

    bb.moveCenter(line.center());

    return QTransform()
            .translate(bb.center().x(), bb.center().y())
            .rotate(-line.angle())
            .translate(-bb.center().x(), -bb.center().y())
            .mapToPolygon(bb.toRect());
}

void BrokenLinePainter::paint(const BrokenLine &line, bool isActive, bool showControlPoints, qreal penWidth, QPainter *painter) const
{
    if (line.points().size() >= 2)
    {
        if (isActive)
        {
            paintLineBorder(line, penWidth, painter);
        }

        for (int i = 0; i < line.points().size()-1; ++i)
        {
            painter->setPen(QPen(QLinearGradient(line.gradientInPoint(i)),
                                 penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter->drawLine(line.fragment(i));
        }
    }

    if (showControlPoints)
    {
        painter->setPen(QPen(QColor(128, 128, 128), 3));
        QRectF r(0, 0, std::max(15.0, penWidth), std::max(15.0, penWidth));
        std::for_each(
                line.points().begin(), line.points().end(),
                [painter, &r](auto & point)
                {
                    r.moveCenter(point.point());
                    if (point.color())
                        painter->setPen(QPen(Qt::black, 6));
                    else
                        painter->setPen(QPen(QColor(128, 128, 128), 3));

                    painter->drawEllipse(r);
                });
    }
}

void BrokenLinePainter::paintLineBorder(const BrokenLine &line, qreal penWidth, QPainter *painter) const
{
    const QMarginsF arcMargin(activeLineBorderOffset, activeLineBorderOffset,
                              activeLineBorderOffset, activeLineBorderOffset);
    const QPen hoveredLinePen(QColor(0xff0090), activeLineBorderWidth,
                        Qt::SolidLine, Qt::MPenCapStyle, Qt::MPenJoinStyle);
    QVector<QPoint> boundingPolygon;
    QVector<QPoint>::iterator left = boundingPolygon.begin();
    QPolygon prevBb;

    for (int i = 0; i < line.points().size()-1; ++i)
    {
        auto bb = calcBoundingBox(line.fragment(i), activeLineBorderOffset, penWidth);
        painter->setPen(hoveredLinePen);

        if (prevBb.isEmpty())
        {
            left = boundingPolygon.insert(left, bb[0]);
        }
        else
        {
            QPointF intersectionPoint;
            auto itype = QLineF(bb[0], bb[1]).
                    intersect(QLineF(prevBb[0], prevBb[1]),
                    &intersectionPoint);
            if (itype == QLineF::BoundedIntersection)
            {
                *left = intersectionPoint.toPoint();
                left = boundingPolygon.insert(left, intersectionPoint.toPoint());
            }
            else if (itype == QLineF::UnboundedIntersection)
            {
                left = boundingPolygon.insert(left, bb[0]);
            }
            else
            {
                qDebug() << itype;
            }
        }
        left = boundingPolygon.insert(left, bb[1]);


        if (prevBb.isEmpty())
        {
            boundingPolygon.push_back(bb[3]);
        }
        else
        {
            QPointF intersectionPoint;
            auto itype = QLineF(bb[2], bb[3]).
                    intersect(QLineF(prevBb[2], prevBb[3]),
                    &intersectionPoint);
            if (itype == QLineF::BoundedIntersection)
            {
                boundingPolygon.back() = intersectionPoint.toPoint();
                boundingPolygon.push_back(intersectionPoint.toPoint());
            }
            else if (itype == QLineF::UnboundedIntersection)
            {
                boundingPolygon.push_back(bb[3]);
            }
            else
            {
                qDebug() << itype;
            }
        }
        boundingPolygon.push_back(bb[2]);

        prevBb = bb;
    }


    std::reverse(left, boundingPolygon.end());
//    painter->setPen(hoveredLinePen);
//    painter->setPen(QColor(Qt::black));
//    painter->drawPolyline(boundingPolygon);


//    painter->setPen(QPen(Qt::black, 2));

    for (auto itPolyPoint = boundingPolygon.begin(); itPolyPoint != boundingPolygon.end(); ++itPolyPoint)
    {
//        painter->drawText(*itPolyPoint, QString("%1").arg(itPolyPoint - boundingPolygon.begin()));

        if (itPolyPoint == boundingPolygon.begin() ||
            itPolyPoint == boundingPolygon.rbegin().base())
        {
            continue;
        }

        auto cntPolyPoint = boundingPolygon.size();
        auto cntHalfPolyPoint = cntPolyPoint / 2;
        const int indexPolyPoint = std::distance(boundingPolygon.begin(), itPolyPoint);
        const int indexCtrlPoint = ((indexPolyPoint < cntHalfPolyPoint)
                              ? (indexPolyPoint+1)/2
                              : (cntPolyPoint-indexPolyPoint)/2);

        if (indexPolyPoint % 2 == 1)
        {
            if (*itPolyPoint != *(itPolyPoint+1) &&
                indexCtrlPoint != 0 &&
                indexCtrlPoint < line.points().size()-1)
            {
                auto const & currentCtrlPoint = line.points()[indexCtrlPoint].point();
                auto const & prevCtrlPoint = line.points()[indexCtrlPoint-1].point();
                auto const & nextCtrlPoint = line.points()[indexCtrlPoint+1].point();
                const QLineF prevCtrlLine(currentCtrlPoint, prevCtrlPoint);
                const QLineF nextCtrlLine(currentCtrlPoint, nextCtrlPoint);

                QRectF arcRect(0, 0, penWidth, penWidth);
                arcRect.moveCenter(currentCtrlPoint);
                arcRect += arcMargin;

                if (nextCtrlLine.angleTo(prevCtrlLine) > 180)
                {
                    painter->drawArc(arcRect,
                                     std::lround((prevCtrlLine.angle()+270)*16),
                                     std::lround((180-nextCtrlLine.angleTo(prevCtrlLine))*16));
                }
                else
                {
                    painter->drawArc(arcRect,
                                     std::lround((nextCtrlLine.angle()+270)*16),
                                     std::lround((180-prevCtrlLine.angleTo(nextCtrlLine))*16));
                }
            }

            painter->drawLine(*(itPolyPoint-1), *itPolyPoint);
        }
    }
}
