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

    testDataNodes.AddNewData(addValues);
    testDataNodes.AddNewData(addValues);
    testDataNodes.AddNewData(addValues);
    testDataNodes.AddNewData(addValues);
    testDataNodes.AddNewData(addValues);
    testDataNodes.AddNewData(addValues);
    testDataNodes.AddNewData(addValues);
    testDataNodes.AddNewData(addValues);
    testDataNodes.AddNewData(addValues);

    // qDebug() << "Added" << FN ;

    testDataNodes.MarkDeletedData(2);

    // qDebug() << "Deleted" << FN ;

    addValues.clear();
    addValues << QVariant("newName") << QVariant(8);

    testDataNodes.SetModifiedData(addValues, 1);

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

    testDataNodes.MarkDeletedData(5);

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

    testDataNodes.SetModifiedData(updValues, 3);

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
    graphDB.AddDataField("status", true); // true for index

    graphDB.CommitNodeType();

    bool res = (graphDB.metaInfo->dataFields.count() == 2); // && (testDataNodes.dataNodes.count() == 8))

    testShowResult(res, FNS);

    return res;
}

bool EgGraphDatabaseTests::testEgLinksCreate()
{
    EgGraphDatabase graphDB;

    graphDB.CreateLinksMetaInfo();

    graphDB.AddLinkType("testLinkType", "nodeType1", "nodeType2");
    graphDB.AddLinkType("testTree", "test", "test");

    bool res = (true); // (graphDB.metaInfo->dataFields.count() == 2) // && (testDataNodes.dataNodes.count() == 8))

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

    bool res = (testDataNodes.metaInfo.myECoGraphDB-> linkTypes["testTree"].loadedLinks.count() == 3); // (graphDB.metaInfo->dataFields.count() == 2) // && (testDataNodes.dataNodes.count() == 8))

    testShowResult(res, FNS);

    return res;
}