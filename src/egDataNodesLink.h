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


#include "egCore.h"
#include "indexes/egIndexesFiles.h"

class EgGraphDatabase;

namespace EgDataNodesLinkNamespace
{
    const char* const egLinkTypesFileName("egLinkTypesMetaInfo");
}

// class EgDataNodesType;

class EgDataNodesLinkType
{
public:

    // EgDataNodesType* firstNodesType;
    // EgDataNodesType* secondNodesType;

    EgGraphDatabase* egDatabase;            // backlink to db

    EgIndexFiles<qint32>* fwdIndexFiles;    // forward links index

    QString linkName;           // link file name

    QString firstTypeName;
    QString secondTypeName;

    QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>  addedLinks;
    QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>  deletedLinks;

    QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>  loadedLinks;

    EgDataNodesLinkType(): fwdIndexFiles(NULL) {}
    ~EgDataNodesLinkType() { if (fwdIndexFiles) delete fwdIndexFiles; }



    // int AddLink (EgDataNode& left_obj, EgDataNode& right_obj); // link objects
    int AddLink (EgDataNodeIDtype leftNodeID, EgDataNodeIDtype rightNodeID);

    // int PrintLinks();        // debug dump

    int LoadLinks();            // load data links from file or server
    int StoreLinks();           // save data links to file or server
    int ResolveLinks();         // move loaded links to data nodes if loaded

    int LoadLinkedNodes(QSet<quint64>& IndexOffsets, EgDataNodeIDtype fromNodeID);

};


#endif // EGDATANODESLINK_H
