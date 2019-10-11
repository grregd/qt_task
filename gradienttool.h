#ifndef GRADIENTTOOL_H
#define GRADIENTTOOL_H

#include "easingcolor.h"

#include <QQuickPaintedItem>
#include <QPen>
//#include <QBrush>

class GradientTool : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal penWidthMax MEMBER penWidthMax /*READ getPenWidthMax WRITE setPenWidthMax*/)
    Q_PROPERTY(qreal penWidth MEMBER penWidth READ getPenWidth WRITE setPenWidth NOTIFY penWidthChanged)
    Q_PROPERTY(bool showControlPoints MEMBER getShowControlPoints READ getShowControlPoints WRITE setShowControlPoints NOTIFY showControlPointsChanged)
    Q_PROPERTY(QColor colorBegin READ getColorBegin WRITE setColorBegin)
    Q_PROPERTY(QColor colorEnd READ getColorEnd WRITE setColorEnd)

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
    void paint7(QPainter *painter);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void hoverMoveEvent(QHoverEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void addPointAtEnd(const QPoint & point);

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

    std::optional<QPoint> findNearest(const QPoint & other) const;

    class Line
    {
        QVector<QPoint> points;
        EasingColor colors;
    };

private:
    bool showControlPoints = false;
    QVector<QPoint> lines_points;
    QVector<QPoint> undoPoints;
    qreal linesLength = 0;
    qreal penWidthMax = 100;
    qreal penWidth = 0;
    EasingColor easingColor;
    std::optional<QPoint> hoverPoint;
    bool dragging = false;
};

#endif // GRADIENTTOOL_H
