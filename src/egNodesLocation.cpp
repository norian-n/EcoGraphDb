/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "egNodesLocation.h"
#include "egDataNodesType.h"

#include <QtDebug>

EgDataNodesLocation::EgDataNodesLocation(EgDataNodesType* thePrimaryNodesType):
    primaryNodesType(thePrimaryNodesType)
  , locationNodesType(new EgDataNodesType())
{}


EgDataNodesLocation::~EgDataNodesLocation()
{
    if (locationNodesType)
        delete locationNodesType;
}

int EgDataNodesLocation::AddLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID)
{
        return locationNodesType-> AddHardLinked(myData, nodeID);
}


int EgDataNodesLocation::UpdateLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID)
{
        return locationNodesType-> UpdateDataNode(myData, nodeID);
}

int EgDataNodesLocation::DeleteLocationOfNode(EgDataNodeIDtype nodeID)
{
        return locationNodesType->DeleteDataNode(nodeID);
}

int EgDataNodesLocation::LoadLocationsData()
{
    int res = 0;

    locationNodesType-> ClearData();

    locationNodesType-> IndexOffsets.clear();

    // for (QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
    for (auto dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
        locationNodesType-> LocalFiles-> primIndexFiles-> Load_EQ(locationNodesType->IndexOffsets, dataNodeIter.key());


    if (! locationNodesType-> IndexOffsets.isEmpty())
        res = locationNodesType-> LocalFiles-> LocalLoadData(locationNodesType-> IndexOffsets, locationNodesType-> dataNodes);


    return res;
}
