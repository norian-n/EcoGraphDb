/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "mainwindow.h"

#include <QtDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    projectsForm(NULL)
{
    ui->setupUi(this);
}

void MainWindow::on_initButton_clicked()
{
/*
    Statuses.connection = &test_server;
    Owners.connection = &test_server;
    Projects.connection = &test_server;
    Funcblocks.connection = &test_server;
*/
    int res = QMessageBox::question(this,
                tr("Warning"),
                tr("All existing data will be deleted and sample data created"));

    if (res == QMessageBox::Yes)
    {
        FillTestData();
        close();
    }
}

MainWindow::~MainWindow()
{
            // FIXME close all
    // if (projectsForm)
    //    projectsForm-> close();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (projectsForm)
        projectsForm-> close();

    // event->accept();
}

void MainWindow::on_projectsButton_clicked()
{
    if (! projectsForm)
        projectsForm = new ProjectsForm;

    projectsForm-> show();
}

void MainWindow::FillTestData()
{
    QDir dir("");

    QStringList nameFilters;

    nameFilters  << "*.odf" << "*.odx" << "*.dat" << "*.ddt" << "*.dln" << "*.ent";

    if (dir.exists("egdb"))
    {
        dir.setCurrent("egdb");

            // get filtered filenames
        QStringList ent = dir.entryList(nameFilters);
        // qDebug() << dir.entryList();

            // remove files
        foreach (const QString& str, ent)
            dir.remove(str);

        dir.setCurrent("..");
    }

    // return; // FIXME STUB

#define ADD_RECORD(values,dataNodesType) ins_values.clear(); ins_values << values; dataNodesType.AddDataNode(ins_values);

    QList<QVariant> ins_values;
    EgNodeTypeSettings typeSettings;

    graphDB.CreateEgDb();

    graphDB.CreateNodeType("statuses", typeSettings);

    graphDB.AddDataField("status");

    graphDB.CommitNodeType();

    Statuses.Connect(graphDB, "statuses");

    ADD_RECORD("Planned", Statuses);
    ADD_RECORD("Running", Statuses);
    ADD_RECORD("Hold", Statuses);
    ADD_RECORD("Complete", Statuses);
    ADD_RECORD("Drop", Statuses);

    Statuses.StoreData();

    graphDB.CreateNodeType("owners", typeSettings);

    graphDB.AddDataField("login");
    graphDB.AddDataField("name");
    graphDB.AddDataField("mail");

    graphDB.CommitNodeType();

    Owners.Connect(graphDB, "owners");

    ADD_RECORD("defuser" << "Default User" << "defuser@no.mail", Owners);
    ADD_RECORD("norian" << "Norian" << "norian@no.mail", Owners);

    Owners.StoreData();

    typeSettings.useLinks = true;
    typeSettings.useGUIsettings = true;
    // typeSettings.useEntryNodes = true;

        // projects
    graphDB.CreateNodeType("projects", typeSettings);

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed);
    graphDB.AddDataField("description");
    graphDB.AddDataField("owner", isIndexed);
    graphDB.AddDataField("launch_date");
    graphDB.AddDataField("end_date");

    graphDB.CommitNodeType();

    Projects.Connect(graphDB, "projects");

    Projects.GUI-> AddSimpleControlDesc("name",          "Project name",   120);
    Projects.GUI-> AddSimpleControlDesc("status",        "Status",         90);
    Projects.GUI-> AddSimpleControlDesc("owner",         "Owner",          90);
    Projects.GUI-> AddSimpleControlDesc("launch_date",   "Launched",       90);
    Projects.GUI-> AddSimpleControlDesc("end_date",      "Finished",       90);
    Projects.GUI-> AddSimpleControlDesc("description",   "Description",    150);

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

    Projects.AddDataNode(ins_values);

    ins_values[Projects.FIELD("name")]        = "Test Project 1";
    ins_values[Projects.FIELD("description")] = "Test Project Description";
    ins_values[Projects.FIELD("status")]      = 1;
    ins_values[Projects.FIELD("owner")]       = 1;
    ins_values[Projects.FIELD("launch_date")] = QDate(2011,11,2);
    ins_values[Projects.FIELD("end_date")]    = QVariant();

    Projects.AddDataNode(ins_values);

    ins_values[Projects.FIELD("name")]        = "Test Project 2";
    ins_values[Projects.FIELD("description")] = "Test Project Description";
    ins_values[Projects.FIELD("status")]      = 2;
    ins_values[Projects.FIELD("owner")]       = 2;
    ins_values[Projects.FIELD("launch_date")] = QDate(2011,11,2);
    ins_values[Projects.FIELD("end_date")]    = QVariant();

    Projects.AddDataNode(ins_values);

    ins_values[Projects.FIELD("name")]        = "Test Project 3";
    ins_values[Projects.FIELD("description")] = "Test Project Description";
    ins_values[Projects.FIELD("status")]      = 3;
    ins_values[Projects.FIELD("owner")]       = 1;
    ins_values[Projects.FIELD("launch_date")] = QDate(2011,11,2);
    ins_values[Projects.FIELD("end_date")]    = QVariant();

    Projects.AddDataNode(ins_values);

    ins_values[Projects.FIELD("name")]        = "Test Project 4";
    ins_values[Projects.FIELD("description")] = "Test Project Description";
    ins_values[Projects.FIELD("status")]      = 4;
    ins_values[Projects.FIELD("owner")]       = 2;
    ins_values[Projects.FIELD("launch_date")] = QDate(2011,11,2);
    ins_values[Projects.FIELD("end_date")]    = QVariant();

    Projects.AddDataNode(ins_values);

    // **************************************

    Projects.StoreData();


        // funcblocks
    typeSettings.useLinks = true;
    typeSettings.useGUIsettings = true;
    typeSettings.useEntryNodes = true;

    graphDB.CreateNodeType("funcblocks", typeSettings);

    graphDB.AddDataField("name");
/*
    graphDB.AddDataField("status", IsIndexed);
    graphDB.AddDataField("description");
    graphDB.AddDataField("owner", IsIndexed);
    graphDB.AddDataField("launch_date");
    graphDB.AddDataField("end_date");
    */

    graphDB.CommitNodeType();

        // add link types
    graphDB.AddLinkType("funcblocksTree", "funcblocks", "funcblocks");
    graphDB.AddLinkType("projects_funcblocks", "projects", "funcblocks");

    graphDB.Connect();

    Funcblocks.Connect(graphDB, "funcblocks");

    Funcblocks.GUI-> AddSimpleControlDesc("name",          "Func block name",   120);
    /*
    Funcblocks.GUI-> AddSimpleControlDesc("status",        "Status",         90);
    Funcblocks.GUI-> AddSimpleControlDesc("owner",         "Owner",          90);
    Funcblocks.GUI-> AddSimpleControlDesc("launch_date",   "Launched",       90);
    Funcblocks.GUI-> AddSimpleControlDesc("end_date",      "Finished",       90);
    Funcblocks.GUI-> AddSimpleControlDesc("description",   "Description",    150);
    */

    ADD_RECORD("Test Root Funcblock 1", Funcblocks);
    ADD_RECORD("Test Funcblock 2", Funcblocks);
    ADD_RECORD("Test Funcblock 3", Funcblocks);
    ADD_RECORD("Test Root Funcblock 4", Funcblocks);

    Funcblocks.AddEntryNode(1);
    Funcblocks.AddEntryNode(4);
    // Funcblocks.AddEntryNode(3);

    Funcblocks.StoreData();

        // set links
    Funcblocks.AddArrowLink("funcblocksTree", 1, Funcblocks, 2);
    Funcblocks.AddArrowLink("funcblocksTree", 2, Funcblocks, 3);
    // Funcblocks.AddArrowLink("funcblocksTree", 1, Funcblocks, 4);

    // Projects.LoadAllData();

    Projects.getMyLinkTypes();

    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 1);
    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 2);
    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 3);
    Projects.AddArrowLink("projects_funcblocks", 1, Funcblocks, 4);

    Funcblocks.StoreAllLinks();

    // Projects.StoreLinks();

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

    Funcblocks.AddDataNode(ins_values);

    ins_values[Funcblocks.metaInfo.nameToOrder["project"]]     = 1;
    ins_values[Funcblocks.metaInfo.nameToOrder["parent"]]      = 1;
    ins_values[Funcblocks.metaInfo.nameToOrder["name"]]        = "ObjDB FB";
    ins_values[Funcblocks.metaInfo.nameToOrder["description"]] = "Object Database Funcblock";
    ins_values[Funcblocks.metaInfo.nameToOrder["status"]]      = 2;
    ins_values[Funcblocks.metaInfo.nameToOrder["owner"]]       = 2;
    ins_values[Funcblocks.metaInfo.nameToOrder["launch_date"]] = QDate(2011,11,2);
    ins_values[Funcblocks.metaInfo.nameToOrder["end_date"]]    = QVariant();

    Funcblocks.AddDataNode(ins_values);

    Funcblocks.StoreData();

}

    */


/*
void MainWindow::displayError(QAbstractSocket::SocketError socketError)
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




