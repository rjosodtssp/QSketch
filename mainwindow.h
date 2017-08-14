#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "myview.h"
#include "myscene.h"
#include "posdialog.h"
#include <QMainWindow>
#include <QGraphicsSceneMoveEvent>
#include <QGraphicsLineItem>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:

signals:
    void getPt(QPointF pt1);
private:
    Ui::MainWindow *ui;
    MyScene* m_scene;
    MyView* m_view;
    PosDialog* dlg;
    QMenu *ptMenu, *LineMenu;
    QList<QAction*> ptActions;
    QList<QAction*> lineActions;
    QGraphicsLineItem* Line;
private:
    void InitUi();
protected:

};

#endif // MAINWINDOW_H
