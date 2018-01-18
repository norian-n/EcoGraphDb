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


/*
    EgNodeTypeSettings typeSettings;

    graphDB.CreateNodeType("serverTest", typeSettings);

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed); // true for index

    graphDB.CommitNodeType();
    */

    EgGraphDatabase graphDB;
    EgDataNodesType testType;

    testType.Connect(graphDB, "serverTestNodeType", "localhost");

    QList<QVariant> addValues;

    addValues << QVariant("testName") << QVariant(7);

    EgDataNodeIDtype newID;

    testType.AddDataNode(addValues, newID);
    testType.AddDataNode(addValues);

    QThread::msleep(200);
    testType.StoreData();

    addValues.clear();
    addValues << QVariant("newName") << QVariant(8);

    testType.UpdateDataNode(addValues, newID);

    QThread::msleep(200);
    testType.StoreData();

    testType.DeleteDataNode(newID+1);

    QThread::msleep(200);
    testType.StoreData();

    QThread::msleep(500);

        // load test
    EgDataNodesType testType2;
    testType2.Connect(graphDB, "serverTestNodeType", "localhost");

    testType2.LoadData(IC("status", EQ, 7));

    // testType2.LoadAllNodes();

    qDebug() << "nodes count: " << testType2.dataNodes.count();


    if (res)
        qDebug() << "\nAll tests PASSED\n";
    else
        qDebug() << "\nSome tests FAILED\n";
}
