#ifndef GRAPHSCENEFORM_H
#define GRAPHSCENEFORM_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
// #include <QGraphicsSceneMouseEvent>

#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"

namespace Ui {
class GraphSceneForm;
}

class GraphSceneForm;

class MyGraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    // explicit GraphicsScene(QObject *parent = 0);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void wheelEvent(QGraphicsSceneWheelEvent *wheelEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);

    GraphSceneForm* myForm = nullptr;
signals:

public slots:

private:
    // QList <QPointF> m_points;

    bool isPressed = false;

    bool isMoved = false;

    int saveX = 0;
    int saveY = 0;

    QGraphicsItem* theItem = nullptr;
};

class GraphSceneForm : public QWidget
{
    Q_OBJECT

public:
    Ui::GraphSceneForm *ui;

    EgDataNodesType nodes;

    bool firstNodeStored = false;
    QGraphicsItem* firstNode = nullptr;

    explicit GraphSceneForm(QWidget *parent = 0);
    ~GraphSceneForm();

private slots:
    void on_loadButton_clicked();

    void on_saveButton_clicked();

private:

    MyGraphicsScene scene;
    EgGraphDatabase graphDB;

    void LoadGraph();
    void SaveGraph();
};

#endif // GRAPHSCENEFORM_H
