#ifndef GRADIENTTOOL_H
#define GRADIENTTOOL_H

#include "brokenline.h"
#include "infobox.h"
#include "brokenlinepainter.h"

#include <algorithm>

#include <QQuickPaintedItem>
#include <QLineF>
#include <QElapsedTimer>

class GradientTool : public QQuickPaintedItem
{
    using HoverPoint = std::optional< std::pair<QPoint, BrokenLine * > >;

    Q_OBJECT
    Q_PROPERTY(qreal penWidthMax MEMBER penWidthMax)
    Q_PROPERTY(qreal penWidth MEMBER penWidth READ getPenWidth WRITE setPenWidth NOTIFY penWidthChanged)
    Q_PROPERTY(bool showControlPoints MEMBER getShowControlPoints READ getShowControlPoints WRITE setShowControlPoints NOTIFY showControlPointsChanged)
    Q_PROPERTY(QColor defaultColorBegin MEMBER defaultColorBegin)
    Q_PROPERTY(QColor defaultColorEnd MEMBER defaultColorEnd)
    Q_PROPERTY(QString hoverMarkerSpec MEMBER hoverMarkerSpec)

signals:
    void penWidthChanged();
    void showControlPointsChanged();
    void requestColorChange(QColor initColor);
    void pointSelectionChanged(QColor pointColor);

public slots:
    void setColorOfSelectedPoint(const QColor & color);
    void saveToTextFile(const QUrl &fileUrl);
    void loadFromTextFile(const QUrl & fileUrl);
    void exportPng(const QUrl & fileUrl);

public:
    GradientTool();
    void setupInfoBox();

    void paint(QPainter *painter) override;

protected:
    void componentComplete() override;

    void startNewLine();
    void changeActiveLine(BrokenLine*);
//    QPolygon calcBoundingBox(const QLineF & line, qreal margin) const;
    HoverPoint findNearestPoint(const QPoint &checkPos);
    std::optional<QLineF> findHoverLine(const QPoint &checkPos);

    void painSelectedSegmenttHovered(const QLineF &fragment, QPainter *painter) const;
    void paintControlPointHovered(const QPoint &point, QPainter *painter) const;
    void paintControlPointSelected(const BrokenLine::ControlPoint &ctrlPoint, QPainter *painter) const;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void hoverMoveEvent(QHoverEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    QPointF hoverLinePointFromMouse() const;

private:
    void changeScale(int upDown, const QPoint & scalePosition);
    void setPenWidth(qreal newValue);
    void setShowControlPoints(bool newValue);

    qreal getPenWidth() const;
    bool getShowControlPoints() const;

    QPen createHoverMarkerPen(const QString & penSpec, const QPoint & point) const;

    QSize totalSpan();

private:
    BrokenLinePainter brokenLinePainter;
    QColor defaultColorBegin;
    QColor defaultColorEnd;
    QString hoverMarkerSpec = "gradient";
    QColor selectedPointColor = Qt::gray;

    QVector<BrokenLine> lines;
    BrokenLine * activeLine;
    std::optional<QLineF> hoverSegment;
    bool showControlPoints = false;

    qreal penWidthMax = 100;
    qreal penWidth = 0;
    qreal scale = 1;
    QPoint mousePos; // not transformed
    QPoint originOffset; // not transformed
    QPoint lastMouseMovePos; // not transformed

    HoverPoint hoverPoint;
    BrokenLine::ControlPointRef selectedPointRef;
    BrokenLine::ControlPointRef hoveredPointRef;

    bool mouseLeftPressed = false;
    bool mouseDragging = false;

    InfoBox infoBox;
    QElapsedTimer elapsedTimer;
    qint64 elapsedTime;
};

#endif // GRADIENTTOOL_H
