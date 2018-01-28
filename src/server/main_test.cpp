/*
 * egDb server sample console application
 *
 * Copyright (c) 2017 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include <QDebug>
#include <QThread>

#include "../EgDataClient.h"
#include "../egMetaInfo.h"
#include "../egDataNodesType.h"
#include "../egGraphDatabase.h"

int main() // int argc, char *argv[])
{
    int res = true;
/*
    EgServerConnection* serverConnect = new EgServerConnection;
    EgDataNodeTypeMetaInfo metaInfo;

    // test commands

    // serverConnect.SendCommand(0xcd, QString("testNodeType1"));
    // serverConnect.SendCommand(0xab, QString("nodeType2"));

    // create metainfo

    metaInfo.typeName = { "serverTestNodeType" };
    metaInfo.serverConnection = serverConnect;

    metaInfo.AddDataField("name");
    metaInfo.AddDataField("status", true);

    // save metainfo to server

    res = ! metaInfo.ServerStoreMetaInfo();
    // res = res && ! metaInfo.ServerLoadMetaInfo();

    // metaInfo.PrintMetaInfo();
    */

    EgGraphDatabase graphDB;
    EgNodeTypeSettings typeSettings;

    // graphDB.CreateEgDb();

        // test all options

    /*
    typeSettings.useLinks = true;
    typeSettings.useGUIsettings = true;
    typeSettings.useEntryNodes = true;
    typeSettings.useLocation = true;
    typeSettings.useNamedAttributes = true;
    */

    // typeSettings.useEntryNodes = true;

    qDebug() << "Tests launched ";

    graphDB.CreateNodeType("serverTestNodeType", typeSettings, "localhost");

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed); // true for index

    graphDB.CommitNodeType();

    QThread::msleep(100);

    EgDataNodesType testType;

    testType.Connect(graphDB, "serverTestNodeType", "localhost");

    QList<QVariant> addValues;

    addValues << QVariant("testName") << QVariant(7);

    EgDataNodeIDtype newID;

    testType.AddDataNode(addValues, newID);
    testType.AddDataNode(addValues);

    testType.StoreData();

    addValues.clear();
    addValues << QVariant("newName") << QVariant(8);

    testType.UpdateDataNode(addValues, newID);

    QThread::msleep(100);
    testType.StoreData();

    testType.DeleteDataNode(newID+1);

    QThread::msleep(100);
    testType.StoreData();

    QThread::msleep(100);

        // load test
    EgDataNodesType testType2;
    testType2.Connect(graphDB, "serverTestNodeType", "localhost");

    // QThread::msleep(200);
    testType2.LoadData(IC("status", EQ, 8));

    //testType2.LoadAllNodes();

    // QThread::msleep(200);
    // qDebug() << "nodes count: " << testType2.dataNodes.count();

    // for (auto iter: testType2.dataNodes.values())
    //    qDebug() << iter.dataFields;

    res = (testType2.dataNodes.count() == 1);

    if (res)
        qDebug() << "\nAll tests PASSED\n";
    else
        qDebug() << "\nSome tests FAILED\n";
}
