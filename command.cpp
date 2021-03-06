#include "command.h"
#include <QMessageBox>
#include <QStatusBar>
#include <QInputDialog>
#include <QtMath>
#include <QApplication>
#include <QFileDialog>
#include <QGraphicsProxyWidget>

Command::Command(MyScene *scene)
{
    m_scene = scene;
    chosenItems = m_scene->selectedItems();
}

Command::Command(MyView *view)
{
    m_view = view;
    m_scene = m_view->getScene();
    chosenItems = m_scene->selectedItems();
}

MyScene *Command::getScene()
{
    //少做转型的又一例子
//    MyScene* scene = qobject_cast<MyScene*>(m_view->scene());
    return m_view->getScene();
}

void Command::Delete()
{
    if(chosenItems.size()==0)   return;
    foreach(QGraphicsItem* item, chosenItems)
        m_scene->removeItem(item);  //删除item及其子item
    m_view->showStatus(QString("删除%1个图形成功").arg(chosenItems.size()) );
}

//这种方式只能绕原点旋转
void Command::Rotate(float angle)
{
    foreach(QGraphicsItem* item, chosenItems)
    {
        QTransform t;
        t.rotate(angle,Qt::ZAxis);
        item->setTransform(t,true);
//        绕场景坐标的原点旋转, 默认绕图形坐标的原点旋转
//        item->setTransformOriginPoint(item->mapFromScene(pt));
//        item->setRotation(angle);  // 这种方式无法连续旋转,只能转到固定位置
    }
    m_view->showStatus("已经旋转"+QString::number(angle)+"°");
}

//　快捷平移
void Command::Translate(int direction, int pace)
{
    foreach(QGraphicsItem* item, chosenItems)
    {
        if(direction == UP)
        {
            if(item->type() == QGraphicsSimpleTextItem::Type || item->type() == QGraphicsPixmapItem::Type)
                m_translate.translate(0, -pace);
            else
                m_translate.translate(0, pace);
        }
        else if(direction == DOWN)
        {
            if(item->type() == QGraphicsSimpleTextItem::Type || item->type() == QGraphicsPixmapItem::Type)
                m_translate.translate(0, pace);
            else
                m_translate.translate(0, -pace);
        }
        else if(direction == LEFT)
            m_translate.translate(-pace, 0);
        else if(direction == RIGHT)
            m_translate.translate(pace, 0);

        // false不能连续平移,只能平移到固定位置
        item->setTransform(m_translate,true);
    }
}

// 场景坐标是item->scenePos();不是item->mapToScene(item->pos());
//平移到某点
void Command::Translate(QPointF pt)
{
    foreach(QGraphicsItem* item, chosenItems)
    {
        if(item->type() == QGraphicsSimpleTextItem::Type || item->type() == QGraphicsPixmapItem::Type)
        {
            QPointF movePt = pt - item->scenePos();
            m_translate.translate(movePt.x(), movePt.y());
            item->setTransform(m_translate);
            QTransform t;
            t.rotate(180, Qt::XAxis);
            item->setTransform(t,true);
        }
        else
        {
//            QPointF p1 = item->boundingRect().center(); // 同一图形的值不变
            QPointF movePt = pt - item->boundingRect().center();
            m_translate.translate(movePt.x(), movePt.y());
            item->setTransform(m_translate);
        }
    }
    QString pos = "("+QString::number(pt.x()) + "," + QString::number(pt.y());
    m_view->showStatus("已经移动到点"+ pos);
}

// 只在视觉上改变,大小和坐标都没有改变,实际没多大意义
void Command::Zoom(bool in)
{
    qreal factor;
    foreach(QGraphicsItem* item, chosenItems)
    {
        factor = item->scale();
        if(in)
            item->setScale(1.414*factor);
        else
            item->setScale(0.707*factor);

        m_view->showStatus("所选图形的当前比例: "+QString::number(item->scale()));
    }
}

void Command::SelectAll(bool state)
{
    QList<QGraphicsItem*> all = m_scene->items(m_scene->sceneRect(),
                                               Qt::IntersectsItemShape,Qt::AscendingOrder);
    foreach (QGraphicsItem* item, all)
    {
        //去掉场景初始化的7个图形
        if(item->data(0).isNull())
            item->setSelected(state);
    }
    if(all.size()<8)
        m_view->showStatus("当前没有图形");
    else
        m_view->showStatus(QString("选择了%1个图形").arg(all.size()-7));
}

void Command::FillBrush()
{
    brush_dlg = new BrushDlg();
    if(brush_dlg->exec() != QDialog::Accepted)    return;

    Qt::BrushStyle style = brush_dlg->getStyle();
    QColor color = brush_dlg->getColor();
    if(!color.isValid())
        color = Qt::white;
    QBrush brush;
    brush.setStyle(style);
    brush.setColor(color);

    foreach(QGraphicsItem* item, chosenItems)
    {
        switch(item->type())
        {
        case QGraphicsRectItem::Type:
        {
            QGraphicsRectItem* R = qgraphicsitem_cast<QGraphicsRectItem*>(item);
            R->setBrush(brush);
            break;
        }
        case QGraphicsEllipseItem::Type:
        {
            QGraphicsEllipseItem* E = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
            E->setBrush(brush);
            break;
        }
        default:
            break;
        }
    }
    m_view->showStatus("填充图案完成");
}

void Command::SetMovable(bool state)
{
    foreach(QGraphicsItem* item, chosenItems)
    {
        item->setFlag(QGraphicsItem::ItemIsMovable, state);
//        qDebug()<<item->flags();
        if(item->flags()==( QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable ))
        {
            m_view->SetMoveFlag(true);
            m_view->showStatus("当前图形已经可拖动");
        }
        else
        {
            m_view->SetMoveFlag(false);
            m_view->showStatus("当前图形无法再拖动");
        }
    }
}

void Command::Stretch()
{
    if(chosenItems.size()!=1)   return;
    if(chosenItems.at(0)->type()!=QGraphicsLineItem::Type)   return;

    select_dlg = new SelectDlg();
    if(select_dlg->exec()!=QDialog::Accepted)   return;
    qreal len = select_dlg->getLength();
    int direction = select_dlg->getDirection();

    QGraphicsLineItem* L = qgraphicsitem_cast<QGraphicsLineItem*>(chosenItems.at(0));
    qreal angle = L->line().angle();
    QPointF p1 = L->line().p1();
    QPointF p2 = L->line().p2();

    QLineF line;
    line.setLength(len);

    if(direction== SelectDlg::Direction::start)
    {
        line.setP1(p1);
        line.setAngle(180+angle);
    }
    else if(direction== SelectDlg::Direction::end)
    {
        line.setP1(p2);
        line.setAngle(360+angle);
    }
    m_scene->addLine(line,QPen(Qt::white))->setFlag(QGraphicsItem::ItemIsSelectable);
}

void Command::changeStyle()
{
    foreach(QGraphicsItem* item, chosenItems)
    {
        switch(item->type())
        {
        case QGraphicsRectItem::Type:
        {
            QGraphicsRectItem* R = qgraphicsitem_cast<QGraphicsRectItem*>(item);
            R->setPen(m_view->getPen());
            R->setBrush(m_view->getBrush());
            break;
        }
        case QGraphicsEllipseItem::Type:
        {
            QGraphicsEllipseItem* E = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
            E->setPen(m_view->getPen());
            E->setBrush(m_view->getBrush());
            break;
        }
        case QGraphicsLineItem::Type:
        {
            QGraphicsLineItem* L = qgraphicsitem_cast<QGraphicsLineItem*>(item);
            L->setPen(m_view->getPen());
            break;
        }
        default:
            break;
        }
    }
    m_view->showStatus("图形样式已经改变");
}

void Command::SetSymmetry(Qt::Axis axis)
{
    QTransform t;
    t.rotate(180, axis);
    foreach(QGraphicsItem* item, chosenItems)
//     combine为true,否则连续对称操作会出错,仍是以原始位置为准
    item->setTransform(t,true);
}

void Command::ShowItemInfo()
{
    if(chosenItems.size()!=1)
    {
        QMessageBox::warning(0, "出错了!","只能选择一个图形 !");
        return;
    }
    QString type;
    QPointF pos;
    QSizeF size;
    QColor color;
    QString info;
//    qDebug()<<"type:"<<chosenItems.at(0)->type();
    foreach (QGraphicsItem* item, chosenItems)
    {
        switch(item->type())
        {
        case QGraphicsRectItem::Type:
        {
            QGraphicsRectItem* R = qgraphicsitem_cast<QGraphicsRectItem*>(item);
            type = "矩形";
            pos = R->boundingRect().center() + R->scenePos();
            size = R->rect().size();
            color = R->pen().color();
            info = getItemInfo(type, pos, size, color);
            break;
        }
        case QGraphicsEllipseItem::Type:
        {
            QGraphicsEllipseItem* Ell = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
            type = "椭圆";
            pos = Ell->boundingRect().center() + Ell->scenePos();
            size = Ell->rect().size();
            color = Ell->pen().color();
            info = getItemInfo(type, pos, size, color);
            break;
        }
        case CrossPt::Type :
        {
            type = "X形点";
            size = QSize(2,2);
            color = QColor(247, 160, 57);
            info = getItemInfo(type, pos, size, color);
            break;
        }
        case CirclePt::Type :
        {
            type = "圆形点";
            size = QSize(2,2);
            color = QColor(247, 160, 57);
            info = getItemInfo(type, pos, size, color);
            break;
        }
        case QGraphicsLineItem::Type :
        {
            QGraphicsLineItem* L = qgraphicsitem_cast<QGraphicsLineItem*>(item);
            qreal x1=L->line().p1().x();
            qreal y1=L->line().p1().y();
            qreal x2=L->line().p2().x();
            qreal y2=L->line().p2().y();
            qreal length = L->line().length();
            qreal angle = 360 - L->line().angle();
            if(angle > 360)   angle -= 360;

            type = "直线";
            color = L->pen().color();
            info =  QString("图形类型: %1 \n").arg(type);
            info += QString("两个点的坐标: (%2 , %3) 和 (%4 , %5)      \n").arg(x1).arg(y1).arg(x2).arg(y2);
            info += QString("直线长度: %4          直线角度: %5        \n").arg(length).arg(angle);
            info += QString("直线颜色: %6").arg(QVariant(color.toRgb()).toString());
            break;
        }
        case QGraphicsSimpleTextItem::Type :
        {
            QGraphicsSimpleTextItem* Text = qgraphicsitem_cast<QGraphicsSimpleTextItem*>(item);
            type = "文字";
            pos = Text->scenePos();
            QFont font = Text->font();
            info =  QString("图形类型: %1 \n").arg(type);
            info += QString("图形坐标: (%2 , %3)     \n").arg(pos.x()).arg(pos.y());
            info += QString("字体名称: %4            \n").arg(font.family());
            info += QString("字体大小: %5            \n").arg(font.pointSizeF());
            break;
        }
        case MyLine::Type :
        {
            MyLine* L = qgraphicsitem_cast<MyLine*>(item);
            qreal x1= L->getP1().x();
            qreal y1= L->getP1().y();
            qreal x2= L->getP3().x();
            qreal y2= L->getP3().y();

            qreal length = L->getLength();
            qreal angle =  L->getAngle();
            if(angle > 360)   angle -= 360;
            type = "直线";
            info =  QString("图形类型: %1 \n").arg(type);
            info += QString("两个点的坐标: (%2 , %3) 和 (%4 , %5)      \n").arg(x1).arg(y1).arg(x2).arg(y2);
            info += QString("直线长度: %4          直线角度: %5      \n").arg(length).arg(angle);
        }
        default:
            break;
        }
    }
    QMessageBox::information(0, "图形信息",info);
}

void Command::CatchPt()
{
    if(!m_view->goCatch())
    {
        QMessageBox::warning(0,"出错了!","没有打开捕捉模式 !");
        return;
    }
    if(chosenItems.size()!=1)   return;

    switch(chosenItems.at(0)->type())
    {
    case QGraphicsLineItem::Type :
    {
        QGraphicsLineItem* L = qgraphicsitem_cast<QGraphicsLineItem*>(chosenItems.at(0));
        QPointF p1 = L->line().p1();
        QPointF p2 = L->line().p2();
        QPointF p3;
        p3.setX( (p1.x()+p2.x())/2 );
        p3.setY( (p1.y()+p2.y())/2 );

        if(inCatchRange(m_view->getScenePos(),p1))
            m_view->catchPt(p1);
        else if(inCatchRange(m_view->getScenePos(),p2))
            m_view->catchPt(p2);
        else if(inCatchRange(m_view->getScenePos(),p3))
            m_view->catchPt(p3);
        else
        {
            m_view->changeCursor(Qt::ArrowCursor);
            return;
        }
    }
        break;
    case QGraphicsRectItem::Type :
    {
        QGraphicsRectItem* R = qgraphicsitem_cast<QGraphicsRectItem*>(chosenItems.at(0));
        QPointF bl = R->rect().topLeft();
        QPointF br = R->rect().topRight();
        QPointF tl = R->rect().bottomLeft();
        QPointF tr = R->rect().bottomRight();
        QPointF center = R->rect().center();

        //始终不能捕捉五个点,最多捕捉同一对角线的三个点 ???
        if(inCatchRange(m_view->getScenePos(),tl))
            m_view->catchPt(tl);
        else if(inCatchRange(m_view->getScenePos(),center))
            m_view->catchPt(center);
        else if(inCatchRange(m_view->getScenePos(),br))
            m_view->catchPt(br);
        else if(inCatchRange(m_view->getScenePos(),tr))
            m_view->catchPt(tr);
        else if(inCatchRange(m_view->getScenePos(),bl))
            m_view->catchPt(bl);
        else
        {
            m_view->changeCursor(Qt::ArrowCursor);
            return;
        }
    }
        break;
    case QGraphicsEllipseItem::Type :
    {
        QGraphicsEllipseItem* Elli = qgraphicsitem_cast<QGraphicsEllipseItem*>(chosenItems.at(0));
        QPointF center = Elli->rect().center();
        if(inCatchRange(m_view->getScenePos(),center))
            m_view->catchPt(center);
        else
        {
            m_view->changeCursor(Qt::ArrowCursor);
            return;
        }
    }
        break;
    case CirclePt::Type :
    {
        CirclePt* pt = qgraphicsitem_cast<CirclePt*>(chosenItems.at(0));
        QPointF center = pt->boundingRect().center()+pt->scenePos();
        if(inCatchRange(m_view->getScenePos(),center))
            m_view->catchPt(center);
        else
        {
            m_view->changeCursor(Qt::ArrowCursor);
            return;
        }
    }
        break;
    case CrossPt::Type :
    {
        CrossPt* pt = qgraphicsitem_cast<CrossPt*>(chosenItems.at(0));
        QPointF center = pt->boundingRect().center()+pt->scenePos();
        if(inCatchRange(m_view->getScenePos(),center))
            m_view->catchPt(center);
        else
        {
            m_view->changeCursor(Qt::ArrowCursor);
            return;
        }
    }
        break;
    default:
        break;
    }
}

bool Command::inCatchRange(QPointF src, QPointF des)
{
    bool rangeX = qAbs(src.x()-des.x()) < range;
    bool rangeY = qAbs(src.y()-des.y()) < range;
    return (rangeX || rangeY) ;
}

qreal Command::getSlope(QGraphicsLineItem* line)
{
    QPointF p1 = line->line().p1();
    QPointF p2 = line->line().p2();
    qreal slope = (p2.y()-p1.y()) / (p2.x()-p1.x());
    return slope;
}

qreal Command::getLinesAngle()
{
    if(chosenItems.size()!=2)
    {
        QMessageBox::warning(0,"出错了","没有选择两个图形!");
        return 360;
    }
    if(chosenItems.at(0)->type()!=QGraphicsLineItem::Type  || chosenItems.at(1)->type()!=QGraphicsLineItem::Type)
    {
        QMessageBox::warning(0,"出错了","有的图形不是直线!");
        return 360;
    }
    QGraphicsLineItem* Line1 = qgraphicsitem_cast<QGraphicsLineItem*>(chosenItems.at(0));
    qreal slope1 = getSlope(Line1);
    QGraphicsLineItem* Line2 = qgraphicsitem_cast<QGraphicsLineItem*>(chosenItems.at(1));
    qreal slope2 = getSlope(Line2);

    qreal tanV = qAbs( (slope2 - slope1)/(1+slope1*slope2) );
    qreal angle = qAtan(tanV);
    return qRadiansToDegrees(angle);
}

QString Command::getItemInfo(QString type, QPointF pos, QSizeF size, QColor c)
{
    QString info;
    info = QString("图形类型: %1 \n").arg(type);
    info += QString("图形坐标: (%2 , %3)     \n").arg(pos.x()).arg(pos.y());
    info += QString("图形大小: %4 X %5       \n").arg(size.width()).arg(size.height());
    info += QString("图形颜色: %7").arg(QVariant(c.toRgb()).toString());
    return info;
}

void Command::SmartZoom()
{
    //有item的范围Rect的大小,之前因为画坐标轴,所以范围几乎与sceneRect相同
    //    qDebug()<<m_scene->itemsBoundingRect();
    //    m_scene->items(Qt::AscendingOrder); 按绘画顺序排列
    m_view->fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    m_view->showStatus("自适应画面大小");
}

void Command::InsertPix()
{
    QString fileName=QFileDialog::getOpenFileName(0,"打开图片",QApplication::applicationDirPath(),
                                                  QStringLiteral("所有文件(*.*);;PNG文件(*.png);;JPG文件(*.jpg);;JPEG文件(*.jpeg);;BMP文件(*.bmp)") );
    if(fileName.isEmpty())      return;
    QPixmap pix(fileName);
    QGraphicsPixmapItem* pixItem = m_scene->addPixmap(pix);
    pixItem->setFlag(QGraphicsItem::ItemIsSelectable);
    pixItem->setTransform(QTransform::fromScale(1,-1));
    if(pixItem)
        m_view->showStatus("插入图片成功");
}

void Command::InsertWidget(QWidget* w)
{
    QGraphicsProxyWidget* widget = m_scene->addWidget(w);
//    widget->setTransform(QTransform::fromScale(1,-1));
    widget->setFlag(QGraphicsItem::ItemIsSelectable);
}

void Command::Animation()
{
    foreach(QGraphicsItem* item, chosenItems)
    {
        int angle =10;
        QTransform t;
        t.rotate(angle,Qt::ZAxis);
        item->setTransform(t,true);
    }
}

QList<QPointF> Command::getDividePts()
{
    QList<QPointF> list;
    if(chosenItems.size()!=1)
    {
        QMessageBox::warning(0,"出错了!","只能选择一条直线");
        list.clear();
        return list;
    }
    if(chosenItems.at(0)->type()!=QGraphicsLineItem::Type)
    {
        QMessageBox::warning(0,"出错了!","只能选择直线");
        list.clear();
        return list;
    }
    QGraphicsLineItem* L = qgraphicsitem_cast<QGraphicsLineItem*>(chosenItems.at(0));
    QPointF p1 = L->line().p1();
    QPointF p2 = L->line().p2();
    int n = QInputDialog::getInt(0, QStringLiteral("请输入直线的分割数"),"",2,2,999);

    QPointF *Pt = new QPointF[n];
    for(int i=1;i<n;i++)
    {
        Pt[i].setX(p1.x()+i*(p2.x()-p1.x())/n);
        Pt[i].setY(p1.y()+i*(p2.y()-p1.y())/n);
        list<<Pt[i];
    }
    return list;
}
