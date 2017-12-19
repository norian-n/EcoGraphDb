/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
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

#ifdef EG_LIB_BUILD     // library build switch, define it in project or egCore.h

#include "ecographdb_global.h"

class ECOGRAPHDBSHARED_EXPORT EgDataNodesLinkType

#else                   // not a library build

class EgDataNodesLinkType

#endif

// class EgDataNodesLinkType
{
public:
    bool isConnected = false;

    EgGraphDatabase* egDatabase  {nullptr};        // backlink to db
    EgDataNodesType* linksStorage {nullptr};      // data nodes for links info

    EgDataNodesType* firstType {nullptr};
    EgDataNodesType* secondType {nullptr};

    QString linkName;

    QString firstTypeName;
    QString secondTypeName;

    EgDataNodesLinkType() {}  // required for debug purposes, don't use it ever

    EgDataNodesLinkType(EgGraphDatabase* theDatabase);

    ~EgDataNodesLinkType();

    int AddLink (EgDataNodeIDtype leftNodeID, EgDataNodeIDtype rightNodeID);
    int DeleteLink (EgDataNodeIDtype linkNodeID); // FIXME store linkNodeID or search

    int LoadLinks();
    int StoreLinks();

    int ResolveLinksToPointers();

    int ResolveNodeTypes();

    int LoadLinkedNodes(EgDataNodeIDtype fromNodeID);

};


#endif // EGDATANODESLINK_H
