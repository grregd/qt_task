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

private:
    QPen circlePen;
    QBrush circleBrush;
};

#endif // GRADIENTTOOL_H
