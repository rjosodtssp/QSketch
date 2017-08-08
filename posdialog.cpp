#include "posdialog.h"
#include "ui_posdialog.h"
#include <QDebug>

PosDialog::PosDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PosDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("请输入点的坐标");
    this->setWindowIcon(QIcon(":/Icon/Icon/input.png"));

    validator = new QDoubleValidator(-300,300,2,this);
    ui->pt1_x->setValidator(validator);
    ui->pt1_y->setValidator(validator);
    ui->pt2_x->setValidator(validator);
    ui->pt2_y->setValidator(validator);
}

PosDialog::~PosDialog()
{
    delete ui;
}

QPointF PosDialog::getPt()
{
    return pt1;
}

QList<QPointF> PosDialog::getLine()
{
    QList<QPointF> list;
    list << pt1 << pt2;
    return list;
}

void PosDialog::showPt()
{
    ui->pt2_x->setVisible(false);
    ui->pt2_y->setVisible(false);
    ui->label_3->setVisible(false);
    ui->label_4->setVisible(false);
}

void PosDialog::on_accept_clicked()
{
    pt1.setX(ui->pt1_x->text().toFloat());
    pt1.setY(ui->pt1_y->text().toFloat());
    pt2.setX(ui->pt2_x->text().toFloat());
    pt2.setY(ui->pt2_y->text().toFloat());
    accept();
}

void PosDialog::on_cancel_clicked()
{
    this->close();
}
