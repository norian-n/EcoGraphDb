/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "egNamedAttributes.h"

#include <QtDebug>

EgNamedAttributes::EgNamedAttributes(EgDataNodesType* thePrimaryNodesType):
    primaryNodesType(thePrimaryNodesType)
  , namedAttributesType(new EgDataNodesType())
{}


EgNamedAttributes::~EgNamedAttributes()
{
    if (namedAttributesType)
        delete namedAttributesType;
}

int EgNamedAttributes::AddNamedAttribute(EgDataNodeIDtype nodeID, QString &name, QVariant &value)
{
    QList<QVariant> myData;

    myData << nodeID << name << value;

    namedAttributesType-> AddDataNode(myData); // AddDataNode(EgDataNode& tmpObj)

    return 0;
}


int EgNamedAttributes::UpdateNamedAttribute(EgDataNodeIDtype nodeID, QString &name, QVariant &value)
{
    QList<QVariant> myData;

    myData << nodeID << name << value;

    namedAttributesType-> UpdateDataNode(myData, nodeID);

    return 0;
}

int EgNamedAttributes::DeleteNamedAttribute(EgDataNodeIDtype nodeID)
{
    namedAttributesType->DeleteDataNode(nodeID);

    return 0;
}

int EgNamedAttributes::LoadNamedAttributes()
{
    int res = 0;

    namedAttributesType-> ClearData();

    namedAttributesType-> IndexOffsets.clear();

    // for (QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
    for (auto dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
        // namedAttributesType-> LocalFiles-> primIndexFiles-> Load_EQ(namedAttributesType->IndexOffsets, dataNodeIter.key()); // FIXME wrong index

        namedAttributesType-> LocalFiles-> indexFiles["nodeid"]-> Load_EQ(namedAttributesType->IndexOffsets, dataNodeIter.key());


    if (! namedAttributesType-> IndexOffsets.isEmpty())
        res = namedAttributesType-> LocalFiles-> LocalLoadData(namedAttributesType-> IndexOffsets, namedAttributesType-> dataNodes);


    return res;
}
