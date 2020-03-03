/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2020 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

// #ifndef EGDATANODESLINK_H
// #define EGDATANODESLINK_H

#pragma once

#include "egCore.h"
#include "indexes/egIndexesFiles.h"

namespace EgDataNodesLinkNamespace
{
    const char* const egLinkTypesFileName("egLinkTypesMetaInfo");
    const char* const egLinkFileNamePostfix("_egLinkType");
}

class EgGraphDatabase;
class EgDataNodesType;

struct EgLinkNames
{
    QString linkName;

    QString firstTypeName;
    QString secondTypeName;
};


#ifdef EG_LIB_BUILD     // library build switch, define it in project or egCore.h

#include "ecographdb_global.h"

class ECOGRAPHDBSHARED_EXPORT EgLinkType

#else                   // not a library build

class EgLinkType

#endif

// class EgDataNodesLinkType
{
public:
    bool isConnected { false };

    EgGraphDatabase* egDatabase   {nullptr};     // backlink to db
    EgDataNodesType* linksStorage {nullptr};     // data nodes type for links

    EgDataNodesType* fromType {nullptr};
    EgDataNodesType* toType   {nullptr};

    EgLinkNames allLinkNames;

    EgLinkType();

    EgLinkType(EgGraphDatabase* theDatabase);

    ~EgLinkType();

    // int Connect(EgGraphDatabase &myDB, const QString& linkTypeName);

    int Connect(EgGraphDatabase &myDB, const QString& linkTypeName, EgDataNodesType& aFromType, EgDataNodesType& aToType);

    int AddArrowLink (EgDataNodeIdType fromNodeID, EgDataNodeIdType toNodeID);

    int AddLinkToStorageOnly (EgDataNodeIdType fromNodeID, EgDataNodeIdType toNodeID);

    int DeleteLink (EgDataNodeIdType linkNodeID); // FIXME store linkNodeID or search

    int LoadLinks();
    int StoreLinks();

    int ResolveLinksToPointers();

    int ResolveNodeTypes();

    int LoadLinkedNodesFrom(EgDataNodeIdType fromNodeID);

};


// #endif // EGDATANODESLINK_H
