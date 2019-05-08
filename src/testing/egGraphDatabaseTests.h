/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EGGRAPHDATABASETESTS_H
#define EGGRAPHDATABASETESTS_H

#include "egExtraInfo.h"
#include "egDataNode.h"
#include "egDataNodesType.h"
#include "egGraphDatabase.h"

class EgGraphDatabaseTests
{
public:
    EgGraphDatabase graphDB;

    bool testEgDataNodeTypeMetaInfo(EgDataNodeTypeExtraInfo& egDataNodeTypeMetaInfo);

    bool testEgDataNode(EgDataNodeTypeExtraInfo& egDataNodeTypeMetaInfo);

    bool testEgDataNodesTypeBasicNodeOps(EgDataNodesType& testDataNodes);

    bool testEgDataNodesTypeDelNode(EgDataNodesType& testDataNodes);

    bool testEgDataNodesTypeUpdateNode(EgDataNodesType& testDataNodes);

    bool testEgDataNodesTypeBasicLoadStore(EgDataNodesType& testDataNodes);

    bool testEgDataNodesTypeGUIDescriptors(EgDataNodesType& testDataNodes);

    bool testEgGraphDatabaseCreate();

    bool testEgLinksCreate();

    // bool testEgLinksBasics(EgDataNodesType& testDataNodes);

    bool testEgEntryNodes(EgDataNodesType& testDataNodes);

    void testShowResult(bool res, const QString &theMessage)
    {
        if (res)
        {
            qDebug().noquote() << "PASS" << theMessage;
        }
        else
        {
            qDebug().noquote() << "FAILED" << theMessage;
        }
    }
};

#endif // EGGRAPHDATABASETESTS_H
