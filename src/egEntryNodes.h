#ifndef EGENTRYNODES_H
#define EGENTRYNODES_H

#include "egDataNode.h"

class EgDataNodesType;

class EgEntryNodes   // Data Objects head API
{
public:

      QMap <EgDataNodeIDtype, EgDataNode*> entryNodes;   // aka "root" nodes

      int AddEntryNode(EgDataNode& entryNode);

      int StoreEntryNodes(EgDataNodesType &egType);
      int LoadEntryNodes(EgDataNodesType& egType);

};


#endif // EGENTRYNODES_H
