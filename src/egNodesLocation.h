/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
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
