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

    graphDB.CreateNodeType("test");

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", true); // true for index

    graphDB.CommitNodeType();

    graphDB.CreateLinksMetaInfo();

    // graphDB.AddLinkType("testLinkType", "nodeType1", "nodeType2");
    graphDB.AddLinkType("testTree", "test", "test");


    graphDB.Connect();

    testDataNodes.Connect(graphDB, "test");

    for (int i = 0; i < 10; i++)
    {
        addValues.clear();
        addValues << QVariant("testName" + QString(i+1)) << QVariant(1);
        testDataNodes.AddNewData(addValues);
    }

    testDataNodes.StoreData();
}

void EgLinksTests::testCreateLinks()
{
    // EgGraphDatabase graphDB;
    // EgDataNodesType testDataNodes;

    graphDB.Connect();

    testDataNodes.Connect(graphDB, "test");

    testDataNodes.LoadAllData();

    testDataNodes.AddArrowLink("testTree", 1, testDataNodes, 4);
    testDataNodes.AddArrowLink("testTree", 1, testDataNodes, 6);
    testDataNodes.AddArrowLink("testTree", 4, testDataNodes, 7);

    testDataNodes.StoreLinks();
    testDataNodes.LoadLinks();
}


bool EgLinksTests::testFuncblocksCase()
{

    testCreateNodes();

    testCreateLinks();

    bool res = (graphDB.linkTypes["testTree"].loadedLinks.count() == 3);

    testShowResult(res, FNS);

    return res;
}
