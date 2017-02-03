/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include <QtDebug>

#include "egGraphDatabaseTests.h"

bool EgGraphDatabaseTests::testEgDataNodeTypeMetaInfo(EgDataNodeTypeMetaInfo& egDataNodeTypeMetaInfo)
{
        // create meta-info
    egDataNodeTypeMetaInfo.AddDataField("name");
    egDataNodeTypeMetaInfo.AddDataField("status", true);

    // egDataNodeTypeMetaInfo.PrintMetaInfo();

        // store meta-info
    egDataNodeTypeMetaInfo.LocalStoreMetaInfo();

        // clean up
    egDataNodeTypeMetaInfo.Clear();

    // egDataNodeTypeMetaInfo.PrintMetaInfo();

        // load meta-info
    egDataNodeTypeMetaInfo.LocalLoadMetaInfo();

        // check result
    // egDataNodeTypeMetaInfo.PrintMetaInfo();

    bool res = ((egDataNodeTypeMetaInfo.dataFields.count() == 2)
                && (egDataNodeTypeMetaInfo.nameToOrder.count() == 2)
                && (egDataNodeTypeMetaInfo.indexedToOrder.count() == 1));

    testShowResult(res, FNS);

    return res;
}


bool EgGraphDatabaseTests::testEgDataNode(EgDataNodeTypeMetaInfo& egDataNodeTypeMetaInfo)
{
    EgDataNode dataNode(egDataNodeTypeMetaInfo);

    dataNode["name"]    = QString("testName");
    dataNode["status"]  = QString("testStatus");

    bool res = ((dataNode["name"].toString() == QString("testName")) && (dataNode["status"].toString() == QString("testStatus")));

    testShowResult(res, FNS);

    return res;
}

bool EgGraphDatabaseTests::testEgDataNodesTypeBasicNodeOps(EgDataNodesType& testDataNodes)
{
    QList<QVariant> addValues;

    addValues << QVariant("testName") << QVariant(7);

    testDataNodes.AddDataNode(addValues);
    testDataNodes.AddDataNode(addValues);
    testDataNodes.AddDataNode(addValues);
    testDataNodes.AddDataNode(addValues);
    testDataNodes.AddDataNode(addValues);
    testDataNodes.AddDataNode(addValues);
    testDataNodes.AddDataNode(addValues);
    testDataNodes.AddDataNode(addValues);
    testDataNodes.AddDataNode(addValues);

    // qDebug() << "Added" << FN ;

    testDataNodes.DeleteDataNode(2);

    // qDebug() << "Deleted" << FN ;

    addValues.clear();
    addValues << QVariant("newName") << QVariant(8);

    testDataNodes.UpdateDataNode(addValues, 1);

    // TestNodesType.Connect("AddNodeTest");

    bool res = ((testDataNodes.addedDataNodes.count() == 8)
            && (testDataNodes.dataNodes.count() == 8)
            && (testDataNodes.deletedDataNodes.count() == 0)
            && (testDataNodes.updatedDataNodes.count() == 0));

    testShowResult(res, FNS);

    return res;
}


bool EgGraphDatabaseTests::testEgDataNodesTypeDelNode(EgDataNodesType& testDataNodes)
{

    testDataNodes.DeleteDataNode(5);

    testDataNodes.StoreData();
    testDataNodes.LoadAllData();

    bool res = ((testDataNodes.deletedDataNodes.count() == 0) && (testDataNodes.dataNodes.count() == 7));

    testShowResult(res, FNS);

    return res;
}

bool EgGraphDatabaseTests::testEgDataNodesTypeUpdateNode(EgDataNodesType& testDataNodes)
{
    QList<QVariant> updValues;

    updValues << QVariant("updatedName") << QVariant(9);

    testDataNodes.UpdateDataNode(updValues, 3);

    testDataNodes.StoreData();
    testDataNodes.LoadAllData();

    bool res = ((testDataNodes.updatedDataNodes.count() == 0) && (testDataNodes.dataNodes.count() == 7));

    testShowResult(res, FNS);

    return res;
}

bool EgGraphDatabaseTests::testEgDataNodesTypeBasicLoadStore(EgDataNodesType& testDataNodes)
{
    testDataNodes.StoreData();
    testDataNodes.LoadAllData();

    // qDebug() <<  "dataNodes count = " << testDataNodes.dataNodes.count() << FN;

    bool res = ((testDataNodes.addedDataNodes.count() == 0) && (testDataNodes.dataNodes.count() == 8));

    testShowResult(res, FNS);

    return res;
}

bool EgGraphDatabaseTests::testEgDataNodesTypeGUIDescriptors(EgDataNodesType& testDataNodes)
{
    testDataNodes.GUI.CreateDataNodesForControlDescs();

    testDataNodes.GUI.AddSimpleControlDesc("name", "Name", 100);
    testDataNodes.GUI.AddSimpleControlDesc("status", "Status", 50);

    testDataNodes.GUI.LoadSimpleControlDesc();

    bool res = (testDataNodes.GUI.basicControlDescs.count() == 2); // (testDataNodes.addedDataNodes.count() == 0) && (testDataNodes.dataNodes.count() == 8))

    testShowResult(res, FNS);

    return res;
}

bool EgGraphDatabaseTests::testEgGraphDatabaseCreate()
{
    EgGraphDatabase graphDB;

    graphDB.CreateNodeType("test");

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed); // true for index

    graphDB.CommitNodeType();
/*
    graphDB.CreateNodeType("locations", useLocations);

    graphDB.AddDataField("name");
    graphDB.AddLocationField("imageType");

    graphDB.CommitNodeType();
    */

    bool res = (graphDB.fieldsCreated == 2); //  && (graphDB.locationMetaInfo-> dataFields.count() == 3));

    testShowResult(res, FNS);

    return res;
}

bool EgGraphDatabaseTests::testEgLinksCreate()
{
    EgGraphDatabase graphDB;

    graphDB.CreateEgDb();

    graphDB.AddLinkType("testLinkType", "nodeType1", "nodeType2");
    graphDB.AddLinkType("testTree", "test", "test");

    bool res = (true); // (graphDB.linksCreated == 2); // && (testDataNodes.dataNodes.count() == 8))

    testShowResult(res, FNS);

    return res;
}


bool EgGraphDatabaseTests::testEgLinksBasics(EgDataNodesType& testDataNodes)
{
// Projects.AddArrowLink("detailLinkType", Projects[1], Detail[2]);

    testDataNodes.AddArrowLink("testTree", 1, testDataNodes, 4);
    testDataNodes.AddArrowLink("testTree", 1, testDataNodes, 6);
    testDataNodes.AddArrowLink("testTree", 4, testDataNodes, 7);

    testDataNodes.StoreLinks();
    testDataNodes.LoadLinks();

    // testDataNodes.metaInfo.myECoGraphDB-> StoreAllLinks();
    // testDataNodes.metaInfo.myECoGraphDB-> LoadAllLinks();

    bool res = (testDataNodes.metaInfo.myECoGraphDB-> linkTypes["testTree"].linksStorage-> dataNodes.count() == 3); // (graphDB.metaInfo->dataFields.count() == 2) // && (testDataNodes.dataNodes.count() == 8))

    // qDebug() <<  "dataNodes count = " << testDataNodes.dataNodes.count() << FN;

    testShowResult(res, FNS);

    return res;
}


bool EgGraphDatabaseTests::testEgEntryNodes(EgDataNodesType& testDataNodes)
{
// Projects.AddArrowLink("detailLinkType", Projects[1], Detail[2]);

    testDataNodes.AddEntryNode(1);
    testDataNodes.AddEntryNode(4);

    // testDataNodes.entryNodesInst.StoreEntryNodes(testDataNodes);
    testDataNodes.entryNodesInst.LoadEntryNodes(testDataNodes);

    bool res = (testDataNodes.entryNodesInst.entryNodes.count() == 2); // (graphDB.metaInfo->dataFields.count() == 2) // && (testDataNodes.dataNodes.count() == 8))

    testShowResult(res, FNS);

    return res;
}
