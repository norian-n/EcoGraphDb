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

int EgDataNodesLocation::AddLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID)
{
        return locationStorage-> AddHardLinked(myData, nodeID);
}


int EgDataNodesLocation::UpdateLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID)
{
        return locationStorage-> UpdateDataNode(myData, nodeID);
}

int EgDataNodesLocation::DeleteLocationOfNode(EgDataNodeIDtype nodeID)
{
        return locationStorage->DeleteDataNode(nodeID);
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
