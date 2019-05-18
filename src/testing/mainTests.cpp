/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include <QtDebug>
#include <QFile>
#include <QDir>

// #include <stdio.h>

#include "egGraphDatabaseTests.h"
#include "egIndexesTests.h"
#include "egLinksTests.h"
#include "egLocationTests.h"
#include "egNamedAttributesTests.h"


void cleanUpFiles()
{
    QDir dir("egdb"); // QDir::currentPath()

    // qDebug()  << "dir name: " << dir.dirName() << FN;
    // qDebug()  << "dir current path: " << dir.currentPath() << FN;

    dir.removeRecursively();

    return;

    /*
    QStringList nameFilters;

    nameFilters  << "*.odf" << "*.odx" << "*.dat" << "*.ddt" << "*.dln" << "*.ent"; // << "test*.*";

        // get filtered filenames
    QStringList ent = dir.entryList(nameFilters);
    // qDebug() << dir.entryList();

        // remove files
    foreach (const QString& str, ent)
        dir.remove(str);

    if (! dir.exists("egdb"))
    {
        qDebug() << "no egdb dir found at " << dir.path() << FN;
        return;
    }

    qDebug()  << "dir name: " << dir.dirName() << FN;
    qDebug()  << "dir current path: " << dir.currentPath() << FN;

    if (! dir.setCurrent("egdb"))
    {
        qDebug()  << "can't find the egdb dir " << FN;
    }


    qDebug()  << "dir name 2: " << dir.dirName() << FN;
    qDebug()  << "dir current path 2: " << dir.currentPath() << FN;

        // get filtered filenames
    ent = dir.entryList(nameFilters);
    qDebug() << dir.entryList();

        // remove files
    foreach (const QString& str, ent)
        dir.remove(str);

    dir.setCurrent("..");
    */
}

int main() // int argc, char *argv[])
{
    bool res = true;
    bool tmpRes = false;

    EgGraphDatabaseTests egDbTests;

    EgGraphDatabase graphDB;
    EgDataNodesType testEgDataNodesType;

    cleanUpFiles();

/*
    EgDataNodesType testType2;
    testType2.Connect(graphDB, "serverTestNodeType");

    testType2.LoadData(IC("status", EQ, 8));

    // testType2.LoadAllNodes();

    qDebug() << "nodes count: " << testType2.dataNodes.count();

    for (auto iter: testType2.dataNodes.values())
        qDebug() << iter.dataFields;
*/

    // return 0; // FIXME STUB

        // meta info standalone
    // EgDataNodeTypeMetaInfo egDataNodeTypeMetaInfo("MetaTest");

    // res = res && egDbTests.testEgDataNodeTypeMetaInfo(egDataNodeTypeMetaInfo);

    // res = res && egDbTests.testEgDataNode(egDataNodeTypeMetaInfo);

        // baisc functionality

    // tmpRes = egDbTests.testEgLinksCreate(); // special links module created
    // res = res && tmpRes;


        // indexes standalone tests

    QDir mydir = QDir::current();

    // qDebug() << "Path: " << mydir.path() << FN;

    if (! mydir.exists("egdb"))
        mydir.mkdir("egdb");

    EgIndexFiles<qint32> testIndexFiles("IndexesTests");
    EgIndexesTests<qint32> indexTests;

    tmpRes = indexTests.testEgDataIndexAddFirst(testIndexFiles);
    res = res && tmpRes;

    tmpRes = indexTests.testEgDataIndexDelFirst(testIndexFiles);
    res = res && tmpRes;

    tmpRes = indexTests.testEgDataInsertIndexes(testIndexFiles);
    res = res && tmpRes;

    // return 0; // FIXME STUB

    tmpRes = indexTests.testSelectOperations(testIndexFiles);
    res = res && tmpRes;

    // tmpRes = indexTests.testEgDataDeleteAllIndexes(testIndexFiles);

    // data nodes type

    // graphDB.Connect();

    tmpRes = egDbTests.testEgGraphDatabaseCreate();
    res = res && tmpRes;

    tmpRes = ! (bool) testEgDataNodesType.Connect(graphDB, "test");
    res = res && tmpRes;

    // testEgDataNodesType.metaInfo.PrintMetaInfo();

    tmpRes = egDbTests.testEgDataNodesTypeBasicNodeOps(testEgDataNodesType);
    res = res && tmpRes;

    tmpRes = egDbTests.testEgDataNodesTypeBasicLoadStore(testEgDataNodesType);
    res = res && tmpRes;

    // return 0; // FIXME STUB

    tmpRes = egDbTests.testEgDataNodesTypeDelNode(testEgDataNodesType);
    res = res && tmpRes;

    tmpRes = egDbTests.testEgDataNodesTypeUpdateNode(testEgDataNodesType);
    res = res && tmpRes;

    // res = res && egDbTests.testEgDataNodesTypeGUIDescriptors(testEgDataNodesType);

    // return 0; // FIXME STUB

    // tmpRes = egDbTests.testEgLinksBasics(testEgDataNodesType);
    // res = res && tmpRes;

    tmpRes = egDbTests.testEgEntryNodes(testEgDataNodesType);
    res = res && tmpRes;

    // return 0; // FIXME STUB

    EgLinksTests linksTests;

    tmpRes = linksTests.testLinksTree();
    res = res && tmpRes;

    // return 0; // FIXME STUB

    EgLocationTests locTests;

    tmpRes = locTests.testCreateLocations();
    res = res && tmpRes;

    tmpRes = locTests.testAddLocations();
    res = res && tmpRes;

    tmpRes = locTests.testLoadLocationsData();
    res = res && tmpRes;

    EgNamedAttributesTests attrTests;

    tmpRes = attrTests.testCreateAttributes();
    res = res && tmpRes;

    tmpRes = attrTests.testAddAttributes();
    res = res && tmpRes;

    tmpRes = attrTests.testLoadAttributes();
    res = res && tmpRes;

    // qDebug() << res;

    if (res)
        qDebug() << "\nAll tests PASSED\n";
    else
        qDebug() << "\nSome tests FAILED\n";

    // getchar(); // for standalone launch

}
