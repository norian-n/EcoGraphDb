/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egNamedAttributesTests.h"

bool EgNamedAttributesTests::testCreateAttributes()
{
    EgNodeTypeSettings typeSettings;

    // graphDB.CreateEgDb();

        // test all options

    /*
    typeSettings.useLinks = true;
    typeSettings.useGUIsettings = true;
    typeSettings.useEntryNodes = true;
    typeSettings.useLocation = true;
    typeSettings.useNamedAttributes = true;
    */

    typeSettings.useNamedAttributes = true;

    graphDB.CreateNodeType("attributes", typeSettings);

    graphDB.AddDataField("name");
    graphDB.AddDataField("status", isIndexed);

    graphDB.CommitNodeType();

    // bool res = (graphDB.attributesMetaInfo-> dataFields.count() == 3); // x,y,imageType

    // testShowResult(res, FNS);

    return 0;
}

bool EgNamedAttributesTests::testAddAttributes()
{
    EgDataNodeIdType newID;
    QList<QVariant> addValues;
    QString nameString;

    // graphDB.Connect();

    testDataNodes.Connect(graphDB, "attributes");

    for (int i = 0; i < 10; i++)
    {
        addValues.clear();
        addValues << QVariant("testAddAttributes_" + QVariant(i+1).toString()) << i;

        testDataNodes.AddDataNode(addValues, newID);

        nameString = "test_" + QVariant(i+1).toString();
        testDataNodes.namedAttributes-> AddNamedAttribute(newID, nameString, QVariant(i));
    }

    testDataNodes.StoreData();

    testDataNodes.AutoLoadAllData();

    bool res = (testDataNodes.namedAttributes->namedAttributesStorage-> DataNodesCount() == 10);

    testShowResult(res, FNS);

    return res;
}

bool EgNamedAttributesTests::testLoadAttributes()
{
    // graphDB.Connect();

    testDataNodes.Connect(graphDB, "attributes");

    testDataNodes.LoadDataByIndexes("status", EQ, 2);

    // testDataNodes.LoadLocationsData();

    bool res = (testDataNodes.namedAttributes->namedAttributesStorage-> DataNodesCount() == 1);

    testShowResult(res, FNS);

    return res;
}


