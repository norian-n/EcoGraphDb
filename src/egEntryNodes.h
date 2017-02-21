/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EGENTRYNODES_H
#define EGENTRYNODES_H

#include <QDir>

#include "egDataNode.h"

class EgDataNodesType;

class EgEntryNodes   // Data Objects head API
{
public:

    EgDataNodesType* nodesType = nullptr;           // entry nodes (aka "root" nodes) of this type
    EgDataNodesType* entryStorage = nullptr;        // entry nodes info storage

    QList <EgDataNodeIDtype> entryNodesList;   // fast search by ID ??

    EgEntryNodes() = delete;                        //  only bound to node type

    EgEntryNodes(EgDataNodesType* theNodesType);
    ~EgEntryNodes();

    int AddEntryNode (EgDataNodeIDtype nodeID);
    int DeleteEntryNode (EgDataNodeIDtype nodeID);

    int LoadEntryNodes();
    int StoreEntryNodes();

};


#endif // EGENTRYNODES_H
