/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "egLocationTests.h"

bool EgLocationTests::testCreateLocations()
{
    graphDB.CreateNodeType("locations", useLocations);

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed); // create index

    graphDB.AddLocationField("imageType"); // x,y are default fields

    graphDB.CommitNodeType();

    bool res = (graphDB.locationFieldsCreated == 3); // x,y,imageType

    testShowResult(res, FNS);

    return res;
}

bool EgLocationTests::testAddLocations()
{
    EgDataNodeIDtype newID;
    QList<QVariant> addValues;
    QList<QVariant> locValues;

    graphDB.Connect();

    testDataNodes.Connect(graphDB, "locations");

    for (int i = 0; i < 100; i++)
    {
        addValues.clear();
        addValues << QVariant("locationsTestName_" + QVariant(i+1).toString()) << i;

        testDataNodes.AddDataNode(addValues, newID);

        locValues.clear();
        locValues << i << i+1 << 1;

        testDataNodes.locations-> AddLocationOfNode(locValues, newID);
    }

    testDataNodes.StoreData();

    testDataNodes.LoadAllData();

    bool res = (testDataNodes.locations->locationNodesType-> DataNodesCount() == 100);

    testShowResult(res, FNS);

    return res;
}

bool EgLocationTests::testLoadLocationsData()
{
    graphDB.Connect();

    testDataNodes.Connect(graphDB, "locations");

    testDataNodes.LoadData("status", EQ, 2);

    // testDataNodes.LoadLocationsData();

    bool res = (testDataNodes.locations->locationNodesType-> DataNodesCount() == 1);

    testShowResult(res, FNS);

    return res;
}


