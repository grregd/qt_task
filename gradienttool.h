#ifndef GRADIENTTOOL_H
#define GRADIENTTOOL_H

#include <QQuickPaintedItem>
#include <QPen>
//#include <QBrush>

class GradientTool : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal penWidth MEMBER penWidth NOTIFY penWidthChanged)
    Q_PROPERTY(QColor colorBegin MEMBER colorBegin NOTIFY colorBeginChanged)
    Q_PROPERTY(QColor colorEnd MEMBER colorEnd NOTIFY colorEndChanged)

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
    QVector<QPoint> lines;
    qreal linesLength = 0;
    QPen circlePen;
    QBrush circleBrush;
    int max = 10000;
    qreal penWidth = 0;
    QColor colorBegin;
    QColor colorEnd;
};

#endif // GRADIENTTOOL_H
