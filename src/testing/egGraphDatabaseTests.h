/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EGGRAPHDATABASETESTS_H
#define EGGRAPHDATABASETESTS_H

#include "egMetaInfo.h"
#include "egDataNode.h"
#include "egDataNodesType.h"
#include "egGraphDatabase.h"

class EgGraphDatabaseTests
{
public:
    bool testEgDataNodeTypeMetaInfo(EgDataNodeTypeMetaInfo& egDataNodeTypeMetaInfo);

    bool testEgDataNode(EgDataNodeTypeMetaInfo& egDataNodeTypeMetaInfo);

    bool testEgDataNodesTypeBasicNodeOps(EgDataNodesType& testDataNodes);

    bool testEgDataNodesTypeDelNode(EgDataNodesType& testDataNodes);

    bool testEgDataNodesTypeUpdateNode(EgDataNodesType& testDataNodes);

    bool testEgDataNodesTypeBasicLoadStore(EgDataNodesType& testDataNodes);

    bool testEgDataNodesTypeGUIDescriptors(EgDataNodesType& testDataNodes);

    bool testEgGraphDatabaseCreate();

    bool testEgLinksCreate();

    bool testEgLinksBasics(EgDataNodesType& testDataNodes);

    bool testEgEntryNodes(EgDataNodesType& testDataNodes);

    void testShowResult(bool res, const QString &theMessage)
    {
        if (res)
        {
            qDebug() << "PASSED" << theMessage;
        }
        else
        {
            qDebug() << "FAILED" << theMessage;
        }
    }
};

#endif // EGGRAPHDATABASETESTS_H
