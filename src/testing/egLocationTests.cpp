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
    graphDB.AddLocationField("imageType");

    graphDB.CommitNodeType();

    bool res = (graphDB.locationMetaInfo-> dataFields.count() == 3);

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
        addValues << QVariant("locationsTestName_" + QVariant(i+1).toString());

        testDataNodes.AddDataNode(addValues, newID);

        locValues.clear();
        locValues << i << i+1 << 1;

        testDataNodes.AddLocationOfNode(locValues, newID);
    }

    testDataNodes.StoreData();

    bool res = false; // (graphDB.linkTypes["linksTestTree"].loadedLinks.count() == 3);

    testShowResult(res, FNS);

    return res;
}
