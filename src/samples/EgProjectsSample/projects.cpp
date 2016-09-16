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
}


void ProjectsForm::TestLinks()
{
    // Projects.AddArrowLink("treeLinkType", 1, 2);
    // Projects.AddArrowLink("detailLinkType", Projects[1], Detail[2]);
}

void ProjectsForm::on_initButton_clicked()
{
/*
    Statuses.connection = &test_server;
    Owners.connection = &test_server;
    Projects.connection = &test_server;
    Funcblocks.connection = &test_server;
*/
    FillTestData();

    // Projects.ClearIndex("status"); // delete old index files

    // Projects.AddIndex("owner");
    // Projects.PrintFieldDesc();
/*
    if (! funcblocks_form)
        funcblocks_form = new FuncBlocksForm();
    // funcblocks_form->project_id = project_id;
    // funcblocks_form->initFuncBlocks();
    funcblocks_form->show();
    */

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
    project_form-> show();
}

void ProjectsForm::on_editProjectButton_clicked()
{
    if (! project_form)
        InitProjectForm();

    project_form->project_id = model->item(Projects.GUI.model_current_row,0)->data(data_id).toInt();

    project_form-> openProject();
    project_form-> show();
}

void ProjectsForm::on_saveButton_clicked()
{
        // get data from model
    Projects.GUI.DataFromModel(model);
        // save data
    Projects.StoreData();
}


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

     IC RootCond =
            /*(IC(Projects, "owner", EQ, 1) || IC(Projects, "odb_pit", GE, 1) && */ IC(Projects, "status", EQ, 2);

    // IndexCondition RootCond = IndexCondition(Projects, "odb_pit", GE, myParam1) && IndexCondition(Projects, "status", EQ, myParam2);

    // IndexCondition RootCond = (CondPit && CondStatus) || (CondPit && CondStatus);

    // Projects.LoadData();

    refreshView();
}


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

/*
    Funcblocks.Connect(graphDB, "funcblocks");

    Funcblocks.LoadAllData();

    Funcblocks.AddArrowLink("funcblocksTree", 1, Funcblocks, 2);
    Funcblocks.AddArrowLink("funcblocksTree", 2, Funcblocks, 3);
    Funcblocks.AddArrowLink("funcblocksTree", 1, Funcblocks, 4);

    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 1);
    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 2);
    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 3);

    Funcblocks.StoreLinks();

    Projects.StoreLinks();

    Funcblocks.StoreLinks();

*/


    if (! funcblocks_form)
        funcblocks_form = new FuncBlocksForm();

    if (model-> item(Projects.GUI.model_current_row,0))
    {
        funcblocks_form-> project_id = model-> item(Projects.GUI.model_current_row,0)-> data(data_id).toInt();;
        funcblocks_form-> loadFuncblocks();
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


void ProjectsForm::FillTestData()
{
   /* Funcblocks.Connect(graphDB, "funcblocks");

    Funcblocks.LoadAllData();

    Funcblocks.AddArrowLink("funcblocksTree", 1, Funcblocks, 2);
    Funcblocks.AddArrowLink("funcblocksTree", 2, Funcblocks, 3);
    Funcblocks.AddArrowLink("funcblocksTree", 1, Funcblocks, 4);

    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 1);
    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 2);
    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 3);

    Funcblocks.StoreLinks();

    Projects.StoreLinks();

    return;
    */

#define ADD_RECORD(values,dataNodesType) ins_values.clear(); ins_values << values; dataNodesType.AddNewData(ins_values);

    QList<QVariant> ins_values;

    graphDB.CreateNodeType("statuses");

    graphDB.AddDataField("status");

    graphDB.CommitNodeType();

    Statuses.Connect(graphDB, "statuses");

    ADD_RECORD("Planned", Statuses);
    ADD_RECORD("Running", Statuses);
    ADD_RECORD("Hold", Statuses);
    ADD_RECORD("Complete", Statuses);
    ADD_RECORD("Drop", Statuses);

    Statuses.StoreData();

    graphDB.CreateNodeType("owners");

    graphDB.AddDataField("login");
    graphDB.AddDataField("name");
    graphDB.AddDataField("mail");

    graphDB.CommitNodeType();

    Owners.Connect(graphDB, "owners");

    ADD_RECORD("defuser" << "Default User" << "defuser@no.mail", Owners);
    ADD_RECORD("norian" << "Norian" << "norian@no.mail", Owners);

    Owners.StoreData();

        // projects
    graphDB.CreateNodeType("projects");

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", IsIndexed);
    graphDB.AddDataField("description");
    graphDB.AddDataField("owner", IsIndexed);
    graphDB.AddDataField("launch_date");
    graphDB.AddDataField("end_date");

    graphDB.CommitNodeType();

    Projects.Connect(graphDB, "projects");

    Projects.GUI.AddSimpleControlDesc("name",          "Project name",   120);
    Projects.GUI.AddSimpleControlDesc("status",        "Status",         90);
    Projects.GUI.AddSimpleControlDesc("owner",         "Owner",          90);
    Projects.GUI.AddSimpleControlDesc("launch_date",   "Launched",       90);
    Projects.GUI.AddSimpleControlDesc("end_date",      "Finished",       90);
    Projects.GUI.AddSimpleControlDesc("description",   "Description",    150);

        // add reordered fields by name
    ins_values.clear();
    for (int k = 0; k < Projects.FieldsCount(); k++) // init
        ins_values << QVariant();

#define FIELD(name) metaInfo.nameToOrder[name]

    ins_values[Projects.FIELD("name")]        = "ECoGraphDB";
    ins_values[Projects.FIELD("description")] = "Exo Cortex Graph Database Project";
    ins_values[Projects.FIELD("status")]      = 3;
    ins_values[Projects.FIELD("owner")]       = 2;
    ins_values[Projects.FIELD("launch_date")] = QDate(2011,11,2);
    ins_values[Projects.FIELD("end_date")]    = QDate(2015,10,30);

    Projects.AddNewData(ins_values);

    ins_values[Projects.FIELD("name")]        = "Test Project 1";
    ins_values[Projects.FIELD("description")] = "Test Project Description";
    ins_values[Projects.FIELD("status")]      = 1;
    ins_values[Projects.FIELD("owner")]       = 1;
    ins_values[Projects.FIELD("launch_date")] = QDate(2011,11,2);
    ins_values[Projects.FIELD("end_date")]    = QVariant();

    Projects.AddNewData(ins_values);

    ins_values[Projects.FIELD("name")]        = "Test Project 2";
    ins_values[Projects.FIELD("description")] = "Test Project Description";
    ins_values[Projects.FIELD("status")]      = 2;
    ins_values[Projects.FIELD("owner")]       = 2;
    ins_values[Projects.FIELD("launch_date")] = QDate(2011,11,2);
    ins_values[Projects.FIELD("end_date")]    = QVariant();

    Projects.AddNewData(ins_values);

    ins_values[Projects.FIELD("name")]        = "Test Project 3";
    ins_values[Projects.FIELD("description")] = "Test Project Description";
    ins_values[Projects.FIELD("status")]      = 3;
    ins_values[Projects.FIELD("owner")]       = 1;
    ins_values[Projects.FIELD("launch_date")] = QDate(2011,11,2);
    ins_values[Projects.FIELD("end_date")]    = QVariant();

    Projects.AddNewData(ins_values);

    ins_values[Projects.FIELD("name")]        = "Test Project 4";
    ins_values[Projects.FIELD("description")] = "Test Project Description";
    ins_values[Projects.FIELD("status")]      = 4;
    ins_values[Projects.FIELD("owner")]       = 2;
    ins_values[Projects.FIELD("launch_date")] = QDate(2011,11,2);
    ins_values[Projects.FIELD("end_date")]    = QVariant();

    Projects.AddNewData(ins_values);

    // **************************************

    Projects.StoreData();


        // funcblocks

    graphDB.CreateNodeType("funcblocks");

    graphDB.AddDataField("name");
/*
    graphDB.AddDataField("status", IsIndexed);
    graphDB.AddDataField("description");
    graphDB.AddDataField("owner", IsIndexed);
    graphDB.AddDataField("launch_date");
    graphDB.AddDataField("end_date");
    */

    graphDB.CommitNodeType();

        // add link type

    graphDB.CreateLinksMetaInfo();
    graphDB.AddLinkType("funcblocksTree", "funcblocks", "funcblocks");
    graphDB.AddLinkType("projects_funcblocks", "projects", "funcblocks");

    Funcblocks.Connect(graphDB, "funcblocks");

    Funcblocks.GUI.AddSimpleControlDesc("name",          "Project name",   120);
    /*
    Funcblocks.GUI.AddSimpleControlDesc("status",        "Status",         90);
    Funcblocks.GUI.AddSimpleControlDesc("owner",         "Owner",          90);
    Funcblocks.GUI.AddSimpleControlDesc("launch_date",   "Launched",       90);
    Funcblocks.GUI.AddSimpleControlDesc("end_date",      "Finished",       90);
    Funcblocks.GUI.AddSimpleControlDesc("description",   "Description",    150);
    */

    ADD_RECORD("Test Root Funcblock 1", Funcblocks);
    ADD_RECORD("Test Root Funcblock 2", Funcblocks);
    ADD_RECORD("Test Root Funcblock 3", Funcblocks);
    ADD_RECORD("Test Root Funcblock 4", Funcblocks);

    Funcblocks.AddEntryNode(1);
    // Funcblocks.AddEntryNode(2);
    // Funcblocks.AddEntryNode(3);

    Funcblocks.StoreData();
/*
    Funcblocks.AddArrowLink("funcblocksTree", 1, Funcblocks, 2);
    Funcblocks.AddArrowLink("funcblocksTree", 2, Funcblocks, 3);
    Funcblocks.AddArrowLink("funcblocksTree", 1, Funcblocks, 4);        

    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 1);
    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 2);
    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 3);

    Funcblocks.StoreLinks();

    Projects.StoreLinks();
    */


}

// =============================================================================================
//                              JUNKYARD
// =============================================================================================

    /*
    Funcblocks.StartCreateObjdb("funcblocks");
    Funcblocks.AddFieldDesc("project",     d_link, false);
    Funcblocks.AddFieldDesc("parent",      d_link, false);
    Funcblocks.AddFieldDesc("status",        d_link, false);
    Funcblocks.AddFieldDesc("owner",         d_link, false);
    Funcblocks.AddFieldDesc("launch_date",   d_date, false);
    Funcblocks.AddFieldDesc("end_date",      d_date, false);
    Funcblocks.AddFieldDesc("name",          d_string, false);
    Funcblocks.AddFieldDesc("description",   d_string, false);
        // reorder
    Funcblocks.metaInfo.nameToOrder.ReorderFieldDescList(); // FIXME - to UpdateControls mode
        // control descs
    Funcblocks.AddControlDesc("name",          "Project name",   120);
    Funcblocks.AddControlDesc("status",        "Status",         70);
    Funcblocks.AddControlDesc("owner",         "Owner",          70);
    Funcblocks.AddControlDesc("launch_date",   "Launched",       70);
    Funcblocks.AddControlDesc("end_date",      "Finished",       70);
    Funcblocks.AddControlDesc("description",   "Description",    250);

    Funcblocks.FinishCreateObjdb();
*/
        // add reordered fields by name
/*    ins_values.clear();
    for (int k = 0; k < Funcblocks.FieldsCount(); k++)
        ins_values << QVariant();

    ins_values[Funcblocks.metaInfo.nameToOrder["project"]]     = 1;
    ins_values[Funcblocks.metaInfo.nameToOrder["parent"]]      = 0;
    ins_values[Funcblocks.metaInfo.nameToOrder["name"]]        = "Test Funcblock";
    ins_values[Funcblocks.metaInfo.nameToOrder["description"]] = "Test Funcblock Description";
    ins_values[Funcblocks.metaInfo.nameToOrder["status"]]      = 1;
    ins_values[Funcblocks.metaInfo.nameToOrder["owner"]]       = 1;
    ins_values[Funcblocks.metaInfo.nameToOrder["launch_date"]] = QDate(2010,10,1);
    ins_values[Funcblocks.metaInfo.nameToOrder["end_date"]]    = QVariant();

    Funcblocks.AddNewData(ins_values);

    ins_values[Funcblocks.metaInfo.nameToOrder["project"]]     = 1;
    ins_values[Funcblocks.metaInfo.nameToOrder["parent"]]      = 1;
    ins_values[Funcblocks.metaInfo.nameToOrder["name"]]        = "ObjDB FB";
    ins_values[Funcblocks.metaInfo.nameToOrder["description"]] = "Object Database Funcblock";
    ins_values[Funcblocks.metaInfo.nameToOrder["status"]]      = 2;
    ins_values[Funcblocks.metaInfo.nameToOrder["owner"]]       = 2;
    ins_values[Funcblocks.metaInfo.nameToOrder["launch_date"]] = QDate(2011,11,2);
    ins_values[Funcblocks.metaInfo.nameToOrder["end_date"]]    = QVariant();

    Funcblocks.AddNewData(ins_values);

    Funcblocks.StoreData();

}

    */


/*
void ProjectsForm::displayError(QAbstractSocket::SocketError socketError)
{

    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Database Error"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Database Error"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the database server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Database Error"),
                                 tr("The following error occurred: %1.")
                                 .arg(tcpSocket->errorString()));
    }

}
*/


