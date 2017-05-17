#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    // scene.setBackgroundBrush(Qt::blue);
}

MainWindow::~MainWindow()
{
    delete ui;

    if (sceneForm)
        delete sceneForm;
}


void MainWindow::on_initButton_clicked()
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

    EgNodeTypeSettings typeSettings;

    typeSettings.useLocation = true;
    typeSettings.useLinks = true;

    graphDB.CreateNodeType("locations", typeSettings);

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed); // create index

    graphDB.AddLocationField("imageType"); // x,y are default fields

    graphDB.CommitNodeType();

    graphDB.CreateEgDb();

    graphDB.AddLinkType("linktype", "locations", "locations");

// #define ADD_RECORD(values,dataNodesType) ins_values.clear(); ins_values << values; dataNodesType.AddDataNode(ins_values);
// QList<QVariant> ins_values;

    nodes.Connect(graphDB, "locations");

    // ADD_RECORD("one" << 1, nodes);
    // ADD_RECORD("two" << 2, nodes);

    EgDataNodeIDtype newID;
    QList<QVariant> addValues;
    QList<QVariant> locValues;

    addValues.clear();
    addValues << "one" << 1;

    nodes.AddDataNode(addValues, newID);

    locValues.clear();
    locValues << 150 << 150 << 2;

    nodes.AddLocation(locValues, newID);

    addValues.clear();
    addValues << "two" << 2;

    nodes.AddDataNode(addValues, newID);

    locValues.clear();
    locValues << 70 << 70 << 2;

    nodes.AddLocation(locValues, newID);

    addValues.clear();
    addValues << "three" << 3;

    nodes.AddDataNode(addValues, newID);

    locValues.clear();
    locValues << 0 << 0 << 2;

    nodes.AddLocation(locValues, newID);

    addValues.clear();
    addValues << "four" << 4;

    nodes.AddDataNode(addValues, newID);

    locValues.clear();
    locValues << 0 << 100 << 2;

    nodes.AddLocation(locValues, newID);

    nodes.StoreData();

    close();
}

void MainWindow::on_sceneButton_clicked()
{
    if (! sceneForm)
        sceneForm = new GraphSceneForm();

    sceneForm->show();

    close();
}
