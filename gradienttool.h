#ifndef GRADIENTTOOL_H
#define GRADIENTTOOL_H

#include "brokenline.h"

#include <algorithm>

#include <QQuickPaintedItem>
#include <QLineF>

class GradientTool : public QQuickPaintedItem
{
    using HoverPoint = std::optional< std::pair<QPoint, BrokenLine * > >;

    Q_OBJECT
    Q_PROPERTY(qreal penWidthMax MEMBER penWidthMax /*READ getPenWidthMax WRITE setPenWidthMax*/)
    Q_PROPERTY(qreal penWidth MEMBER penWidth READ getPenWidth WRITE setPenWidth NOTIFY penWidthChanged)
    Q_PROPERTY(bool showControlPoints MEMBER getShowControlPoints READ getShowControlPoints WRITE setShowControlPoints NOTIFY showControlPointsChanged)
    Q_PROPERTY(QColor colorBegin READ getColorBegin WRITE setColorBegin NOTIFY colorBeginChanged)
    Q_PROPERTY(QColor colorEnd READ getColorEnd WRITE setColorEnd NOTIFY colorEndChanged)
    Q_PROPERTY(QColor defaultColorBegin MEMBER defaultColorBegin)
    Q_PROPERTY(QColor defaultColorEnd MEMBER defaultColorEnd)

signals:
    void penWidthChanged();
    void showControlPointsChanged();
    void colorBeginChanged();
    void colorEndChanged();

public:
    GradientTool();

    void paint(QPainter *painter) override;

public slots:
    void removeLastPoint();
    void redoLastPoint();

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void hoverMoveEvent(QHoverEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void componentComplete() override;

    void addPointAtEnd(const QPoint & point);
    void finishCurrentLine();
    void changeActiveLine(BrokenLine*);
//    QPolygon calcBoundingBox(const QLineF & line, qreal margin) const;
    HoverPoint findNearestPoint(const QPoint &checkPos);
    std::optional<QLineF> findHoverLine(const QPoint &checkPos);

    void paintBoundingBox(const QLineF &fragment, QPainter *painter) const;
    void paintBrokenLine(const BrokenLine &line, QPainter *painter) const;

    QPointF hoverLinePointFromMouse();

private:
    void setPenWidth(qreal newValue);
    void setColorBegin(const QColor & newColor);
    void setColorEnd(const QColor & newColor);
    void setShowControlPoints(bool newValue);

    qreal getPenWidth() const;
    bool getShowControlPoints() const;
    QColor getColorBegin() const;
    QColor getColorEnd() const;

private:
    QColor defaultColorBegin;
    QColor defaultColorEnd;

    QVector<BrokenLine> lines;
    BrokenLine * line_;
    std::optional<QLineF> hoverSegment;
    QPoint mousePos;
    bool showControlPoints = false;
    QVector<QPoint> undoPoints;

    qreal penWidthMax = 100;
    qreal penWidth = 0;

    HoverPoint hoverPoint;
    bool mouseLeftPressed = false;
    bool mouseDragging = false;
    QVector<QPoint>::iterator hoverPointIterator;
};

#endif // GRADIENTTOOL_H
