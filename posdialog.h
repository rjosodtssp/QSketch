#ifndef POSDIALOG_H
#define POSDIALOG_H

#include <QDialog>
#include <QDoubleValidator>
namespace Ui {
class PosDialog;
}

class PosDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PosDialog(QWidget *parent = 0);
    ~PosDialog();
    QPointF getPt();
    QList<QPointF> getLine();
    void showPt();

private slots:
    void on_accept_clicked();

    void on_cancel_clicked();

private:
    Ui::PosDialog *ui;
    QDoubleValidator* validator;
    QPointF pt1,pt2;
};

#endif // POSDIALOG_H