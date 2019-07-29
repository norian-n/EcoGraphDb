/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egLocationTests.h"

bool EgLocationTests::testCreateLocations()
{
    bool res {false};

    if(res) // DUMMY
        EG_LOG_STUB << FN;

    EgNodeTypeSettings typeSettings;

    typeSettings.useLocation = true;

    graphDB.CreateNodeType("locations", typeSettings);

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed); // create index

    graphDB.AddLocationField("imageType"); // x,y are default fields

    graphDB.CommitNodeType();

    res = (graphDB.locationFieldsCreated == 3); // x,y,imageType

    testShowResult(res, FNS);

    return res;
}

bool EgLocationTests::testAddLocations()
{
    EgDataNodeIdType newID;
    QList<QVariant> addValues;
    QList<QVariant> locValues;

    // graphDB.Connect();

    testDataNodes.Connect(graphDB, "locations");

    for (int i = 0; i < 100; i++)
    {
        addValues.clear();
        addValues << QVariant("locationsTestName_" + QVariant(i+1).toString()) << i;

        testDataNodes.AddDataNode(addValues, newID);

        locValues.clear();
        locValues << i << i+1 << 1;

        testDataNodes.locations-> AddLocation(locValues, newID);
    }

    testDataNodes.StoreData();

    testDataNodes.AutoLoadAllData();

    testDataNodes.locations-> GetLocation(locValues, 77);

    // qDebug()  << "GetLocation() of node 77 returned " << locValues << FN;

    bool res = ((testDataNodes.locations->locationStorage-> DataNodesCount() == 100) && (locValues[1].toInt() == 77));

    testShowResult(res, FNS);

    return res;
}

bool EgLocationTests::testLoadLocationsData()
{
    // graphDB.Connect();

    testDataNodes.Connect(graphDB, "locations");

    testDataNodes.LoadDataByIndexes("status", EQ, 2);

    // testDataNodes.LoadLocationsData();

    bool res = (testDataNodes.locations->locationStorage-> DataNodesCount() == 1);

    testShowResult(res, FNS);

    return res;
}


