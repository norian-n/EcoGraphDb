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


    QAction* openAct = new QAction(tr("Node info"), this);
    openAct->setStatusTip(tr("Open node contents form"));

    connect(openAct, &QAction::triggered, &scene, &MyGraphicsScene::editNodeContent);

    ui->graphicsView-> addAction(openAct);

    openAct = new QAction(tr("Delete node"), this);
    openAct->setStatusTip(tr("Delete this node"));

    ui->graphicsView-> addAction(openAct);

    connect(openAct, &QAction::triggered, &scene, &MyGraphicsScene::deleteNode);

    /*
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct); popupMenuTest
    fileToolBar->addAction(openAct);
    */

/*    for (int counterC = 0; counterC < ui->tableWidget->columnCount(); counterC++)
        ui->tableWidget->setColumnWidth(counterC, 50);

    for (int counterR = 0; counterR < ui->tableWidget->rowCount(); counterR++)
        ui->tableWidget->setRowHeight(counterR, 50);

    ui->tableWidget-> setIconSize(QSize(pixmapSizeFixed,pixmapSizeFixed));
    */

    scene.setSceneRect(-300, -150, 600, 300);

    scene.myForm = this;
    iconsScene.myForm = this;

    ui->connectsModeButton-> setDown(true);

    // graphDB.Connect();


    // LoadGraph();
}

void GraphSceneForm::LoadImages()
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

void GraphSceneForm::LoadGraph()
{
    nodes.Connect(graphDB, "locations");

    if (! linktype.isConnected)
        linktype.Connect(graphDB, "linktype", nodes, nodes);

    // qDebug() << "Data loading" << FN;

    nodes.AutoLoadAllData();

    linktype.LoadLinks();
    linktype.ResolveLinksToPointers();
}

void GraphSceneForm::ShowGraphNodes()
{
    QGraphicsItem* newItem;

    scene.clear();
    firstNodeStored = false;

    QList<QVariant> locValues; // node location - x,y

    int x = 0;
    int y = 0;

    int type;

        // add nodes to the scene - QMap<EgDataNodeIDtype, EgDataNode>::iterator
    for (auto& dataNodeIter : nodes.dataNodes)
    {      
        nodes.GetLocation(locValues, dataNodeIter.dataNodeID);

        if (locValues.count() > 2)
        {

            x = locValues[0].toInt();
            y = locValues[1].toInt();

            type = locValues[2].toInt();

            QPixmap pixMap(imagesPix[type]);

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
}

void GraphSceneForm::ShowGraphLinks()
{

    QList<QVariant> locValues; // node location - x,y

    QPen linkPen = QPen(Qt::darkGreen);
    linkPen.setWidth(2);

    int x = 0;
    int y = 0;

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


/*
QPixmap pixMap(pixmapSizeFixed,pixmapSizeFixed);
QString fileName("test.png");

bool loadRes = pixMap.load(fileName);

if (loadRes)
    pixMap = pixMap.scaled(pixmapSizeFixed,pixmapSizeFixed);
else
    pixMap.fill(Qt::green);

        if (type == 1)
            newItem = scene.addEllipse(QRect(0, 0, w, h), circlePen, circleBrush);
        else // if (type == 2)
            //newItem = scene.addRect(QRect(x-pixmapSizeFixed/2, y-pixmapSizeFixed/2, w, h), circlePen, circleBrush);

    */

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
        mimeData-> setData("application/x-dnditemdata", itemData);
        mimeData-> setImageData(imageID);

        QDrag *drag = new QDrag(this);
        drag-> setMimeData(mimeData);
        drag-> setPixmap(pixMap);
        drag-> setHotSpot(QPoint(pixmapSizeFixed/2,pixmapSizeFixed/2));

        drag-> exec(Qt::CopyAction);
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
    myForm-> movedNodeID = 0; // reset

    theItem = itemAt(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), deviceTransform);

    if (mouseEvent->button() == Qt::LeftButton)
    {
        isPressed = true;

        if (theItem && (myForm-> opsMode == sfModeMoving))
        {
            int nodeID = theItem-> data(0).toInt();

            // qDebug() << "image ID:"  << imageID;

            if (nodeID > 0) // is node
            {

                QPixmap pixMap(static_cast <QGraphicsPixmapItem*> (theItem) -> pixmap()); //(myForm-> imagesPix[imageID]);

                QByteArray itemData;

                QDataStream dataStream(&itemData, QIODevice::WriteOnly);
                dataStream << pixMap << QPoint(pixmapSizeFixed/2,pixmapSizeFixed/2);

                QMimeData *mimeData = new QMimeData;
                mimeData-> setData("application/x-dnditemdata", itemData);
                // mimeData-> setImageData(nodeID);

                myForm-> movedNodeID = nodeID;

                QDrag *drag = new QDrag(this);
                drag-> setMimeData(mimeData);
                drag-> setPixmap(pixMap);
                drag-> setHotSpot(QPoint(pixmapSizeFixed/2,pixmapSizeFixed/2));

                drag-> exec(Qt::MoveAction);
            }
        }
    }
    else
    {
        if (theItem)
        {
            myForm-> ui->graphicsView-> setContextMenuPolicy(Qt::ActionsContextMenu);

            myForm-> contextMenuNodeID = theItem-> data(0).toInt();
        }
        else
            myForm-> ui->graphicsView-> setContextMenuPolicy(Qt::NoContextMenu);
    }


    //    qDebug() << "mouse right";
}

void MyGraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // QPoint clickPoint = mouseEvent->scenePos().toPoint();
    QPen linkPen = QPen(Qt::magenta); // darkRed
    linkPen.setWidth(2);

    QGraphicsItem* theDestItem = nullptr;

    theDestItem = itemAt(mouseEvent->scenePos().x(), mouseEvent->scenePos().y(), deviceTransform);

    if (/*isMoved &&*/ theItem && theDestItem && (theItem != theDestItem) && (theItem-> data(0).toInt()) && (theDestItem-> data(0).toInt()))
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

    resetButtons();

    // saveX = 0;
    // saveY = 0;

    // qDebug() << "mouseReleaseEvent at: " <<  mouseEvent->scenePos().x() << ", " << mouseEvent->scenePos().y();
}


void MyGraphicsScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
        event->setAccepted(event->mimeData()->hasFormat("application/x-dnditemdata"));

        saveDragX = event-> scenePos().x();
        saveDragY = event-> scenePos().y();

        // qDebug() << "Drag enter event at " <<  event-> scenePos().x() << " " << event-> scenePos().y();
}


void MyGraphicsScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    // qDebug() << "Move event at " <<  event-> scenePos().x() << " " << event-> scenePos().y();
}


inline void MyGraphicsScene::resetButtons()
{
    if (myForm-> opsMode == sfModeMoving)
    {
        myForm->ui->connectsModeButton-> setDown(false);
        myForm->ui->moveModeButton-> setDown(true);
    }
    else
    {
        myForm->ui->connectsModeButton-> setDown(true);
        myForm->ui->moveModeButton-> setDown(false);
    }
}

void MyGraphicsScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    // qDebug() << "Dropped node ID: " << myForm-> movedNodeID;
    // qDebug() << "Drop event at " <<  event-> scenePos().x() << " " << event-> scenePos().y();

        // check if no actual movement after click
    if (std::abs(saveDragX - event-> scenePos().x()) < 1 && std::abs(saveDragY - event-> scenePos().y()) < 1)
    {
        myForm-> movedNodeID = 0; // reset
        resetButtons();

        return;
    }

    QPoint clickPoint = event->scenePos().toPoint();

    if (! myForm-> movedNodeID) // drop from outside
    {

        int imageID = event->mimeData()->imageData().toInt();

        // QPixmap pixMap(myForm-> imagesPix[imageID]);

        QGraphicsItem* newItem = addPixmap(QPixmap(myForm-> imagesPix[imageID]));

        if (newItem)
        {
            newItem->setPos(clickPoint.x()-pixmapSizeFixed/2, clickPoint.y()-pixmapSizeFixed/2);

            EgDataNodeIdType newID;
            QList<QVariant> addValues;
            QList<QVariant> locValues;

            // QString nodeName = QVariant(clickPoint.x()).toString() + "," + QVariant(clickPoint.y()).toString();

            addValues.clear();
            addValues << "New node" << 2; // nodeName

            myForm-> nodes.AddDataNode(addValues, newID);

            newItem->setData(0, newID); // nodeID

            locValues.clear();
            locValues << clickPoint.x() << clickPoint.y() << imageID;

            myForm-> nodes.AddLocation(locValues, newID);

            if (! myForm-> firstNode)
                myForm-> firstNode = newItem;

                // show label
            QGraphicsTextItem* label = new QGraphicsTextItem("New node"); // .value()
            label->setPos(clickPoint.x()-pixmapSizeFixed+10, clickPoint.y()-pixmapSizeFixed);

            addItem(label);
        }
    }
    else // inside movement
    {
        QList<QVariant> locValues;

            // get old location
        myForm-> nodes.GetLocation(locValues, myForm-> movedNodeID);

            // set new location
        locValues[0] = clickPoint.x();
        locValues[1] = clickPoint.y();

        myForm-> nodes.UpdateLocation(locValues, myForm-> movedNodeID);

        myForm-> ShowGraphNodes();
        myForm-> ShowGraphLinks();
    }

    myForm-> movedNodeID = 0; // reset

    resetButtons();

    // qDebug() << "Drop event at " <<  event-> scenePos().x() << " " << event-> scenePos().y();
}

/*
void MyGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // QPoint clickPoint = mouseEvent->scenePos().toPoint();

    if (isPressed)
    {
        isMoved = true;
    }

}
*/

/*
void MyGraphicsScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    // qDebug() << mouseEvent->scenePos();

    if (! myForm-> nodes.isConnected)
        return;

    QPoint clickPoint = mouseEvent->scenePos().toPoint();
}
*/

void MyGraphicsScene::editNodeContent()
{
    // qDebug() << "Edit Node Content";

    if (! myForm-> contextMenuNodeID) // bad node id
        return;

    if (! nodeForm)
    {
        nodeForm = new NodeForm();
        nodeForm-> mainCallee = myForm; // PARANOID: check form pointer

        nodeForm-> setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint); //   | Qt::Window
        nodeForm-> setFixedSize(nodeForm->size());
        nodeForm-> setModal(true);
    }

    nodeForm-> theFormMode = formModeEdit;
    nodeForm-> setWindowTitle("Edit Node Data");

    nodeForm-> nodeID = myForm-> contextMenuNodeID;
    nodeForm-> openNode();

    nodeForm-> show();

    myForm-> contextMenuNodeID = 0; // reset
}

void MyGraphicsScene::deleteNode()
{
    if (! myForm-> contextMenuNodeID) // bad node id
        return;

    if (! nodeForm)
    {
        nodeForm = new NodeForm();
        nodeForm-> mainCallee = myForm; // PARANOID: check form pointer

        nodeForm-> setWindowFlags(Qt::WindowTitleHint | Qt::CustomizeWindowHint); //   | Qt::Window
        nodeForm-> setFixedSize(nodeForm->size());
        nodeForm-> setModal(true);
    }

    nodeForm-> theFormMode = formModeDelete;
    nodeForm-> setWindowTitle("Delete Node");

    nodeForm-> nodeID = myForm-> contextMenuNodeID;
    nodeForm-> openNode();

    nodeForm-> show();

    myForm-> contextMenuNodeID = 0; // reset
}


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

GraphSceneForm::~GraphSceneForm()
{
    delete ui;
}

void GraphSceneForm::on_loadButton_clicked()
{
    LoadImages();
    LoadGraph();

    ShowGraphNodes();
    ShowGraphLinks();
}

void GraphSceneForm::on_saveButton_clicked()
{
    nodes.StoreData();
    linktype.StoreLinks();
}


void GraphSceneForm::on_moveModeButton_clicked()
{
    opsMode = sfModeMoving;

    ui->connectsModeButton-> setDown(false);
    ui->moveModeButton-> setDown(true);
}

void GraphSceneForm::on_connectsModeButton_clicked()
{
    opsMode = sfModeConnecting;

    ui->connectsModeButton-> setDown(true);
    ui->moveModeButton-> setDown(false);
}

