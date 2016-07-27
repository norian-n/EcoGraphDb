/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "funcblocks.h"
#include <QtDebug>

FuncBlocksForm::FuncBlocksForm(QWidget *parent)
    : QWidget(parent)
  ,  ui(new Ui::FuncBlocksForm)
  , model (NULL)
{
    ui->setupUi(this);
        // connect buttons
    connect(ui->addTopBlockButton, SIGNAL(clicked()),this, SLOT(addTopBlock()));
    connect(ui->addButton, SIGNAL(clicked()),this, SLOT(addSubBlock()));
    connect(ui->editButton, SIGNAL(clicked()),this, SLOT(editSubBlock()));
    connect(ui->deleteButton, SIGNAL(clicked()),this, SLOT(deleteSubBlock()));
    connect(ui->cancelButton, SIGNAL(clicked()),this, SLOT(cancelExit()));
    connect(ui->okButton, SIGNAL(clicked()),this, SLOT(okExit()));
    connect(ui->compButton, SIGNAL(clicked()),this, SLOT(openCompForm()));
/*
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(QStringList(QString("Func block")) << QString("Description"));
    ui->treeWidget->setColumnWidth(0,300);

    ui->treeWidget->clear();
*/

    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    // ui->treeView->setExpandsOnDoubleClick(true);
/*
    Funcblocks.Connect(graphDB, "funcblocks");
    // Funcblocks.parent_field = QString("parent");
    Funcblocks.LoadAllData();

    Funcblocks.LoadLinkedData("projects_funcblocks", project_id);

    Funcblocks.LoadLinks();
    */

/*
        // ref obj classes
    Statuses.Connect(graphDB, "status_data");
    Statuses.LoadData();
    Owners.Connect(graphDB, "owner_data");
    Owners.LoadData();

    */

    /*
        // datalinks:  status - create and set autolink
    FuncblockStatusLink = new LinkData(&Funcblocks, &Statuses); // create link
    FuncblockStatusLink->SetAutoLinkFields(Funcblocks.FD["status"], Statuses.FD["status"]); // set autolink
        // datalinks: owner - create and set autolink
    FuncblockOwnerLink = new LinkData(&Funcblocks, &Owners); // create link
    FuncblockOwnerLink->SetAutoLinkFields(Funcblocks.FD["owner"],Owners.FD["login"]); // set autolink
    */


    /*
    QList<LinkData*> autolinks_list;
    autolinks_list << FuncblockStatusLink << FuncblockOwnerLink;
    */
    /*
    Funcblocks.GUI.AddAutoSubstitute("status", Statuses, "status");
    Funcblocks.GUI.AddAutoSubstitute("owner",  Owners,   "login");


    */

    /*
    model = new QStandardItemModel(0, Funcblocks.ModelFieldsCount());

    Funcblocks.GUI.DataToModelTree(model, "funcblocksTree");
        // attach model
    ui->treeView->setModel(model);

    // Funcblocks.GUI.SetViewWidths(ui->treeView);

    ui->treeView->expandAll();
    */

    graphDB.Connect();

    Funcblocks.Connect(graphDB, "funcblocks");
    Funcblocks.getGUIinfo();

    Projects.Connect(graphDB, "projects");

    if (! model)
        model = new QStandardItemModel(0, Funcblocks.ModelFieldsCount());
}


void FuncBlocksForm::loadFuncblocks()
{
    // IC RootCond = IC(Projects, "odb_pit", EQ, project_id);

    Projects.index_tree-> clear();
    Projects.index_tree-> AddNode(NULL, true, false, EQ, "odb_pit", project_id);  // FIXME STUB

    // Funcblocks.Connect(graphDB, "funcblocks");
    // Projects.Connect(graphDB, "projects");

    Projects.LoadData();


    // Funcblocks.LoadAllData(); // STUB

    // Projects.LoadLinks();

    Funcblocks.LoadLinkedData("projects_funcblocks", project_id);
    Funcblocks.LoadLink("funcblocksTree");

    Funcblocks.myLinkTypes["funcblocksTree"]-> ResolveLinks(); // FIXME STUB

/*
    qDebug() << "project ID = " << project_id << FN;
    qDebug() << "projects count = " << Projects.dataNodes.count() << FN;
    qDebug() << "funcblocks count = " << Funcblocks.dataNodes.count() << FN;
    */



    Funcblocks.GUI.DataToModelTree(model, "funcblocksTree");

        // attach model
    ui->treeView->setModel(model);

    // Funcblocks.GUI.SetViewWidths(ui->treeView);

    ui->treeView->expandAll();
}

void FuncBlocksForm::addSubBlock()
{

}

void FuncBlocksForm::addTopBlock()
{

}

void FuncBlocksForm::editSubBlock()
{

}

void FuncBlocksForm::deleteSubBlock()
{

}

void FuncBlocksForm::openCompForm()
{

}

void FuncBlocksForm::okExit()
{
    // Funcblocks.LoadData();
    // Funcblocks.PrintObjData();
        // exit
    close();
}

void FuncBlocksForm::cancelExit()
{
        // exit
    close();
}

// =============================================================================================
//                              JUNKYARD
// =============================================================================================

/*
void FuncBlocksForm::FillTestData()
{

    QList<QVariant> ins_values;

        // funcblocks
    Funcblocks.AddFieldDesc("project",       "Project link",        d_link);
    Funcblocks.AddFieldDesc("parent",        "Parent link",         d_link);
    // Funcblocks.AddFieldDesc("status",        "Funcblock status",        d_link);
    // Funcblocks.AddFieldDesc("owner",         "Funcblock owner",         d_link);
    Funcblocks.AddFieldDesc("launch_date",   "Launch date",           d_date);
    Funcblocks.AddFieldDesc("end_date",      "End_date",              d_date);
    Funcblocks.AddFieldDesc("name",          "Funcblock name",          d_string);
    Funcblocks.AddFieldDesc("description",   "Funcblock description",   d_string);

    Funcblocks.StoreFieldDesc();

#define ADD_RECORD(values,dataclass) ins_values.clear(); ins_values << values; dataclass.AddNewData(ins_values);

        // Funcblocks
    ADD_RECORD(QVariant(1) << QVariant(0) << QVariant(QDate(2010,10,1)) << QVariant(0) <<
               QVariant("Top level funcblock") << QVariant("Test Funcblock Description"), Funcblocks);
    ADD_RECORD(QVariant(1) << QVariant(1) << QVariant(QDate(2011,11,2)) << QVariant(0) <<
               QVariant("Detail funcblock 1") << QVariant("Test Funcblock Description"), Funcblocks);
    ADD_RECORD(QVariant(1) << QVariant(1) << QVariant(QDate(2012,12,3)) << QVariant(0) <<
               QVariant("Detail funcblock 2") << QVariant("Test Funcblock Description"), Funcblocks);

#undef ADD_RECORD

    Funcblocks.PrintObjData();
    Funcblocks.StoreData();    
}
*/

// qDebug() << FN << "data indexes : " << Funcblocks.dobj_map.size();
// qDebug() << FN << "parent indexes : " << Funcblocks.parent_map.size();

