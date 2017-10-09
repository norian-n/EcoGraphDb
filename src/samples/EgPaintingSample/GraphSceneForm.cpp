#include "GraphSceneForm.h"
#include "ui_GraphSceneForm.h"

#include <QtDebug>
#include <QGraphicsTextItem>

GraphSceneForm::GraphSceneForm(QWidget *parent) :
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

    ui->tableWidget-> setIconSize(QSize(40,40));
    */



    scene.myForm = this;
    iconsScene.myForm = this;

    graphDB.Connect();


    // LoadGraph();
}

void GraphSceneForm::LoadImages()
{
    iconsScene.clear();

    iconsScene.addRect(QRect(0, 0, 0, 0)); //, circleBrush); // center stub




    QPixmap pixMap(40,40);
    QByteArray imageData;
    QDataStream dataStream(&imageData, QIODevice::ReadOnly);

    images.Connect(graphDB, "images");

    // qDebug() << "Data loading" << FN;

    images.AutoLoadAllData();

    int k = 0;
    QGraphicsItem* newItem;

    for (auto dataNodeIter = images.dataNodes.begin(); dataNodeIter != images.dataNodes.end(); ++dataNodeIter)
    {
        imageData = dataNodeIter.value()["bytearray"].toByteArray();

        // qDebug() << "Size: " << imageData.size() << " content:" << imageData << FN;

        dataStream >> pixMap;

        // pixMap = pixMap.scaled(40,40);

        newItem = iconsScene.addPixmap(pixMap);
        if (newItem)
        {
            newItem->setPos(-750 + k*50, -40);
            k++;
        }
    }
}

void GraphSceneForm::LoadGraph()
{
    QGraphicsItem* newItem;

    QPixmap pixMap(40,40);
    QString fileName("test.png");

    bool loadRes = pixMap.load(fileName);

    if (loadRes)
        pixMap = pixMap.scaled(40,40);
    else
        pixMap.fill(Qt::green);

    /*QTableWidgetItem* theItem = new QTableWidgetItem();
    theItem-> setData(Qt::UserRole, fileName);
    // theItem->setFlags(Qt::ItemIsDragEnabled);
    theItem-> setIcon(pixMap);

    ui->tableWidget-> setItem(0, 0, theItem);
    */

    QBrush circleBrush = QBrush(QColor(0xa6, 0xce, 0x39)); // gradient
    QPen circlePen = QPen(Qt::black);
    circlePen.setWidth(1);

    QPen linkPen = QPen(Qt::darkGreen);
    linkPen.setWidth(2);

    /*
    iconsScene.clear();

    iconsScene.addRect(QRect(0, 0, 0, 0)); //, circleBrush); // center stub


    for (int k = 0 ; k < 10; k++)
    {
        newItem = iconsScene.addPixmap(pixMap);
        if (newItem)
            newItem->setPos(-750 + k*50, -40);
    }
*/

    scene.clear();
    firstNodeStored = false;

        // get coords from egDb

    // qDebug() << "Before connect" << FN;

    nodes.Connect(graphDB, "locations");

    // qDebug() << "Data loading" << FN;

    nodes.AutoLoadAllData();

    // qDebug() << "Loaded" << FN;

    nodes.LoadLinkType("linktype");
    nodes.myLinkTypes["linktype"]-> ResolveLinks(nodes, nodes);

    QList<QVariant> locValues;

        // nodes size and center
    int w = 40;
    int h = 40;

    int x = 0;
    int y = 0;

    int type;

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
                newItem = scene.addEllipse(QRect(0, 0, w, h), circlePen, circleBrush);
            else // if (type == 2)
                //newItem = scene.addRect(QRect(x-20, y-20, w, h), circlePen, circleBrush);
                newItem = scene.addPixmap(pixMap);

            if (newItem)
                newItem->setPos(x-20, y-20);

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


void ItemsMenuGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // QPoint clickPoint = mouseEvent->scenePos().toPoint();


    // isPressed = true;

    // saveX = clickPoint.x();
    // saveY = clickPoint.y();

    QPixmap pixMap(40,40);
    QString fileName("test.png");

    bool loadRes = pixMap.load(fileName);

    if (loadRes)
        pixMap = pixMap.scaled(40,40);
    else
        pixMap.fill(Qt::green);

    theItem = itemAt(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), deviceTransform);

    if (theItem)
    {
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);
        dataStream << pixMap << QPoint(20,20);

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("application/x-dnditemdata", itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setPixmap(pixMap);
        drag->setHotSpot(QPoint(20,20));

        drag->start(Qt::CopyAction);
        // drag->exec(Qt::MoveAction);
    }

    // if (theItem)
    //    qDebug() << "item pos:"  << theItem->pos().toPoint().x() << ", "
    //                             << theItem->pos().toPoint().y();
}


void ItemsMenuGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    // qDebug() << "Drag move event at " <<  event-> scenePos().x() << " " << event-> scenePos().y();
}

void ItemsMenuGraphicsScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    // Q_UNUSED(event);
    // dragOver = false;
    // update();

    // qDebug() << "Drag leave event at " <<  event-> scenePos().x() << " " << event-> scenePos().y();
}

void MyGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // QPoint clickPoint = mouseEvent->scenePos().toPoint();

    isPressed = true;

    // saveX = clickPoint.x();
    // saveY = clickPoint.y();

    theItem = itemAt(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), deviceTransform);

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

        QGraphicsItem * theLine = addLine(theItem->scenePos().toPoint().x()+20, theItem->scenePos().toPoint().y()+20,
                theDestItem->scenePos().toPoint().x()+20, theDestItem->scenePos().toPoint().y()+20, linkPen);

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
    // addRect(QRect(event-> scenePos().x()-20, event-> scenePos().y()-20, 40, 40)); // , circlePen, circleBrush

    QPixmap pixMap(40,40);
    QString fileName("test.png");

    bool loadRes = pixMap.load(fileName);

    if (loadRes)
        pixMap = pixMap.scaled(40,40);
    else
        pixMap.fill(Qt::green);

    QGraphicsItem* newItem = addPixmap(pixMap);

    QPoint clickPoint = event->scenePos().toPoint();

    if (newItem)
        newItem->setPos(clickPoint.x()-20, clickPoint.y()-20);

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

    QGraphicsItem* newItem = addEllipse(QRect(0, 0, w, h), circlePen, circleBrush);

    if (newItem)
    {
        newItem->setPos(clickPoint.x()-20, clickPoint.y()-20);

        EgDataNodeIDtype newID;
        QList<QVariant> addValues;
        QList<QVariant> locValues;

        QString nodeName = QVariant(clickPoint.x()).toString() + "," + QVariant(clickPoint.y()).toString();

        addValues.clear();
        addValues << nodeName << 2;

        myForm-> nodes.AddDataNode(addValues, newID);

        newItem->setData(0, newID); // nodeID

        locValues.clear();
        locValues << clickPoint.x() << clickPoint.y() << 1;

        myForm-> nodes.AddLocation(locValues, newID);

        if (! myForm-> firstNode)
            myForm-> firstNode = newItem;
    }

    // QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
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

    // QGraphicsScene::wheelEvent(wheelEvent);
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
    LoadImages();
    LoadGraph();
}

void GraphSceneForm::on_saveButton_clicked()
{
    SaveGraph();
}
