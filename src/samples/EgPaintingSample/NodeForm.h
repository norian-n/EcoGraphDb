#ifndef PROJECT_FORM_H
#define PROJECT_FORM_H

#include "ui_node_form.h"
#include "../../egDataNodesType.h"

class GraphSceneForm;

class NodeForm :  public QWidget
{
    Q_OBJECT

public:
    NodeForm(QWidget *parent = 0);

    GraphSceneForm* main_callee;

    int projectID;
    int formMode;

    void initProject();
    void openProject();

    EgDataNode newProject;

    EgDataNodesType* Projects;
    EgDataNode theProject; // update or delete

        // references
    EgDataNodesType* Statuses;
    EgDataNodesType* Owners;

private slots:

    void okExit();
    void cancelExit();

private:
    Ui::NodeForm *ui;
};

#endif // PROJECT_FORM_H
