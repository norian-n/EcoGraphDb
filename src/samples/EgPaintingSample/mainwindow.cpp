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
    cleanUpFiles();

        // create node type

    EgNodeTypeSettings typeSettings;

    typeSettings.useLocation = true;
    typeSettings.useLinks = true;

    graphDB.CreateNodeType("locations", typeSettings);

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed); // create index

    graphDB.AddLocationField("imageType"); // x,y are default fields

    graphDB.CommitNodeType();

        // create link type

    graphDB.AddLinkType("linktype", "locations", "locations");

// QList<QVariant> ins_values;
// #define ADD_RECORD(values,dataNodesType) ins_values.clear(); ins_values << values; dataNodesType.AddDataNode(ins_values);

    // ADD_RECORD("one" << 1, nodes);
    // ADD_RECORD("two" << 2, nodes);

        // add sample data with location coordinates

    nodes.Connect(graphDB, "locations");

    EgDataNodeIDtype newID;
    QList<QVariant> addValues;
    QList<QVariant> locValues;

    addValues.clear();
    addValues << "one" << 1;

    nodes.AddDataNode(addValues, newID);

    locValues.clear();
    locValues << 150 << 150 << 1;

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
    locValues << 0 << 0 << 3;

    nodes.AddLocation(locValues, newID);

    addValues.clear();
    addValues << "four" << 4;

    nodes.AddDataNode(addValues, newID);

    locValues.clear();
    locValues << 0 << 100 << 4;

    nodes.AddLocation(locValues, newID);

        // add links (graph edges)

    nodes.AddArrowLink("linktype", 1, nodes, 2);
    nodes.AddArrowLink("linktype", 2, nodes, 3);
    nodes.AddArrowLink("linktype", 2, nodes, 4);

    nodes.StoreData();
    nodes.StoreAllLinks();

    AddImages();

    close();
}

void MainWindow::cleanUpFiles()
{
    QDir dir("");

    QStringList nameFilters;

    nameFilters  << "*.odf" << "*.odx" << "*.dat" << "*.ddt" << "*.dln" << "*.ent"; // << "test*.*";

        // get filtered filenames
    QStringList ent = dir.entryList(nameFilters);
    // qDebug() << dir.entryList();

        // remove files
    foreach (const QString& str, ent)
        dir.remove(str);

    if (! dir.exists("egdb"))
        return;

    dir.setCurrent("egdb");

        // get filtered filenames
    ent = dir.entryList(nameFilters);
    // qDebug() << dir.entryList();

        // remove files
    foreach (const QString& str, ent)
        dir.remove(str);

    dir.setCurrent("..");
}

void MainWindow::AddImages()
{
    // create node type

    EgNodeTypeSettings typeSettings;

    graphDB.CreateNodeType("images", typeSettings);

    graphDB.AddDataField("name");
    graphDB.AddDataField("bytearray");

    graphDB.CommitNodeType();

    images.Connect(graphDB, "images");

    EgDataNodeIDtype newID;
    QList<QVariant> addValues;

        // get all files
    QDir dir;

    QStringList nameFilters;

    nameFilters  << "*.png" << "*.jpg" << "*.bmp" << "*.gif" << "*.ico"; // << "test*.*";

        // get filtered filenames
    QStringList ent = dir.entryList(nameFilters);
    qDebug() << ent;

    foreach (const QString& fileName, ent)
    {
        // QString fileName("test.png");

        QPixmap pixMap(40,40);

        bool loadRes = pixMap.load(fileName);

        if (loadRes)
        {
            pixMap = pixMap.scaled(40,40);

            QByteArray imageData;
            QDataStream dataStream(&imageData, QIODevice::WriteOnly);

            dataStream << pixMap;

            addValues.clear();
            addValues << fileName << imageData;

            images.AddDataNode(addValues, newID);

            qDebug() << fileName;
        }
    }

    images.StoreData();

    /*
    QPixmap pixmap = *child->pixmap();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << pixmap << QPoint(event->pos() - child->pos());*/
}

void MainWindow::on_sceneButton_clicked()
{
    if (! sceneForm)
        sceneForm = new GraphSceneForm();

    sceneForm->show();

    close();
}
