#ifndef PROJECT_FORM_H
#define PROJECT_FORM_H

#include <QDialog>

#include "ui_NodeForm.h"
#include "../../egDataNodesType.h"

class GraphSceneForm;

class NodeForm :  public QDialog
{
    Q_OBJECT

public:
    NodeForm(QWidget *parent = 0);

    GraphSceneForm* mainCallee;

    EgDataNodeIdType nodeID;
    formMode theFormMode;

    void openNode();

    EgDataNode newProject;

    EgDataNodesType* Nodes;
    EgDataNode theNode;

private slots:

    void okExit();
    void cancelExit();

private:
    Ui::NodeForm *ui;
};

#endif // PROJECT_FORM_H
