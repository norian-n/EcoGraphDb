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

      QMap <EgDataNodeIDtype, EgDataNode*> entryNodes;   // aka "root" nodes

      QDir dir;

      int AddEntryNode(EgDataNode& entryNode);

      int StoreEntryNodes(EgDataNodesType &egType);
      int LoadEntryNodes(EgDataNodesType& egType);
};


#endif // EGENTRYNODES_H
