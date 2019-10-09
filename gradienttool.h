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

    void paint0(QPainter *painter);
    void paint1(QPainter *painter);
    void paint2(QPainter *painter);
    void paint3(QPainter *painter);
    void paint6(QPainter *painter);
    void paint7(QPainter *painter);

    void myDrawLine(QPainter *painter, const QPoint &p1, const QPoint &p2);

private:
    QPen circlePen;
    QBrush circleBrush;
    int max = 10000;
};

#endif // GRADIENTTOOL_H
