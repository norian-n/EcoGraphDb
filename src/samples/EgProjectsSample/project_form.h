#ifndef PROJECT_FORM_H
#define PROJECT_FORM_H

#include "ui_project_form.h"
#include "../../egDataNodesType.h"

class MainWindow;

/*
class CustomProject
{
public:

    QString name;
    int status;
    QString owner;
    QDate launch_date;
    QDate end_date;
    QString description;
};
*/

class ProjectForm :  public QWidget
{
    Q_OBJECT

public:
    ProjectForm(QWidget *parent = 0);

    MainWindow * main_callee;
//    SubProjectsForm * sub_callee;
    int project_id;
    void initProject();
    void openProject();
        // primary table
    /*

    DataObjects* Projects;
    DataObj TheProject; // local ptr, move to
        // references
    DataObjects* Statuses;
    DataObjects* Owners;
    */
    EgDataNode newProject;

    EgDataNodesType* Projects;
    EgDataNode theProject; // update or delete

        // references
    EgDataNodesType* Statuses;
    EgDataNodesType* Owners;

    // CustomProject cp;

    // QList<QVariant> new_fields;

private slots:
    // void on_inputSpinBox1_valueChanged(int value);
    // void on_inputSpinBox2_valueChanged(int value);
    // void addProject();
    // void deleteSubProject();
    void okExit();
    void cancelExit();

private:
    Ui::ProjectForm *ui;
};

#endif // PROJECT_FORM_H
