/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2020 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

// #ifndef EGENTRYNODES_H
// #define EGENTRYNODES_H

#pragma once

#include "egDataNode.h"

class EgDataNodesType;

class EgEntryNodes   // Data Objects head API
{
public:

    EgDataNodesType* nodesType = nullptr;           // entry nodes (aka "root" nodes) of this type
    EgDataNodesType* entryStorage = nullptr;        // entry nodes info storage

    QList <EgDataNodeIdType> entryNodesList;   // fast search by ID ??

    EgEntryNodes() = delete;                        //  only bound to node type

    EgEntryNodes(EgDataNodesType* theNodesType);
    ~EgEntryNodes();

    int AddEntryNode (EgDataNodeIdType nodeID);
    int DeleteEntryNode (EgDataNodeIdType nodeID);

    int LoadEntryNodes();
    int StoreEntryNodes();

};

// #endif // EGENTRYNODES_H
