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
    Q_PROPERTY(qreal penWidthMax MEMBER penWidthMax)
    Q_PROPERTY(qreal penWidth MEMBER penWidth READ getPenWidth WRITE setPenWidth NOTIFY penWidthChanged)
    Q_PROPERTY(bool showControlPoints MEMBER getShowControlPoints READ getShowControlPoints WRITE setShowControlPoints NOTIFY showControlPointsChanged)
    Q_PROPERTY(QColor colorBegin READ getColorBegin WRITE setColorBegin NOTIFY colorBeginChanged)
    Q_PROPERTY(QColor colorEnd READ getColorEnd WRITE setColorEnd NOTIFY colorEndChanged)
    Q_PROPERTY(QColor defaultColorBegin MEMBER defaultColorBegin)
    Q_PROPERTY(QColor defaultColorEnd MEMBER defaultColorEnd)
    Q_PROPERTY(QColor hoverSelectColor MEMBER hoverSelectColor)

signals:
    void penWidthChanged();
    void showControlPointsChanged();
    void colorBeginChanged();
    void colorEndChanged();
    void requestColorChange(QColor initColor);

public:
    GradientTool();

    void paint(QPainter *painter) override;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
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

    void paintInfo(QPainter *painter);
    
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
    QColor hoverSelectColor = 0xff1493;

    QVector<BrokenLine> lines;
    BrokenLine * line_;
    std::optional<QLineF> hoverSegment;
    bool showControlPoints = false;

    qreal penWidthMax = 100;
    qreal penWidth = 0;
    qreal scale = 1;
    QPoint mousePos; // not transformed
    QPoint originOffset; // not transformed
    QPoint lastMouseMovePos; // not transformed

    HoverPoint hoverPoint;
    QVector<BrokenLine::ControlPoint>::iterator hoverPointIterator;

    bool mouseLeftPressed = false;
    bool mouseDragging = false;
};

#endif // GRADIENTTOOL_H
