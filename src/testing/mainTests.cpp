/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
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

int main() // int argc, char *argv[])
{
    bool res = true;
    bool tmpRes;

    EgGraphDatabaseTests egDbTests;

    EgGraphDatabase graphDB;
    EgDataNodesType testEgDataNodesType;

    cleanUpFiles();

    // return 0; // FIXME STUB

        // indexes standalone

    // EgIndexFiles<qint32> testIndexFiles("IndexesTests");
    // EgIndexesTests<qint32> indexTests;

    // return 0; // FIXME STUB

    // res = res && indexTests.testEgDataIndexAddFirst(testIndexFiles);
    // res = res && indexTests.testEgDataIndexDelFirst(testIndexFiles);

    // res = res && indexTests.testEgDataInsertIndexes(testIndexFiles);
    // res = res && indexTests.testEgDataDeleteAllIndexes(testIndexFiles);


    // return 0; // FIXME STUB

    // res = res && indexTests.testEgDataDeleteIndex(testIndexFiles);

    // res = res && indexTests.testSelectOperations(testIndexFiles);

    // return 0; // FIXME STUB



        // meta info standalone
    // EgDataNodeTypeMetaInfo egDataNodeTypeMetaInfo("MetaTest");

    // res = res && egDbTests.testEgDataNodeTypeMetaInfo(egDataNodeTypeMetaInfo);

    // res = res && egDbTests.testEgDataNode(egDataNodeTypeMetaInfo);

        // baisc functionality

    tmpRes = egDbTests.testEgGraphDatabaseCreate();
    res = res && tmpRes;

    // tmpRes = egDbTests.testEgLinksCreate(); // special links module created
    // res = res && tmpRes;

    // graphDB.Connect();

    testEgDataNodesType.Connect(graphDB, "test");
    // testEgDataNodesType.metaInfo.PrintMetaInfo();

    // return 0; // FIXME STUB

    tmpRes = egDbTests.testEgDataNodesTypeBasicNodeOps(testEgDataNodesType);
    res = res && tmpRes;

    tmpRes = egDbTests.testEgDataNodesTypeBasicLoadStore(testEgDataNodesType);
    res = res && tmpRes;

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

    EgLocationTests locTests;

    tmpRes = locTests.testCreateLocations();
    res = res && tmpRes;

    tmpRes = locTests.testAddLocations();
    res = res && tmpRes;

    tmpRes = locTests.testLoadLocationsData();
    res = res && tmpRes;

    EgNamedAttributesTests attrTests;

    attrTests.testCreateAttributes();

    tmpRes = attrTests.testAddAttributes();
    res = res && tmpRes;

    tmpRes = attrTests.testLoadAttributes();
    res = res && tmpRes;

    if (res)
        qDebug() << "\nAll tests PASSED\n";
    else
        qDebug() << "\nSome tests FAILED\n";

    // getchar(); // for standalone launch

}
