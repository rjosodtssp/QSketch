## <center>QSketch是一个简易的绘图工具。</center>

#### 使用说明

用快捷键PgUp和PgDown改变平移的幅度

#### To-do list:
1. 自定义箭头图元和渐变的圆形图元


#### Bugs:
1. 打印的结果仍然是场景未做对称的情况
2. 跨文件复制图元时，所有图元仍然按鼠标位置复制，结果是都聚集到一起
3. 图元进行平移后，保存再打开，位置保存不成功;复制某图元，保存再打开，复制的图元位置不对
4. 最近打开文件记录和文件修改时间 无法在关闭程序后保存
9. 原生图元类不设定为ItemIsMovable时，选定后，再右键菜单会取消选择;拖动或用QTransform后，场景坐标实际没有改变。但是自定义类没有这两个问题。旋转图元后，输出坐标值未变


5. setAngle()的参数为什么是360-angle?
6. 只有自定义的CrossPt类调用setPos得到的坐标是场景坐标，Qt类得到的都是图元坐标
7. QObject::connect: Incompatible sender/receiver arguments QAction::triggered(bool) --> MyView::Paste(QPointF)
11. 保存函数MyScene::Export代码重复严重，MyView::Copy()直接调用了Export,但MyView::Paste()没有直接调用MyScene::Import，而是修改Import，重复也很严重