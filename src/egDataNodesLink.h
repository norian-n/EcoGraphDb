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

#include <QString>
#include <QMap>
#include <QVariant>
#include <QFile>
#include <QDir>

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

    // EgDataNodesType* firstNodesType;
    // EgDataNodesType* secondNodesType;

    bool isConnected = false;

    EgGraphDatabase* egDatabase;            // backlink to db

    EgIndexFiles<EgDataNodeIDtype>* fwdIndexFiles = nullptr;    // forward links index

    EgDataNodesType* linksStorage;

    QDir dir;

    QString linkName;           // link file name

    QString firstTypeName;
    QString secondTypeName;

    QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>  addedLinks;
    QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>  deletedLinks;

    QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>  loadedLinks;

    EgDataNodesLinkType(): egDatabase(nullptr), linksStorage(nullptr) {} // new EgDataNodesType()

    EgDataNodesLinkType(EgGraphDatabase* theDatabase);
    ~EgDataNodesLinkType() { if (fwdIndexFiles) delete fwdIndexFiles; }

    int AddLink (EgDataNodeIDtype leftNodeID, EgDataNodeIDtype rightNodeID);
    int DeleteLink (EgDataNodeIDtype linkNodeID);

    int LoadLinks();            // load data links from file or server
    int StoreLinks();           // save data links to file or server
    int ResolveLinks(EgDataNodesType& firstType, EgDataNodesType& secondType);         // move loaded links to data nodes if loaded

    int LoadLinkedNodes(EgDataNodeIDtype fromNodeID);

};


#endif // EGDATANODESLINK_H
