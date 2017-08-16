#ifndef MYVIEW_H
#define MYVIEW_H

#include <QGraphicsView>
#include <QWidget>
#include <QGraphicsItem>

#include "myscene.h"
#include "posdialog.h"

#define PI 3.1415926
class MyView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MyView(QWidget *parent = 0);
    ~MyView();
signals:
    void changeScale(qreal t);

private:
    MyScene* m_scene;
    QGraphicsLineItem* Line;
    PosDialog* dlg;
    bool drawPt, drawCirPt, drawCross, drawPtXY;
    bool drawLine, drawLineXY,drawLineAH;
    bool drawRect, drawRectXY,drawRounded;
    bool drawElli, drawElliXY;

    enum PARAMETER{
        pt_size = 3,
        line_width = 5
    };
    QPointF start;
    QPointF end;
    QMatrix matrix;

    enum AppMode
    {
        NORMAL,
        DRAG,
        EDIT
    };
    AppMode mode;
    QPointF dragStart;
    QGraphicsItem *viewCenter;

    QList<QGraphicsItem*> chosenItems;
private:
    void catchPt(QPointF pt);
    void updateCenterRect();

public slots:
    void test();
    void setLine();
    void setPt();
    void setRect();
    void setEllipse();

    void ShowContextMenu();  //加右键菜单
    void setNormal();
    void Locate();   //重置
    void setMeasure(/*QGraphicsItem* item*/);
    void Delete();
    void SaveImage();
    void Redraw();
protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);
};

#endif // MYVIEW_H
