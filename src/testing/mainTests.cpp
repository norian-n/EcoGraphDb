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

void cleanUpFiles()
{
    QDir dir("");
    QStringList nameFilters;

    nameFilters  << "*.odf" << "*.odx" << "*.dat" << "*.ddt" << "*.dln"; // << "test*.*";

        // get filtered filenames
    QStringList ent = dir.entryList(nameFilters);
    // qDebug() << dir.entryList();

        // remove files
    foreach (const QString& str, ent)
        dir.remove(str);
}

int main() // int argc, char *argv[])
{
    bool res = true;

    EgGraphDatabaseTests egDbTests;

    EgGraphDatabase graphDB;
    EgDataNodesType testEgDataNodesType;

    cleanUpFiles();

        // indexes standalone

    EgIndexFiles<qint32> testIndexFiles("IndexesTests");
    EgIndexesTests<qint32> indexTests;


    // res = res && indexTests.testEgDataIndexAddFirst(testIndexFiles);


    res = res && indexTests.testEgDataInsertIndexes(testIndexFiles);

    // res = res && indexTests.testEgDataDeleteIndex(testIndexFiles);

    res = res && indexTests.testSelectOperations(testIndexFiles);

    return 0; // FIXME STUB



        // meta info standalone
    // EgDataNodeTypeMetaInfo egDataNodeTypeMetaInfo("MetaTest");

    // res = res && egDbTests.testEgDataNodeTypeMetaInfo(egDataNodeTypeMetaInfo);

    // res = res && egDbTests.testEgDataNode(egDataNodeTypeMetaInfo);

        // baisc functionality

    res = res && egDbTests.testEgGraphDatabaseCreate();

    res = res && egDbTests.testEgLinksCreate();

    testEgDataNodesType.Connect(graphDB, "test");

    res = res && egDbTests.testEgDataNodesTypeBasicNodeOps(testEgDataNodesType);

    res = res && egDbTests.testEgDataNodesTypeBasicLoadStore(testEgDataNodesType);

    res = res && egDbTests.testEgDataNodesTypeDelNode(testEgDataNodesType);

    res = res && egDbTests.testEgDataNodesTypeUpdateNode(testEgDataNodesType);

    res = res && egDbTests.testEgDataNodesTypeGUIDescriptors(testEgDataNodesType);

    // return 0; // FIXME STUB

    res = res && egDbTests.testEgLinksBasics(testEgDataNodesType);

    res = res && egDbTests.testEgEntryNodes(testEgDataNodesType);


    if (res)
        qDebug() << "\nAll tests PASSED\n";
    else
        qDebug() << "\nSome tests FAILED\n";

    // getchar(); // for standalone launch

}
