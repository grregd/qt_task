#ifndef GRADIENTTOOL_H
#define GRADIENTTOOL_H

//#include "easingcolor.h"
#include "brokenline.h"

#include <algorithm>

#include <QQuickPaintedItem>

class GradientTool : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal penWidthMax MEMBER penWidthMax /*READ getPenWidthMax WRITE setPenWidthMax*/)
    Q_PROPERTY(qreal penWidth MEMBER penWidth READ getPenWidth WRITE setPenWidth NOTIFY penWidthChanged)
    Q_PROPERTY(bool showControlPoints MEMBER getShowControlPoints READ getShowControlPoints WRITE setShowControlPoints NOTIFY showControlPointsChanged)
    Q_PROPERTY(QColor colorBegin READ getColorBegin WRITE setColorBegin NOTIFY colorBeginChanged)
    Q_PROPERTY(QColor colorEnd READ getColorEnd WRITE setColorEnd NOTIFY colorEndChanged)

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
    void paintBrokenLine(const BrokenLine &line, QPainter *painter) const;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void hoverMoveEvent(QHoverEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void addPointAtEnd(const QPoint & point);
    void finishCurrentLine();

private:
//    void setPenWidthMax(qreal newValue);
    void setPenWidth(qreal newValue);
    void setColorBegin(const QColor & newColor);
    void setColorEnd(const QColor & newColor);
    void setShowControlPoints(bool newValue);

//    qreal getPenWidthMax() const;
    qreal getPenWidth() const;
    bool getShowControlPoints() const;
    QColor getColorBegin() const;
    QColor getColorEnd() const;

private:
    QVector<BrokenLine> lines;
    BrokenLine* line_;
    bool showControlPoints = false;
    QVector<QPoint> undoPoints;
    qreal penWidthMax = 100;
    qreal penWidth = 0;

    using HoverPoint = std::optional< std::pair<QPoint, BrokenLine * > >;
    HoverPoint hoverPoint;
    bool dragging = false;
    QVector<QPoint>::iterator hoverPointIterator;
};

#endif // GRADIENTTOOL_H
