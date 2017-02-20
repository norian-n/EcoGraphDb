/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef NODES_LOCATION_H
#define NODES_LOCATION_H

#include "egCore.h"

class EgDataNodesType;

class EgDataNodesLocation
{
public:

    EgDataNodesType* primaryNodesType;      // backlink
    EgDataNodesType* locationStorage;

    int AddLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID);
    int UpdateLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID);
    int DeleteLocationOfNode(EgDataNodeIDtype nodeID);

    int LoadLocationsData();

    EgDataNodesLocation() = delete;         // backlink has to be set

    EgDataNodesLocation(EgDataNodesType* thePrimaryNodesType);

    ~EgDataNodesLocation();

};


#endif // NODES_LOCATION_H
