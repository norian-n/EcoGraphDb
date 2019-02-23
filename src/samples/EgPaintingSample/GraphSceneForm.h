#ifndef GRAPHSCENEFORM_H
#define GRAPHSCENEFORM_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QMap>

// #include <QGraphicsSceneMouseEvent>

#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"

const int pixmapSizeFixed  = 40;

namespace Ui {
class GraphSceneForm;
}

class FingersTreeForm;

class MyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    // explicit GraphicsScene(QObject *parent = 0);
    // virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);

    virtual void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

    FingersTreeForm* myForm = nullptr;
signals:

public slots:

private:
    // QList <QPointF> m_points;

    bool isPressed = false;

    bool isMoved = false;

    int saveX = 0;
    int saveY = 0;

    qreal scaleFactor = 1.0;

    QTransform deviceTransform;
    QGraphicsItem* theItem = nullptr;
};

class ItemsMenuGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:

    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

    // virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);
    // virtual void dragLeaveEvent(QGraphicsSceneDragDropEvent *event);

    /*
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
    */



    FingersTreeForm* myForm = nullptr;
signals:

public slots:

private:
    // QList <QPointF> m_points;

    /*bool isPressed = false;

    bool isMoved = false;

    int saveX = 0;
    int saveY = 0;
    */
    QTransform deviceTransform;
    QGraphicsItem* theItem = nullptr;
};

class FingersTreeForm : public QWidget
{
    Q_OBJECT

public:
    Ui::GraphSceneForm *ui;

    EgDataNodesType nodes;
    EgDataNodesType images;

    QMap <EgDataNodeIdType, QPixmap> imagesPix;

    bool firstNodeStored = false;
    QGraphicsItem* firstNode = nullptr;

    explicit FingersTreeForm(QWidget *parent = 0);
    ~FingersTreeForm();

private slots:
    void on_loadButton_clicked();

    void on_saveButton_clicked();

private:

    MyGraphicsScene scene;
    ItemsMenuGraphicsScene iconsScene;
    EgGraphDatabase graphDB;

    void LoadImages();
    void LoadGraph();
};

#endif // GRAPHSCENEFORM_H
