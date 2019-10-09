#ifndef GRADIENTTOOL_H
#define GRADIENTTOOL_H

#include "easingcolor.h"

#include <QQuickPaintedItem>
#include <QPen>
//#include <QBrush>

class GradientTool : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal penWidth MEMBER penWidth NOTIFY penWidthChanged)
    Q_PROPERTY(QColor colorBegin WRITE setColorBegin)
    Q_PROPERTY(QColor colorEnd WRITE setColorEnd)

signals:
    void penWidthChanged();
    void colorBeginChanged();
    void colorEndChanged();

public:
    GradientTool();

    void paint(QPainter *painter) override;

//public slots:
//    void addPoint(int x, int y);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent* event) override;

    void paint7(QPainter *painter);

private:
    void setColorBegin(const QColor & newColor);
    void setColorEnd(const QColor & newColor);

private:
    QVector<QPoint> lines;
    qreal linesLength = 0;
    QPen circlePen;
    QBrush circleBrush;
    int max = 10000;
    qreal penWidth = 0;
    EasingColor easingColor;
};

#endif // GRADIENTTOOL_H
