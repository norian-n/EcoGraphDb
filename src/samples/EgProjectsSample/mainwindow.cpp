#include "mainwindow.h"

#include <QtDebug>
#include <QMessageBox>
#include <QTextCodec>

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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    added_row(1),
    project_form(NULL),
    funcblocks_form(NULL)
{

    ui->setupUi(this);

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

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


void MainWindow::TestLinks()
{
    // Projects.AddArrowLink("treeLinkType", 1, 2);
    // Projects.AddArrowLink("detailLinkType", Projects[1], Detail[2]);
}

void MainWindow::on_initButton_clicked()
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

MainWindow::~MainWindow()
{
            // application to terminate - no cleanup is required
}

inline void MainWindow::InitProjectForm()   // project details form setup
{
    project_form = new ProjectForm;
    project_form->main_callee = this;
    project_form->Projects = &Projects;
    project_form->Statuses = &Statuses;
    project_form->Owners = &Owners;
    project_form->initProject();
}

void MainWindow::on_addProjectButton_clicked()
{
    if (! project_form)
        InitProjectForm();

    project_form->project_id = 0;

    project_form->openProject();
    project_form->show();
}

void MainWindow::on_editProjectButton_clicked()
{
    if (! project_form)
        InitProjectForm();

    project_form->project_id = model->item(Projects.GUI.model_current_row,0)->data(data_id).toInt();

    project_form->openProject();
    project_form->show();
}

void MainWindow::on_saveButton_clicked()
{
        // get data from model
    Projects.GUI.DataFromModel(model);
        // save data
    Projects.StoreData();
}


void MainWindow::on_loadButton_clicked()
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


void MainWindow::refreshView()
{
        // detach model
    ui->tableView->setModel(NULL);
        // move data to model
    Projects.GUI.DataToModel(model);
        // attach model
    ui->tableView->setModel(model);
    Projects.GUI.SetViewWidths(ui->tableView);
}

void MainWindow::on_addButton_clicked()
{
    QList<QVariant> new_fields;
    QList<QStandardItem *> items;
        // clear and assign
    for (int k = 0; k < Projects.FieldsCount(); k++)
        new_fields << QVariant();

    /*Projects.AddNewData(new_fields);
    Projects.StoreData();

    refreshView();
    */

    Projects.GUI.AddRowOfModel(model, items);
}

void MainWindow::on_deleteButton_clicked()
{
        // delete row in dataclass and model
    Projects.GUI.DeleteRowOfModel(model);
}

void MainWindow::on_tableView_clicked(QModelIndex index)
{
        // set current data row index
    Projects.GUI.model_current_row = index.row();
}

// const QModelIndex & topLeft, const QModelIndex & bottomRight
void MainWindow::model_data_changed(const QModelIndex & topLeft, const QModelIndex & bottomRight)
{
        // update current index
    Projects.GUI.model_current_row = topLeft.row();
        // modify row in dataclass and model
    Projects.GUI.ModifyRowOfModel(model);
}


void MainWindow::FillTestData()
{

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

    Projects.GUI.AddSimpleControlDesc("name",          "Project name",   120);
    Projects.GUI.AddSimpleControlDesc("status",        "Status",         90);
    Projects.GUI.AddSimpleControlDesc("owner",         "Owner",          90);
    Projects.GUI.AddSimpleControlDesc("launch_date",   "Launched",       90);
    Projects.GUI.AddSimpleControlDesc("end_date",      "Finished",       90);
    Projects.GUI.AddSimpleControlDesc("description",   "Description",    150);

    Projects.Connect(graphDB, "projects");

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

#undef ADD_RECORD

        // funcblocks
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
    */
}


// =============================================================================================
//                              JUNKYARD
// =============================================================================================

/*
void MainWindow::GetServerData()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    out << opcode_store_metaInfo.nameToOrderesc; // datadesc.h
    tcpSocket->write(block);
    block.clear();
    out.device()->seek(0);
    // int b_written =
    out << Projects.field_count;
    tcpSocket->write(block); // TODO : process error

    if (! tcpSocket->waitForReadyRead(10000)) // up to 10 seconds
    {
        // process error
      qDebug() << "GetServerData(): tcpSocket waitForReadyRead error";
      return;
    }

    if (blockSize == 0) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;

        in >> blockSize;
    }

    if (tcpSocket->bytesAvailable() < blockSize)
    {
        qDebug() << "tcpSocket data size mismatch: " << tcpSocket->bytesAvailable() << " available, required " << blockSize;
        return;
    }

    QString test_str;
    in >> test_str;

    qDebug() << "Got string: " <<  test_str;

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
// QByteArray test_bytearray;

/*
Projects.StartCreateObjdb("projects");

Projects.metaInfo.nameToOrder
        << DFieldDesc("123", d_link)
        << DFieldDesc(QString::fromLocal8Bit("руссиш"), d_int32);

// test_bytearray << Projects.metaInfo.nameToOrder;
// test_bytearray >> Projects.metaInfo.nameToOrder;

Projects.metaInfo.nameToOrder.AddControlDesc("123", "456", ct_table_view, 0x6789);
Projects.metaInfo.nameToOrder.AddControlDesc(QString::fromLocal8Bit("руссиш"), QString::fromLocal8Bit("руссиш label"), ct_table_view, 0x1234);

// Projects.metaInfo.nameToOrder.PackControlDescs(&test_bytearray);
// Projects.metaInfo.nameToOrder.UnpackControlDescs(&test_bytearray);

Projects.FinishCreateObjdb();

*/
// Projects.Connect("projects");

// Projects.PrintFieldDesc();

// qDebug() << FN << test_bytearray.toHex();

/*
test_bytearray << Projects.metaInfo.nameToOrder;
test_bytearray >> Projects.metaInfo.nameToOrder;

Projects.PrintFieldDesc();
*/

/*
QByteArray test_bytearray;
QVariant test_variant;
QString test_string("12345");
QTextCodec *codec = QTextCodec::codecForName("Windows-1251");

test_variant = test_string;
test_bytearray = test_variant.toByteArray();
test_string.clear();
test_string = codec->toUnicode(test_bytearray);

 qDebug() << FN << test_bytearray.toHex();

DFieldDesc my_desc("test descriptor 1", d_link);

Projects.metaInfo.nameToOrder << my_desc << DFieldDesc("test descriptor 2", d_int32);
Projects.PrintFieldDesc();
*/
 // qDebug() << FN << codec->toUnicode(QVariant(QString("test string локальная строка")).toByteArray());

// Projects.PackControlDescs(&my_packed_descs, Projects.field_desc_list);
// Projects.ParseControlDescs(&my_packed_descs, Projects.field_desc_list);

/*
if (! funcblocks_form)
    funcblocks_form = new FuncBlocksForm;
// funcblocks_form->project_id = project_id;
// funcblocks_form->initFuncBlocks();
funcblocks_form->show();
*/



// ObjDBtest.TestAllLocal();

/*

void MainWindow::TestFunc()
{
    // qDebug() << Owners.DClassName;
    qDebug() << FN << "Test error message";
};

    Projects.filter_values.clear();
    Projects.filter_values << "Test 4";
    Projects.RemoteFilterID = 3;


QList<QVariant> test_values;
qint32 my_value = 355;
QString my_string("test_string");

test_values << my_value << my_string;

qDebug() << "Test type 0: " <<  test_values[0].typeName() << " " << (int) test_values[0].type();
qDebug() << "Test type 1: " <<  test_values[1].typeName() << " " << (int) test_values[1].type();
*/

//Projects.connection = &test_server; // remote
//Projects.connection = LOCAL;

// Projects.CompressData();


// Projects.GetOdbId(); // test odb_id
// Projects.StoreFieldDesc();
// Projects.LoadFieldDesc();
// Projects.RemoteAppendData();
// Projects.RemoteDeleteData();
// Projects.RemoteUpdateData();
// Projects.LoadData();


/*  connect(tcpSocket, SIGNAL(connected()), this, SLOT(GetServerData()));
  // connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(GetServerData()));
  connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
          this, SLOT(displayError(QAbstractSocket::SocketError)));
*/
  // connect(ui->addFormButton, SIGNAL(clicked()), this, SLOT(addProject()));
  // connect(ui->editFormButton, SIGNAL(clicked()), this, SLOT(editProject()));

  // QLocale::setDefault(QLocale::Russian);

  // my_database = new ObjectDatabase("test_database");


// current_row(0)
//cur_item(NULL)

// connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(on_saveButton_clicked()));
// connect(ui->loadButton, SIGNAL(clicked()), this, SLOT(on_loadButton_clicked()));

// connect(ui->tableView->commitData();

/*
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
*/

// tcpSocket->waitForReadyWrite(1000);

//block.append(14);
//tcpSocket->write(block, 1);
//tcpSocket->flush();

// qDebug() << "Written bytes 1: " <<  b_written;

/*
if (! tcpSocket->waitForBytesWritten(10000)) // up to 10 seconds
{
    // process error
  qDebug() << "GetServerData(): tcpSocket waitForBytesWritten error";
  return;
}
*/
/*

// block.clear();
// out2 << Projects.field_count;
// b_written =
// tcpSocket->write(block2); // TODO : process error

// qDebug() << "Written bytes 2: " <<  b_written;
*/


/*
    if (! tcpSocket->waitForBytesWritten(10000)) // up to 10 seconds
    {
        // process error
      qDebug() << "RemoteStoreFieldDesc(): tcpSocket waitForBytesWritten error";
      return -3;
    }
*/

// srvSocket.read(command_buffer, 1);
// command_id = command_buffer[0];

/*   if (! srvSocket.waitForReadyRead(10000)) // wait up to 10 sec
   {
     // process error
     return -2;
   }


   // srvSocket.read(db_id_buffer, 4);    // TODO: process error
   // db_id = (quint32) *db_id_buffer;  // ?? check bytes order
*/

       // get database name

       // get desc count
   /*
   if (! srvSocket.waitForReadyRead(10000)) // wait up to 10 sec
   {
     qDebug() << "Execute(): tcpSocket waitForReadyRead error";
     // process error
     return -2;
   }
   */
   // srvSocket.read(desc_count_buffer, 2); // TODO: process error: int read_count = if (read_count != 2) { // process error ; return -3;}
   // quint16 desc_count = (quint16) *desc_count_buffer;  // ?? check bytes order

// char command_buffer[1];
// char db_id_buffer[4];
// char desc_count_buffer[2];

/*
        // smoke test
    if (! tcpSocket.waitForReadyRead(10000)) // up to 10 seconds
    {
        // process error
        qDebug() << "RemoteStoreFieldDesc(): tcpSocket waitForReadyRead error";
        return -4;
    }

    if (tcpSocket.bytesAvailable() < (int)sizeof(quint16))
    {
        qDebug() << "RemoteStoreFieldDesc(): no block size bytes available";
        return -5;
    }

    in >> blockSize;

    if (tcpSocket.bytesAvailable() < blockSize)
    {
        qDebug() << "RemoteStoreFieldDesc: tcpSocket data size mismatch: " << tcpSocket.bytesAvailable() << " available, required " << blockSize;
        return -6;
    }

    QString test_str;
    in >> test_str;

    qDebug() << "Got string: " <<  test_str;
*/

/*
    if (! tcpSocket.waitForBytesWritten(10000)) // up to 10 seconds
    {
        // process error
      qDebug() << "GetServerData(): tcpSocket waitForBytesWritten error";
      return -3;
    }
*/

// tcpSocket = new QTcpSocket(this);
// tcpSocket.disconnectFromHost();

