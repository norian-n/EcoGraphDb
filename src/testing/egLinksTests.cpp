/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "egLinksTests.h"

void EgLinksTests::testCreateNodes()
{
    // EgGraphDatabase graphDB;
    // EgDataNodesType testDataNodes;
    QList<QVariant> addValues;

        // create nodes type
    graphDB.CreateNodeType("linksTest");

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed); // create index

    graphDB.CommitNodeType();

        // create links type
    graphDB.CreateLinksMetaInfo();

    graphDB.AddLinkType("linksTestTree", "linksTest", "linksTest");

    graphDB.Connect();

    testDataNodes.Connect(graphDB, "linksTest");

    for (int i = 0; i < 100; i++)
    {
        addValues.clear();
        addValues << QVariant("linksTestName_" + QVariant(i+1).toString()) << QVariant(2);
        testDataNodes.AddDataNode(addValues);
    }

    testDataNodes.StoreData();
}

void EgLinksTests::testCreateLinks()
{
    // EgGraphDatabase graphDB;
    // EgDataNodesType testDataNodes;

    graphDB.Connect();

    testDataNodes.Connect(graphDB, "linksTest");

    testDataNodes.LoadAllData();

    testDataNodes.AddArrowLink("linksTestTree", 1, testDataNodes, 4);
    testDataNodes.AddArrowLink("linksTestTree", 1, testDataNodes, 6);
    testDataNodes.AddArrowLink("linksTestTree", 4, testDataNodes, 7);

    testDataNodes.StoreLinks();
    testDataNodes.LoadLinks();
}


bool EgLinksTests::testLinksTree()
{

    testCreateNodes();

    testAddIndexedNodes();

    testUpdateIndexedNodes();

    testCreateLinks();

    bool res = (graphDB.linkTypes["linksTestTree"].loadedLinks.count() == 3);

    testShowResult(res, FNS);

    return res;
}

void EgLinksTests::testAddIndexedNodes()
{
    QList<QVariant> addValues;

    graphDB.Connect();

    testDataNodes.Connect(graphDB, "linksTest");

    for (int i = 0; i < 100; i++)
    {
        addValues.clear();
        addValues << QVariant("testIndexed_" + QVariant(i+1).toString()) << QVariant(i);
        testDataNodes.AddDataNode(addValues);
    }

    testDataNodes.StoreData();
}

void EgLinksTests::testUpdateIndexedNodes()
{
    // QList<QVariant> addValues;

    graphDB.Connect();

    testDataNodes.Connect(graphDB, "linksTest");

    testDataNodes.LoadAllData();

    for (int i = 0; i < 200; i++)
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

    testDataNodes.LoadData(IC("status", EQ, 2));

    bool res = (testDataNodes.DataNodesCount() == 200);

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
