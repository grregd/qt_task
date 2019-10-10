#ifndef GRADIENTTOOL_H
#define GRADIENTTOOL_H

#include "easingcolor.h"

#include <QQuickPaintedItem>
#include <QPen>
//#include <QBrush>

class GradientTool : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal penWidth MEMBER penWidth READ getPenWidth WRITE setPenWidth NOTIFY penWidthChanged)
    Q_PROPERTY(bool showControlPoints MEMBER getShowControlPoints READ getShowControlPoints WRITE setShowControlPoints NOTIFY showControlPointsChanged)
    Q_PROPERTY(QColor colorBegin WRITE setColorBegin)
    Q_PROPERTY(QColor colorEnd WRITE setColorEnd)

signals:
    void penWidthChanged();
    void showControlPointsChanged();
    void colorBeginChanged();
    void colorEndChanged();

public:
    GradientTool();

    void paint(QPainter *painter) override;

//public slots:
//    void addPoint(int x, int y);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void hoverMoveEvent(QHoverEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

    void paint7(QPainter *painter);

private:
    void setPenWidth(qreal newValue);
    void setColorBegin(const QColor & newColor);
    void setColorEnd(const QColor & newColor);

    qreal getPenWidth() const;
    bool getShowControlPoints() const;
    void setShowControlPoints(bool newValue);

    std::optional<QPoint> findNearest(const QPoint & other) const;

private:
    bool showControlPoints = false;
    QVector<QPoint> lines;
    qreal linesLength = 0;
    QPen circlePen;
    QBrush circleBrush;
    int max = 10000;
    qreal penWidth = 0;
    EasingColor easingColor;
    std::optional<QPoint> hoverPoint;
};

#endif // GRADIENTTOOL_H
