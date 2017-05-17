/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EG_DATANODE_H
#define EG_DATANODE_H

#include <QString>
#include <QVariant>
#include <QStandardItemModel>
#include <QDate>

#include "egCore.h"

namespace EgDataNodesNamespace
{
    const char* const egDummyNodesType("egdummynodestype");
    const char* const egDummyLinkType("egdummylinktype");

    const char* const egGUIfileName("_egGUIcontrolDescriptors");
    const char* const egLocationFileName("_egLocationNodes");
    const char* const egAttributesFileName("_egNamedAttributes");
    const char* const egEntryNodesFileName("_egEntryNodes");
}

static QVariant egNotFound("<Not found>");

class EgDataNodeTypeMetaInfo;

struct EgExtendedLinkType
{
      EgDataNodeIDtype dataNodeID;
      EgDataNode* dataNodePtr;

      EgExtendedLinkType(): dataNodeID(0), dataNodePtr(NULL) {}
};

class EgDataNodeLinks           // Data Object Instance 1
{
public:

  QMap <QString, QList<EgExtendedLinkType> > inLinks;
  QMap <QString, QList<EgExtendedLinkType> > outLinks;
};

#ifdef EG_LIB_BUILD     // library build switch, define it in project or egCore.h

#include "ecographdb_global.h"

class ECOGRAPHDBSHARED_EXPORT EgDataNode

#else                   // not a library build

class EgDataNode

#endif

{
public:

    EgDataNodeIDtype dataNodeID = 0;

    quint64 dataFileOffset = 0;

    bool isAdded = false;                         // new data node

    EgDataNodeTypeMetaInfo* metaInfo = nullptr;     // data type metainfo backlink

    EgDataNodeLinks* nodeLinks = nullptr;           // links to other nodes if required

    QList<QVariant> dataFields;           // data itself

    EgDataNode() {}
    EgDataNode(EgDataNodeTypeMetaInfo& a_metaInfo);

    ~EgDataNode();

    void clear();

    QVariant& operator [] (QString& fieldName);  // value by name 1
    QVariant& operator [] (const char* fName);   // value by name 2

};

QDataStream& operator << (QDataStream& d_stream, EgDataNode& d_node);    // transfer and file operations
QDataStream& operator >> (QDataStream& d_stream, EgDataNode& d_node);    // transfer and file operations


#endif // EG_DATANODE_H
