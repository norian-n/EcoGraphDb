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
  , locationStorage(new EgDataNodesType())
{}


EgDataNodesLocation::~EgDataNodesLocation()
{
    if (locationStorage)
        delete locationStorage;
}

int EgDataNodesLocation::AddLocation(QList<QVariant>& locationData, EgDataNodeIDtype nodeID)
{
        return locationStorage-> AddHardLinked(locationData, nodeID);
}


int EgDataNodesLocation::UpdateLocation(QList<QVariant>& locationData, EgDataNodeIDtype nodeID)
{
        return locationStorage-> UpdateDataNode(locationData, nodeID);
}

int EgDataNodesLocation::DeleteLocation(EgDataNodeIDtype nodeID)
{
        return locationStorage->DeleteDataNode(nodeID);
}

int EgDataNodesLocation::GetLocation(QList<QVariant>& locationData, EgDataNodeIDtype nodeID)
{
    locationData.clear();

        // check if node exists
    if(! locationStorage-> dataNodes.contains(nodeID))
        return 1;

    locationData = locationStorage-> dataNodes[nodeID].dataFields;

    return 0;
}

int EgDataNodesLocation::LoadLocationsData()
{
    int res = 0;

    locationStorage-> ClearData();

    locationStorage-> IndexOffsets.clear();

    // for (QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
    for (auto dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
        locationStorage-> LocalFiles-> primIndexFiles-> Load_EQ(locationStorage->IndexOffsets, dataNodeIter.key());


    if (! locationStorage-> IndexOffsets.isEmpty())
        res = locationStorage-> LocalFiles-> LocalLoadData(locationStorage-> IndexOffsets, locationStorage-> dataNodes);


    return res;
}
