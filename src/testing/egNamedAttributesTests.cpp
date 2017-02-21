/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "egNamedAttributesTests.h"

bool EgNamedAttributesTests::testCreateAttributes()
{
    EgNodeTypeSettings typeSettings;

    graphDB.CreateEgDb();

        // test all options

    /*
    typeSettings.useLinks = true;
    typeSettings.useGUIsettings = true;
    typeSettings.useEntryNodes = true;
    typeSettings.useLocation = true;
    typeSettings.useNamedAttributes = true;
    */

    typeSettings.useLocation = true;

    graphDB.CreateNodeType("locations", typeSettings);

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed); // create index

    graphDB.AddLocationField("imageType");

    graphDB.CommitNodeType();

    bool res = (graphDB.locationMetaInfo-> dataFields.count() == 3); // x,y,imageType

    testShowResult(res, FNS);

    return res;
}

bool EgNamedAttributesTests::testAddAttributes()
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

    testDataNodes.LoadAllNodes();

    bool res = (testDataNodes.locations->locationStorage-> DataNodesCount() == 100);

    testShowResult(res, FNS);

    return res;
}

bool EgNamedAttributesTests::testLoadAttributes()
{
    graphDB.Connect();

    testDataNodes.Connect(graphDB, "locations");

    testDataNodes.LoadData("status", EQ, 2);

    // testDataNodes.LoadLocationsData();

    bool res = (testDataNodes.locations->locationStorage-> DataNodesCount() == 1);

    testShowResult(res, FNS);

    return res;
}


