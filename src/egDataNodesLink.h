/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EGDATANODESLINK_H
#define EGDATANODESLINK_H

#include "egCore.h"
#include "indexes/egIndexesFiles.h"

namespace EgDataNodesLinkNamespace
{
    const char* const egLinkTypesFileName("egLinkTypesMetaInfo");
    const char* const egLinkFileNamePostfix("_egLinkType");
}

class EgGraphDatabase;
class EgDataNodesType;

class EgDataNodesLinkType
{
public:
    bool isConnected = false;

    EgGraphDatabase* egDatabase;        // backlink to db
    EgDataNodesType* linksStorage;      // data nodes for links info

    QString linkName;

    QString firstTypeName;
    QString secondTypeName;

    EgDataNodesLinkType(): egDatabase(nullptr), linksStorage(nullptr) {}        // required by debug, don't use it

    EgDataNodesLinkType(EgGraphDatabase* theDatabase);
    ~EgDataNodesLinkType();

    int AddLink (EgDataNodeIDtype leftNodeID, EgDataNodeIDtype rightNodeID);
    int DeleteLink (EgDataNodeIDtype linkNodeID); // FIXME store linkNodeID or search

    int LoadLinks();
    int StoreLinks();
    int ResolveLinks(EgDataNodesType& firstType, EgDataNodesType& secondType);  // move loaded links to data nodes if loaded

    int LoadLinkedNodes(EgDataNodeIDtype fromNodeID);

};


#endif // EGDATANODESLINK_H
