/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "projects.h"

#include <QtDebug>
#include <QMessageBox>
#include <QTextCodec>

/*
int FilterTest (QList<QVariant>& obj_fields_values, QList<QVariant>& filter_values, QMap<QString, int>& obj_field_indexes)
{
      // qDebug() << "FilterTest(): callback called";

      if ((obj_fields_values.count() > obj_field_indexes["owner"]) && filter_values.count()) // filter value set assert
      {
          if (obj_fields_values[ obj_field_indexes["owner"] ] == filter_values[0])  // where Projects.owner == 1
              return 1; // good data
      }
      return 0; // bad data
}
*/

ProjectsForm::ProjectsForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectsForm),
    added_row(1),
    project_form(NULL),
    funcblocks_form(NULL)
{

    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    graphDB.Connect();

    // test_server.server_address = "106.109.9.43";
        // dataclasses
    // Projects.Connect("projects", &test_server);
    // Projects.PrintFieldDesc();

    // Projects.PrintFieldDesc();
    // Projects.LoadData();
        // filter test
    /*
    Projects.filter_values.clear();
    Projects.filter_values << 2;

    Projects.RemoteFilterID = 1; // use remote filter #1
    // Projects.SetLocalFilter(&FilterTest);
    */
        // ref obj classes
    // Statuses.Connect("statuses", &test_server);
    // Statuses.PrintFieldDesc();

    Statuses.Connect(graphDB, "statuses");
    Statuses.LoadAllData();
    // Statuses.PrintObjData();

    // Owners.Connect("owners", &test_server);
    //Owners.PrintFieldDesc();

    Owners.Connect(graphDB, "owners");
    Owners.LoadAllData();

    // Owners.PrintObjData();

    Projects.Connect(graphDB, "projects");

    Projects.getGUIinfo();

    Projects.GUI.AddAutoSubstitute("status", Statuses, "status");
    Projects.GUI.AddAutoSubstitute("owner",  Owners,   "login");


    /*
            // datalinks:  status - create and set autolink
        ProjectStatusLink = new LinkData(&Projects, &Statuses); // create link
        ProjectStatusLink->SetAutoLinkFields(Projects.metaInfo.nameToOrder["status"], Statuses.metaInfo.nameToOrder["status"]); // set autolink
            // datalinks: owner - create and set autolink
        ProjectOwnerLink = new LinkData(&Projects, &Owners); // create link
        ProjectOwnerLink->SetAutoLinkFields(Projects.metaInfo.nameToOrder["owner"],Owners.metaInfo.nameToOrder["login"]); // set autolink

        autolinks_list << ProjectStatusLink << ProjectOwnerLink;
    */

        // model for projects - create and set onChange callback
    model = new QStandardItemModel(); // 0, Projects.ModelFieldsCount() count control descriptors only
    connect(model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(model_data_changed(const QModelIndex&, const QModelIndex&)));

    // IC RootCond = IC("owner", EQ, 2) &&  IC("status", EQ, 3);

    // Projects.LoadData(IC("owner", EQ, 2) &&  IC("status", EQ, 3));

    Projects.LoadAllData();

    refreshView();

}

ProjectsForm::~ProjectsForm()
{
            // FIXME close all

}

void ProjectsForm::closeEvent(QCloseEvent* event)
{
    if (project_form)
        project_form-> close();

    if (funcblocks_form)
        funcblocks_form -> close();
}

inline void ProjectsForm::InitProjectForm()   // project details form setup
{
    project_form = new ProjectForm;
    project_form->main_callee = this;
    project_form->Projects = &Projects;
    project_form->Statuses = &Statuses;
    project_form->Owners = &Owners;
    project_form->initProject();
}

void ProjectsForm::on_addProjectButton_clicked()
{
    if (! project_form)
        InitProjectForm();

    project_form-> project_id = 0;

    project_form-> openProject();

    project_form-> hide();
    project_form-> setWindowModality(Qt::WindowModal);
    project_form-> show();
}

void ProjectsForm::on_editProjectButton_clicked()
{
    if (! project_form)
        InitProjectForm();

    project_form->project_id = model->item(Projects.GUI.model_current_row,0)->data(data_id).toInt();

    project_form-> openProject();

    project_form-> hide();
    project_form-> setWindowModality(Qt::WindowModal);
    project_form-> show();
}

/*
void ProjectsForm::on_saveButton_clicked()
{
        // get data from model
    Projects.GUI.DataFromModel(model);
        // save data
    Projects.StoreData();
}
*/

/*
void ProjectsForm::on_loadButton_clicked()
{
    // QString a = QString("status");
        // get data from server or file
    // QVariant myParam1(1);
    // QVariant myParam2(2);

    Projects.LoadAllData();


    // Projects.SetFilter();

    // IndexCondition CondPit(Projects, "odb_pit", GE, myParam1);
    // IndexCondition CondStatus(Projects, "status", EQ, myParam2);
*/
     // IC RootCond =
     //       /*(IC(Projects, "owner", EQ, 1) || IC(Projects, "odb_pit", GE, 1) && */ IC(Projects, "status", EQ, 2);

    // IndexCondition RootCond = IndexCondition(Projects, "odb_pit", GE, myParam1) && IndexCondition(Projects, "status", EQ, myParam2);

    // IndexCondition RootCond = (CondPit && CondStatus) || (CondPit && CondStatus);

    // Projects.LoadData();
/*
    refreshView();
}
*/

void ProjectsForm::refreshView()
{
        // detach model
    ui->tableView->setModel(NULL);
        // move data to model
    Projects.GUI.DataToModel(model);
        // attach model
    ui->tableView->setModel(model);
    Projects.GUI.SetViewWidths(ui->tableView);
}

void ProjectsForm::on_addButton_clicked()
{
    /*
    QList<QVariant> new_fields;
    QList<QStandardItem *> items;
        // clear and assign
    for (int k = 0; k < Projects.FieldsCount(); k++)
        new_fields << QVariant();
        */

    /*Projects.AddNewData(new_fields);
    Projects.StoreData();

    refreshView();
    */

    // Projects.GUI.AddRowOfModel(model, items);

    if (! funcblocks_form)
    {
        funcblocks_form = new FuncBlocksForm();
    }

    if (model-> item(Projects.GUI.model_current_row,0))
    {
        funcblocks_form-> projectID = model-> item(Projects.GUI.model_current_row,0)-> data(data_id).toInt();;
        funcblocks_form-> loadFuncblocks();
        funcblocks_form-> hide();
        funcblocks_form-> setWindowModality(Qt::WindowModal);
        funcblocks_form-> show();
    }

}

void ProjectsForm::on_deleteButton_clicked()
{
        // delete row in dataclass and model
    Projects.GUI.DeleteRowOfModel(model);
}

void ProjectsForm::on_tableView_clicked(QModelIndex index)
{
        // set current data row index
    Projects.GUI.model_current_row = index.row();
}

// const QModelIndex & topLeft, const QModelIndex & bottomRight
void ProjectsForm::model_data_changed(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
        // update current index
    Projects.GUI.model_current_row = topLeft.row();
        // modify row in dataclass and model
    Projects.GUI.ModifyRowOfModel(model);
}


