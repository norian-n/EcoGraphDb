/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_DATANODE_H
#define EG_DATANODE_H

// #include <QString>
// #include <QVariant>

#include "egCore.h"

namespace EgDataNodesNamespace
{
    const char* const egDataNodesTypesFileName("egDataNodesTypesMetaInfo");

    const char* const egDummyNodesType("egDummyNodesType");
    const char* const egDummyLinkType ("egDummyLinkType");

    const char* const egGUIfileName("_egGUIcontrolDescriptors");
    const char* const egLocationFileName  ("_egLocationNodes");
    const char* const egAttributesFileName("_egNamedAttributes");
    const char* const egEntryNodesFileName("_egEntryNodes");
}

static QVariant egNotFound("<Not found>");

class EgDataNodeTypeExtraInfo;

struct EgExtendedLinkType
{
    EgDataNodeIdType linkID     { 0 };
    EgDataNodeIdType dataNodeID { 0 };
    EgDataNode* dataNodePtr     { nullptr };

    // EgExtendedLinkType(): dataNodeID(0), dataNodePtr(nullptr) {}
};

struct EgDataNodeLinks           // Data Object Instance 1
{ 
    QMap < QString, QList<EgExtendedLinkType> > inLinks;
    QMap < QString, QList<EgExtendedLinkType> > outLinks;
};

#ifdef EG_LIB_BUILD     // library build switch, define it in project or egCore.h

#include "ecographdb_global.h"

class ECOGRAPHDBSHARED_EXPORT EgDataNode

#else                   // not a library build

class EgDataNode

#endif

{
public:

    EgDataNodeIdType dataNodeID { 0 };
    quint64 dataFileOffset      { 0 };                  // stored offset for local file

    bool isAdded {false};                               // is not stored yet

    EgDataNodeTypeExtraInfo* extraInfo { nullptr };     // data type extrainfo backlink to parent class
    EgDataNodeLinks* nodeLinks         { nullptr };     // links to other nodes, would be created if required only

    QList<QVariant> dataFields;                         // data itself

    EgDataNode() {}
    EgDataNode(EgDataNodeTypeExtraInfo& a_extraInfo);

    EgDataNode(const EgDataNode& copyNode);

    ~EgDataNode();

    void clear();

    // TODO

    // GetConnectedToIDs(QList<EgDataNodeIdType>& toIDs);
    // GetConnectedFromIDs(QList<EgDataNodeIdType>& fromIDs);

    // GetConnectedToPointers(QList<EgDataNode*>& toIDs);
    // GetConnectedFromPointers(QList<EgDataNode*>& fromIDs);

    QVariant& operator [] (QString& fieldName);         // value by name 1
    QVariant& operator [] (const char* fieldCharName);  // value by name 2

};

QDataStream& operator << (QDataStream& dStream, EgDataNode& dNode);    // transfer and file operations
QDataStream& operator >> (QDataStream& dStream, EgDataNode& dNode);    // transfer and file operations


#endif // EG_DATANODE_H
