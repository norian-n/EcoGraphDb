#include "GraphSceneForm.h"
#include "ui_GraphSceneForm.h"

#include <QtDebug>
#include <QGraphicsTextItem>

GraphSceneForm::GraphSceneForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphSceneForm)
{
    ui->setupUi(this);

    ui->graphicsView->setScene(&scene);

    scene.myForm = this;

    // LoadGraph();
}


void GraphSceneForm::LoadGraph()
{
    QPixmap pixMap(40,40);

    pixMap.fill(Qt::green);

    QBrush circleBrush = QBrush(QColor(0xa6, 0xce, 0x39)); // gradient
    QPen circlePen = QPen(Qt::black);
    circlePen.setWidth(1);

    QPen linkPen = QPen(Qt::darkGreen);
    linkPen.setWidth(2);

    scene.clear();
    firstNodeStored = false;

        // get coords from egDb

    graphDB.Connect();

    nodes.Connect(graphDB, "locations");

    nodes.AutoLoadAllData();

    nodes.LoadLinkType("linktype");
    nodes.myLinkTypes["linktype"]-> ResolveLinks(nodes, nodes);

    QList<QVariant> locValues;

        // nodes size and center
    int w = 40;
    int h = 40;

    int x = 0;
    int y = 0;

    int type;

    QGraphicsItem* newItem;

        // add nodes to the scene - QMap<EgDataNodeIDtype, EgDataNode>::iterator
    for (auto dataNodeIter = nodes.dataNodes.begin(); dataNodeIter != nodes.dataNodes.end(); ++dataNodeIter)
    {
        nodes.GetLocation(locValues, dataNodeIter.key());

        if (locValues.count() > 2)
        {

            x = locValues[0].toInt();
            y = locValues[1].toInt();

            type = locValues[2].toInt();

            if (type == 1)
                newItem = scene.addEllipse(QRect(x-20, y-20, w, h), circlePen, circleBrush);
            else // if (type == 2)
            {
                //newItem = scene.addRect(QRect(x-20, y-20, w, h), circlePen, circleBrush);
                newItem = scene.addPixmap(pixMap);

                if (newItem)
                    newItem->setPos(x-20, y-20);
            }

                // save first item to push lines below
            if (! firstNodeStored)
            {
                firstNode = newItem;
                firstNodeStored = true;
            }

            // QString msg = dataNodeIter.value()["name"].toString();

            if (newItem)
                newItem->setData(0, dataNodeIter.key()); // nodeID

            QGraphicsTextItem* label = new QGraphicsTextItem(dataNodeIter.value()["name"].toString());
            label->setPos(x-30, y-40);

            scene.addItem(label);

            // qDebug() << msg << FN;
        }
    }

        // destination node center
    int xDst = 0;
    int yDst = 0;

        // add links - QMap <EgDataNodeIDtype, EgDataNode>::iterator
    for (auto dataNodeIter = nodes.dataNodes.begin(); dataNodeIter != nodes.dataNodes.end(); ++dataNodeIter)
    {
        nodes.GetLocation(locValues, dataNodeIter.key());

        if (locValues.count() > 1)
        {
            x = locValues[0].toInt();
            y = locValues[1].toInt();
        }

            // get linked nodes
        if (dataNodeIter.value().nodeLinks && (! dataNodeIter.value().nodeLinks-> outLinks.empty()))
        {
                // QList<EgExtendedLinkType>::iterator
            for (auto linksIter  = dataNodeIter.value().nodeLinks-> outLinks["linktype"].begin();
                      linksIter != dataNodeIter.value().nodeLinks-> outLinks["linktype"].end(); ++linksIter)
            {
                // qDebug()  << "Dst node ID = " << (*linksIter).dataNodePtr-> dataNodeID << FN;

                nodes.GetLocation(locValues, (*linksIter).dataNodePtr-> dataNodeID);

                if (locValues.count() > 1)
                {
                    xDst = locValues[0].toInt();
                    yDst = locValues[1].toInt();
                }

                QGraphicsItem * theLine = scene.addLine(x, y, xDst, yDst, linkPen);

                if (firstNode)
                    theLine->stackBefore(firstNode);

            }
        }
    }
}

void MyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // QPoint clickPoint = mouseEvent->scenePos().toPoint();
    QTransform deviceTransform;

    isPressed = true;

    // saveX = clickPoint.x();
    // saveY = clickPoint.y();

    theItem = itemAt(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), deviceTransform);

    // if (theItem)
        // theItem->moveBy(-100,-100);
        // qDebug() << "item at:" << theItem->boundingRect().center().toPoint().x() << " " <<  theItem-> boundingRect().center().toPoint().y();


    // qDebug() << "press at:" <<  clickPoint.x() << " " << clickPoint.y();
}

void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // QPoint clickPoint = mouseEvent->scenePos().toPoint();
    QPen linkPen = QPen(Qt::darkRed);
    linkPen.setWidth(2);

    QGraphicsItem * theDestItem;
    QTransform deviceTransform;

    theDestItem = itemAt(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), deviceTransform);

    if (isMoved && theItem && theDestItem)
    {
        QGraphicsItem * theLine = addLine(theItem->boundingRect().center().toPoint().x(), theItem->boundingRect().center().toPoint().y(),
                theDestItem->boundingRect().center().toPoint().x(), theDestItem->boundingRect().center().toPoint().y(), linkPen);

        if (myForm-> firstNode)
            theLine->stackBefore(myForm-> firstNode);
        else
        {
            theLine->stackBefore(theItem);
            theLine->stackBefore(theDestItem);
        }

        // qDebug() << "from node: " <<  theItem-> data(0).toInt() << " to node: " <<  theDestItem-> data(0).toInt();

        myForm-> nodes.AddArrowLink("linktype", theItem-> data(0).toInt(), myForm-> nodes, theDestItem-> data(0).toInt());
    }

    isPressed = false;
    isMoved = false;

    // saveX = 0;
    // saveY = 0;

    // qDebug() << "release at:" <<  clickPoint.x() << " " << clickPoint.y();
}

void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // QPoint clickPoint = mouseEvent->scenePos().toPoint();

    if (isPressed)
    {
        isMoved = true;
    }
}

void MyGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // qDebug() << mouseEvent->scenePos();

    if (! myForm-> nodes.isConnected)
        return;

    QPoint clickPoint = mouseEvent->scenePos().toPoint();

    // qDebug() <<  clickPoint.x() << " " << clickPoint.y();

    QBrush circleBrush = QBrush(Qt::magenta); // gradient
    QPen circlePen = QPen(Qt::black);
    circlePen.setWidth(1);

    int w = 40;
    int h = 40;

    QGraphicsItem* newItem = addEllipse(QRect(clickPoint.x()-20, clickPoint.y()-20, w, h), circlePen, circleBrush);

    EgDataNodeIDtype newID;
    QList<QVariant> addValues;
    QList<QVariant> locValues;

    QString nodeName = QVariant(clickPoint.x()).toString() + "," + QVariant(clickPoint.y()).toString();

    addValues.clear();
    addValues << nodeName << 2;

    myForm-> nodes.AddDataNode(addValues, newID);

    if (newItem)
        newItem->setData(0, newID); // nodeID

    locValues.clear();
    locValues << clickPoint.x() << clickPoint.y() << 1;

    myForm-> nodes.AddLocation(locValues, newID);

    QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void MyGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent)
{
    // qDebug() <<  wheelEvent->delta();

    if (wheelEvent->delta() > 0)
    {
        myForm->ui->graphicsView->scale(2.0, 2.0);
    }
    else
    {
        myForm->ui->graphicsView->scale(0.5, 0.5);
    }

    QGraphicsScene::wheelEvent(wheelEvent);
}

void GraphSceneForm::SaveGraph()
{
    nodes.StoreData();

    nodes.StoreAllLinks();
}

GraphSceneForm::~GraphSceneForm()
{
    delete ui;
}

void GraphSceneForm::on_loadButton_clicked()
{
    LoadGraph();
}

void GraphSceneForm::on_saveButton_clicked()
{
    SaveGraph();
}
