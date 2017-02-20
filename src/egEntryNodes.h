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

    QMap <EgDataNodeIDtype, EgDataNode*> entryNodesMap;   // ??

    QDir dir;

    EgEntryNodes() {}   // FIXME delete

    EgEntryNodes(EgDataNodesType* theNodesType);
    ~EgEntryNodes();

    int AddEntryNode (EgDataNodeIDtype nodeID);
    int DeleteEntryNode (EgDataNodeIDtype nodeID);

    int LoadEntryNodes();
    int StoreEntryNodes();


    int AddEntryNode(EgDataNodesType &egType, EgDataNode& entryNode);

    // int StoreEntryNodes(EgDataNodesType &egType);
    int LoadEntryNodes(EgDataNodesType& egType);
};


#endif // EGENTRYNODES_H
