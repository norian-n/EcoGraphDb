#include "GraphSceneForm.h"
#include "ui_GraphSceneForm.h"

#include <QtDebug>
#include <QGraphicsTextItem>

FingersTreeForm::FingersTreeForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GraphSceneForm)
{
    ui->setupUi(this);

    ui->graphicsView-> setScene(&scene);
    ui->graphicsView-> setAcceptDrops(true);

    ui->iconsPanel-> setScene(&iconsScene);

/*    for (int counterC = 0; counterC < ui->tableWidget->columnCount(); counterC++)
        ui->tableWidget->setColumnWidth(counterC, 50);

    for (int counterR = 0; counterR < ui->tableWidget->rowCount(); counterR++)
        ui->tableWidget->setRowHeight(counterR, 50);

    ui->tableWidget-> setIconSize(QSize(pixmapSizeFixed,pixmapSizeFixed));
    */

    scene.setSceneRect(-300, -150, 600, 300);

    scene.myForm = this;
    iconsScene.myForm = this;

    // graphDB.Connect();


    // LoadGraph();
}

void FingersTreeForm::LoadImages()
{
    iconsScene.clear();

    iconsScene.addRect(QRect(0, 0, 0, 0)); //, circleBrush); // center stub

    images.Connect(graphDB, "images");

    // qDebug() << "Data loading" << FN;

    images.AutoLoadAllData();

    int k = 0;
    QGraphicsItem* newItem;

    for (auto dataNodeIter = images.dataNodes.begin(); dataNodeIter != images.dataNodes.end(); ++dataNodeIter)
    {
        QPixmap pixMap(pixmapSizeFixed,pixmapSizeFixed);

        QByteArray imageData;
        QDataStream dataStream(&imageData, QIODevice::ReadOnly);

        imageData = dataNodeIter.value()["bytearray"].toByteArray();

        // qDebug() << "Size: " << imageData.size() << " name: " << dataNodeIter.value()["name"].toString() << FN;

        dataStream >> pixMap;

        // pixMap = pixMap.scaled(pixmapSizeFixed,pixmapSizeFixed);

        newItem = iconsScene.addPixmap(pixMap);

                // add to the map
        imagesPix.insert(dataNodeIter.key(), pixMap);

        if (newItem)
        {
            // qDebug() << "dataNodeIter.key(): " << QVariant((int) dataNodeIter.key());

            newItem->setData(0, dataNodeIter.key());

            // qDebug() << "new item ID: " << newItem-> data(0).toInt();

            newItem->setPos(-750 + k*(pixmapSizeFixed+10), -pixmapSizeFixed);
            // qDebug() << "new item placed at " << -750 + k*50 << FN;

        }

        k++;

    }
}

void FingersTreeForm::LoadGraph()
{
    QGraphicsItem* newItem;

    QPixmap pixMap(pixmapSizeFixed,pixmapSizeFixed);
    QString fileName("test.png");

    bool loadRes = pixMap.load(fileName);

    if (loadRes)
        pixMap = pixMap.scaled(pixmapSizeFixed,pixmapSizeFixed);
    else
        pixMap.fill(Qt::green);

    /*QTableWidgetItem* theItem = new QTableWidgetItem();
    theItem-> setData(Qt::UserRole, fileName);
    // theItem->setFlags(Qt::ItemIsDragEnabled);
    theItem-> setIcon(pixMap);

    ui->tableWidget-> setItem(0, 0, theItem);


    QBrush circleBrush = QBrush(QColor(0xa6, 0xce, 0x39)); // gradient
    QPen circlePen = QPen(Qt::black);
    circlePen.setWidth(1);
    */

    QPen linkPen = QPen(Qt::darkGreen);
    linkPen.setWidth(2);

    /*
    iconsScene.clear();

    iconsScene.addRect(QRect(0, 0, 0, 0)); //, circleBrush); // center stub


    for (int k = 0 ; k < 10; k++)
    {
        newItem = iconsScene.addPixmap(pixMap);
        if (newItem)
            newItem->setPos(-750 + k*50, -pixmapSizeFixed);
    }
*/

    scene.clear();
    firstNodeStored = false;

        // get coords from egDb

    // qDebug() << "Before connect" << FN;

    nodes.Connect(graphDB, "locations");
    linktype.Connect(graphDB, "linktype", nodes, nodes);


    // qDebug() << "Data loading" << FN;

    nodes.AutoLoadAllData();

    linktype.LoadLinks();
    linktype.ResolveLinksToPointers();

    // qDebug() << "Loaded" << FN;

    QList<QVariant> locValues;

        // nodes size and center
    // int w = pixmapSizeFixed;
    // int h = pixmapSizeFixed;

    int x = 0;
    int y = 0;

    int type;

        // add nodes to the scene - QMap<EgDataNodeIDtype, EgDataNode>::iterator
    // for (auto dataNodeIter = nodes.dataNodes.begin(); dataNodeIter != nodes.dataNodes.end(); ++dataNodeIter)
    for (auto& dataNodeIter : nodes.dataNodes)
    {      
        nodes.GetLocation(locValues, dataNodeIter.dataNodeID); //key());

        if (locValues.count() > 2)
        {

            x = locValues[0].toInt();
            y = locValues[1].toInt();

            type = locValues[2].toInt();

            QPixmap pixMap(imagesPix[type]);

            /*
            if (type == 1)
                newItem = scene.addEllipse(QRect(0, 0, w, h), circlePen, circleBrush);
            else // if (type == 2)
                //newItem = scene.addRect(QRect(x-pixmapSizeFixed/2, y-pixmapSizeFixed/2, w, h), circlePen, circleBrush);
                */

            newItem = scene.addPixmap(pixMap);

            if (newItem)
            {
                newItem->setPos(x-pixmapSizeFixed/2, y-pixmapSizeFixed/2);
                newItem->setData(0, dataNodeIter.dataNodeID); //key()); // nodeID

                // save first item to push lines below
                if (! firstNodeStored)
                {
                    firstNode = newItem;
                    firstNodeStored = true;
                }

                QGraphicsTextItem* label = new QGraphicsTextItem(dataNodeIter["name"].toString()); // .value()
                label->setPos(x-pixmapSizeFixed+10, y-pixmapSizeFixed);

                scene.addItem(label);
            }

            // qDebug() << msg << FN;
        }
    }

        // destination node center
    int xDst = 0;
    int yDst = 0;

        // add links - QMap <EgDataNodeIDtype, EgDataNode>::iterator
    // for (auto dataNodeIter = nodes.dataNodes.begin(); dataNodeIter != nodes.dataNodes.end(); ++dataNodeIter)
    for (auto const& dataNodeIter : nodes.dataNodes)
    {
        nodes.GetLocation(locValues, dataNodeIter.dataNodeID); // .key());

        if (locValues.count() > 1)
        {
            x = locValues[0].toInt();
            y = locValues[1].toInt();
        }

            // get linked nodes
        if (dataNodeIter.nodeLinks && (! dataNodeIter.nodeLinks-> outLinks.empty())) // value()
        {
            // qDebug() << "dataNodeIter.nodeLinks-> outLinks : "  << dataNodeIter.nodeLinks-> outLinks.count();

                // QList<EgExtendedLinkType>::iterator
            // for (auto linksIter  = dataNodeIter.value().nodeLinks-> outLinks["linktype"].begin();
            //          linksIter != dataNodeIter.value().nodeLinks-> outLinks["linktype"].end(); ++linksIter)

            for (auto const& linksIter : dataNodeIter.nodeLinks-> outLinks["linktype"])
            {
                // qDebug()  << "Dst node ID = " << linksIter.dataNodePtr-> dataNodeID << FN;

                nodes.GetLocation(locValues, linksIter.dataNodePtr-> dataNodeID);

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


void ItemsMenuGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // QPoint clickPoint = mouseEvent->scenePos().toPoint();


    // isPressed = true;

    // saveX = clickPoint.x();
    // saveY = clickPoint.y();
/*
    QPixmap pixMap(pixmapSizeFixed,pixmapSizeFixed);
    QString fileName("test.png");

    bool loadRes = pixMap.load(fileName);

    if (loadRes)
        pixMap = pixMap.scaled(pixmapSizeFixed,pixmapSizeFixed);
    else
        pixMap.fill(Qt::green);
        */

    theItem = itemAt(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), deviceTransform);

    if (theItem)
    {
        int imageID = theItem-> data(0).toInt();

        // qDebug() << "image ID:"  << imageID;

        QPixmap pixMap(myForm-> imagesPix[imageID]);

        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);

        dataStream << pixMap << QPoint(pixmapSizeFixed/2,pixmapSizeFixed/2);

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-dnditemdata", itemData);
        mimeData->setImageData(imageID);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixMap);
        drag->setHotSpot(QPoint(pixmapSizeFixed/2,pixmapSizeFixed/2));

        drag->start(Qt::CopyAction);
        // drag->exec(Qt::MoveAction);
    }

    // if (theItem)
    //    qDebug() << "item pos:"  << theItem->pos().toPoint().x() << ", "
    //                             << theItem->pos().toPoint().y();
}

/*
void ItemsMenuGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    // qDebug() << "Drag move event at " <<  event-> scenePos().x() << " " << event-> scenePos().y();
}
*/

/*
void ItemsMenuGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    // Q_UNUSED(event);
    // dragOver = false;
    // update();

    // qDebug() << "Drag leave event at " <<  event-> scenePos().x() << " " << event-> scenePos().y();
}
*/

void MyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // QPoint clickPoint = mouseEvent->scenePos().toPoint();

    // saveX = clickPoint.x();
    // saveY = clickPoint.y();

    if (mouseEvent->button() == Qt::LeftButton)
    {
        isPressed = true;

        theItem = itemAt(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), deviceTransform);
    }

    // if (theItem)
        // theItem->moveBy(-100,-100);
        // qDebug() << "item at:" << theItem->boundingRect().center().toPoint().x() << " " <<  theItem-> boundingRect().center().toPoint().y();

    // if (theItem)
    //    qDebug() << "item at:"  << theItem->boundingRect().center().toPoint().x() << ", "
    //                            << theItem->boundingRect().center().toPoint().y();

    // if (theItem)
    //    qDebug() << "item scenePos:"  << theItem->scenePos().toPoint().x() << ", "
    //                            << theItem->scenePos().toPoint().y();

    // if (theItem)
    //    qDebug() << "item pos:"  << theItem->pos().toPoint().x() << ", "
    //                            << theItem->pos().toPoint().y();
}

void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // QPoint clickPoint = mouseEvent->scenePos().toPoint();
    QPen linkPen = QPen(Qt::magenta); // darkRed
    linkPen.setWidth(2);

    QGraphicsItem* theDestItem = nullptr;

    theDestItem = itemAt(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), deviceTransform);

    if (isMoved && theItem && theDestItem && (theItem != theDestItem) && (theItem-> data(0).toInt()) && (theDestItem-> data(0).toInt()))
    {
        // QGraphicsItem * theLine = addLine(theItem->boundingRect().center().toPoint().x(), theItem->boundingRect().center().toPoint().y(),
        //        theDestItem->boundingRect().center().toPoint().x(), theDestItem->boundingRect().center().toPoint().y(), linkPen);

        QGraphicsItem * theLine = addLine(theItem->scenePos().toPoint().x()+pixmapSizeFixed/2, theItem->scenePos().toPoint().y()+pixmapSizeFixed/2,
                theDestItem->scenePos().toPoint().x()+pixmapSizeFixed/2, theDestItem->scenePos().toPoint().y()+pixmapSizeFixed/2, linkPen);

        if (myForm-> firstNode)
            theLine->stackBefore(myForm-> firstNode);
        else
        {
            theLine->stackBefore(theItem);
            theLine->stackBefore(theDestItem);
        }

        // qDebug() << "from node: " <<  theItem-> data(0).toInt() << " to node: " <<  theDestItem-> data(0).toInt();

        myForm-> linktype.AddArrowLink(theItem-> data(0).toInt(), theDestItem-> data(0).toInt());
    }

    isPressed = false;
    isMoved = false;

    // saveX = 0;
    // saveY = 0;

    // qDebug() << "release at:" <<  clickPoint.x() << " " << clickPoint.y();
}


void MyGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
     if (event->mimeData()->hasFormat("application/x-dnditemdata"))
        // event->accept();
        event->setAccepted(true);
    // event->acceptProposedAction();
     else
        // event->ignore();
        event->setAccepted(false);


    // qDebug() << "Drag enter event at " <<  event-> scenePos().x() << " " << event-> scenePos().y();


}

void MyGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    // qDebug() << "Move event at " <<  event-> scenePos().x() << " " << event-> scenePos().y();
}

void MyGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    // qDebug() << "dropped item ID: " << event->mimeData()->imageData().toInt();

    int imageID = event->mimeData()->imageData().toInt();

    QPixmap pixMap(myForm-> imagesPix[imageID]);

    QGraphicsItem* newItem = addPixmap(pixMap);

    QPoint clickPoint = event->scenePos().toPoint();

    if (newItem)
    {
        newItem->setPos(clickPoint.x()-pixmapSizeFixed/2, clickPoint.y()-pixmapSizeFixed/2);

        EgDataNodeIdType newID;
        QList<QVariant> addValues;
        QList<QVariant> locValues;

        QString nodeName = QVariant(clickPoint.x()).toString() + "," + QVariant(clickPoint.y()).toString();

        addValues.clear();
        addValues << nodeName << 2;

        myForm-> nodes.AddDataNode(addValues, newID);

        newItem->setData(0, newID); // nodeID

        locValues.clear();
        locValues << clickPoint.x() << clickPoint.y() << imageID;

        myForm-> nodes.AddLocation(locValues, newID);

        if (! myForm-> firstNode)
            myForm-> firstNode = newItem;
    }

    // qDebug() << "Drop event at " <<  event-> scenePos().x() << " " << event-> scenePos().y();
}

void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // QPoint clickPoint = mouseEvent->scenePos().toPoint();

    if (isPressed)
    {
        isMoved = true;
    }
}

/*
void MyGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // qDebug() << mouseEvent->scenePos();

    if (! myForm-> nodes.isConnected)
        return;

    QPoint clickPoint = mouseEvent->scenePos().toPoint();
}
*/

void MyGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent *wheelEvent)
{
    // qDebug() <<  wheelEvent->delta();

    if (wheelEvent->delta() > 0)
    {
        if (scaleFactor < 5.0)
        {
            myForm->ui->graphicsView->scale(1.25, 1.25); // (2.0, 2.0);
            scaleFactor *= 1.25;
        }
    }
    else
    {
        if (scaleFactor > 1/5.0)
        {
            myForm->ui->graphicsView->scale(0.8, 0.8); // (0.5, 0.5);
            scaleFactor *= 0.8;
        }
    }

    // QGraphicsScene::wheelEvent(wheelEvent);
}

FingersTreeForm::~FingersTreeForm()
{
    delete ui;
}

void FingersTreeForm::on_loadButton_clicked()
{
    LoadImages();
    LoadGraph();
}

void FingersTreeForm::on_saveButton_clicked()
{
    nodes.StoreData();
    linktype.StoreLinks();
}
