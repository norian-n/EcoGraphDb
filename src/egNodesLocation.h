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

    int AddLocation(QList<QVariant>& locationData, EgDataNodeIDtype nodeID);
    int UpdateLocation(QList<QVariant>& locationData, EgDataNodeIDtype nodeID);
    int DeleteLocation(EgDataNodeIDtype nodeID);

    int GetLocation(QList<QVariant>& locationData, EgDataNodeIDtype nodeID);

    int LoadLocationsData();

    EgDataNodesLocation() = delete;         // backlink has to be set

    EgDataNodesLocation(EgDataNodesType* thePrimaryNodesType);

    ~EgDataNodesLocation();

};


#endif // NODES_LOCATION_H
