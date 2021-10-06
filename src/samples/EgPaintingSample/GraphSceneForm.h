// #ifndef GRAPHSCENEFORM_H
// #define GRAPHSCENEFORM_H

#pragma once

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QDrag>
#include <QMap>
#include <QAction>

// #include <QGraphicsSceneMouseEvent>

#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"

#include "NodeForm.h"

const int pixmapSizeFixed  = 40;

namespace Ui {
class GraphSceneForm;
}

class GraphSceneForm;

class MyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    // explicit GraphicsScene(QObject *parent = 0);
    // virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);

    virtual void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);

    // virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent *event);
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent *event);  // dummy required for auto drag & drop
    virtual void dropEvent(QGraphicsSceneDragDropEvent *event);

    GraphSceneForm* myForm = nullptr;  
    NodeForm* nodeForm = nullptr;

signals:

public slots:

    void editNodeContent();
    void deleteNode();

private:
    // QList <QPointF> m_points;

    bool isPressed = false;

    bool isMoved = false;

    qreal saveDragX = 0;
    qreal saveDragY = 0;

    qreal scaleFactor = 1.0;

    QTransform deviceTransform;
    QGraphicsItem* theItem = nullptr;

    inline void resetButtons();
};

class ItemsMenuGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:

    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);

    GraphSceneForm* myForm = nullptr;
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

enum sfModeType
{
    sfModeConnecting,
    sfModeMoving
};

class GraphSceneForm : public QWidget
{
    Q_OBJECT

public:
    Ui::GraphSceneForm *ui;

    EgDataNodesType nodes;
    EgDataNodesType images;

    EgLinkType linktype;

    QMap <EgDataNodeIdType, QPixmap> imagesPix;

    bool firstNodeStored = false;
    QGraphicsItem* firstNode = nullptr;

    sfModeType opsMode {sfModeMoving};
    EgDataNodeIdType movedNodeID {0};
    EgDataNodeIdType contextMenuNodeID {0};

    bool dragDropAction = false;

    explicit GraphSceneForm(QWidget *parent = nullptr);
    ~GraphSceneForm();

    void LoadImages();

    void ShowGraphNodes();
    void ShowGraphLinks();

private slots:

    void on_loadButton_clicked();
    void on_saveButton_clicked();

    void on_moveModeButton_clicked();
    void on_connectsModeButton_clicked();

private:

    MyGraphicsScene scene;
    ItemsMenuGraphicsScene iconsScene;
    EgGraphDatabase graphDB;

    void LoadGraph();
};

// #endif // GRAPHSCENEFORM_H
