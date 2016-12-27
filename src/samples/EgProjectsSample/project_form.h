#ifndef PROJECT_FORM_H
#define PROJECT_FORM_H

#include "ui_project_form.h"
#include "../../egDataNodesType.h"

class ProjectsForm;

class ProjectForm :  public QWidget
{
    Q_OBJECT

public:
    ProjectForm(QWidget *parent = 0);

    ProjectsForm* main_callee;

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
    Ui::ProjectForm *ui;
};

#endif // PROJECT_FORM_H
