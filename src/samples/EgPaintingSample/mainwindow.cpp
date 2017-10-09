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

        // clear egDb

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

        // add links (graph edges)

    nodes.AddArrowLink("linktype", 1, nodes, 2);
    nodes.AddArrowLink("linktype", 2, nodes, 3);
    nodes.AddArrowLink("linktype", 2, nodes, 4);

    nodes.StoreData();
    nodes.StoreAllLinks();

    AddImages();

    close();
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

    QPixmap pixMap(40,40);

    QString fileName("test.png");

    bool loadRes = pixMap.load(fileName);

    if (loadRes)
        pixMap = pixMap.scaled(40,40);
    else
        pixMap.fill(Qt::green);

    QByteArray imageData;
    QDataStream dataStream(&imageData, QIODevice::WriteOnly);

    dataStream << pixMap;

    addValues.clear();
    addValues << "name" << imageData;

    images.AddDataNode(addValues, newID);

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
