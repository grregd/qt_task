#ifndef GRADIENTTOOL_H
#define GRADIENTTOOL_H

#include <QQuickPaintedItem>
#include <QPen>
//#include <QBrush>

class GradientTool : public QQuickPaintedItem
{
    Q_OBJECT;
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
    QVector<QPoint> lines;
    qreal linesLength = 0;
    QPen circlePen;
    QBrush circleBrush;
    int max = 10000;
    qreal penWidth = 4.0;
};

#endif // GRADIENTTOOL_H
