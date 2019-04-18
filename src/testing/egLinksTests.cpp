/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egLinksTests.h"

void EgLinksTests::testCreateNodes()
{
    // EgGraphDatabase graphDB;
    // EgDataNodesType testDataNodes;
    QList<QVariant> addValues;

    EgNodeTypeSettings typeSettings;

    typeSettings.useLinks = true;


    // graphDB.CreateNodeType("test");

    graphDB.CreateNodeType("linksTest", typeSettings);

        // create nodes type
    // graphDB.CreateNodeType("linksTest");

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed); // create index

    graphDB.CommitNodeType();

        // create links type
    // graphDB.CreateEgDb();

    graphDB.AddLinkType("linksTestTree", "linksTest", "linksTest");

    // graphDB.Connect();

    testDataNodes.Connect(graphDB, "linksTest");

    for (int i = 0; i < 100; i++)
    {
        addValues.clear();
        addValues << QVariant("linksTestName_" + QVariant(i+1).toString()) << QVariant(1);
        testDataNodes.AddDataNode(addValues);
    }

    testDataNodes.StoreData();
}

void EgLinksTests::testCreateLinks()
{
    // EgGraphDatabase graphDB;
    // EgDataNodesType testDataNodes;

    // graphDB.Connect();

    testDataNodes.Connect(graphDB, "linksTest");

    testTree.Connect(graphDB, "linksTestTree", testDataNodes, testDataNodes);

    testDataNodes.LoadAllDataNodes();

    /*
    testDataNodes.AddArrowLink("linksTestTree", 1, testDataNodes, 4);
    testDataNodes.AddArrowLink("linksTestTree", 1, testDataNodes, 6);
    testDataNodes.AddArrowLink("linksTestTree", 4, testDataNodes, 7);
    */

    testTree.AddArrowLink(1, 4);
    testTree.AddArrowLink(1, 6);
    testTree.AddArrowLink(4, 7);

    testTree.StoreLinks();

    // testDataNodes.StoreAllLinks();
    // testDataNodes.LoadAllLinks();
}


bool EgLinksTests::testLinksTree()
{

    testCreateNodes();

    testAddIndexedNodes();

    testUpdateIndexedNodes();

    testCreateLinks();

    bool res = (testTree.linksStorage-> dataNodes.count() == 3);

    testShowResult(res, FNS);

    return res;
}

void EgLinksTests::testAddIndexedNodes()
{
    QList<QVariant> addValues;

    // graphDB.Connect();

    testDataNodes.Connect(graphDB, "linksTest");

    for (int i = 0; i < 10; i++)
    {
        addValues.clear();
        addValues << QVariant("testIndexed_" + QVariant(i+1).toString()) << QVariant(3);
        testDataNodes.AddDataNode(addValues);
    }

    testDataNodes.StoreData();
}

void EgLinksTests::testUpdateIndexedNodes()
{
    // QList<QVariant> addValues;

    // graphDB.Connect();

    testDataNodes.Connect(graphDB, "linksTest");

    testDataNodes.LoadAllDataNodes();

    for (int i = 0; i < 110; i++)
    {
        // update node index

        testDataNodes[i+1]["status"] = 2;
        testDataNodes.UpdateDataNode(i+1);
    }

    testDataNodes.StoreData();

    /*
    testDataNodes.LoadData(IC("status", LT, 2));

    qDebug() << "testDataNodes.DataNodesCount LT = " << (int) testDataNodes.DataNodesCount() << FN ;

    testDataNodes.LoadData(IC("status", GT, 2));

    qDebug() << "testDataNodes.DataNodesCount GT = " << (int) testDataNodes.DataNodesCount() << FN ;

    testDataNodes.LoadData(IC("status", LE, 2));

    qDebug() << "testDataNodes.DataNodesCount LE = " << (int) testDataNodes.DataNodesCount() << FN ;

    testDataNodes.LoadData(IC("status", GE, 2));

    qDebug() << "testDataNodes.DataNodesCount GE = " << (int) testDataNodes.DataNodesCount() << FN ;
    */

    testDataNodes.LoadDataByIndexes(IC("status", EQ, 2));

    // qDebug() << testDataNodes.DataNodesCount() << FN;

    bool res = (testDataNodes.DataNodesCount() == 110);

    testShowResult(res, FNS);

/*
    testDataNodes.MarkDeletedData(17);

    testDataNodes.StoreData();

    addValues.clear();
    addValues << QVariant("testIndexed_" + QVariant(17).toString()) << QVariant(2);
    testDataNodes.AddDataNode(addValues);

    testDataNodes.StoreData();
    */
}
