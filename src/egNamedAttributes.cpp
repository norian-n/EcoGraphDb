/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "egNamedAttributes.h"
#include "egDataNodesType.h"

#include <QtDebug>

EgNamedAttributes::EgNamedAttributes(EgDataNodesType* thePrimaryNodesType):
    primaryNodesType(thePrimaryNodesType)
  , namedAttributesStorage(new EgDataNodesType())
{}


EgNamedAttributes::~EgNamedAttributes()
{
    if (namedAttributesStorage)
        delete namedAttributesStorage;
}

int EgNamedAttributes::AddNamedAttribute(EgDataNodeIDtype nodeID, QString &name, QVariant &value)
{
    QList<QVariant> myData;

    myData << nodeID << name << value;

    namedAttributesStorage-> AddDataNode(myData); // AddDataNode(EgDataNode& tmpObj)

    return 0;
}


int EgNamedAttributes::UpdateNamedAttribute(EgDataNodeIDtype nodeID, QString &name, QVariant &value)
{
    QList<QVariant> myData;

    myData << nodeID << name << value;

    namedAttributesStorage-> UpdateDataNode(myData, nodeID);

    return 0;
}

int EgNamedAttributes::DeleteNamedAttribute(EgDataNodeIDtype nodeID)
{
    namedAttributesStorage->DeleteDataNode(nodeID);

    return 0;
}

int EgNamedAttributes::LoadNamedAttributes()
{
    int res = 0;

    namedAttributesStorage-> ClearData();

    namedAttributesStorage-> IndexOffsets.clear();

    // for (QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
    for (auto dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
        // namedAttributesType-> LocalFiles-> primIndexFiles-> Load_EQ(namedAttributesType->IndexOffsets, dataNodeIter.key()); // FIXME wrong index

        namedAttributesStorage-> LocalFiles-> indexFiles["nodeid"]-> Load_EQ(namedAttributesStorage->IndexOffsets, dataNodeIter.key());


    if (! namedAttributesStorage-> IndexOffsets.isEmpty())
        res = namedAttributesStorage-> LocalFiles-> LocalLoadData(namedAttributesStorage-> IndexOffsets, namedAttributesStorage-> dataNodes);


    return res;
}
