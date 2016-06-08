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

class EgDataNodeTypeMetaInfo;

struct EgExtendedLinkType
{
      EgDataNodeIDtype dataNodeID;
      EgDataNode* dataNodePtr;
};

class EgDataNodeLinks           // Data Object Instance 1
{
public:

  QHash <QString, QList<EgExtendedLinkType> > inLinks;
  QHash <QString, QList<EgExtendedLinkType> > outLinks;
};

class EgDataNode           // Data Object Instance 1
{
public:

    EgDataNodeIDtype dataNodeID;

    quint64 dataFileOffset;

    bool isAdded;                         // new data node

    EgDataNodeTypeMetaInfo* metaInfo;     // data type metainfo backlink

    EgDataNodeLinks* nodeLinks;           // links to other nodes if required

    QList<QVariant> dataFields;           // data itself

    EgDataNode(): isAdded(false), metaInfo(NULL), nodeLinks(NULL) {}
    EgDataNode(EgDataNodeTypeMetaInfo& a_metaInfo);

    ~EgDataNode() { if (nodeLinks) delete nodeLinks; }

    void clear();

    QVariant& operator [] (QString& fieldName);  // value by name 1
    QVariant& operator [] (const char* fName);   // value by name 2

};

QDataStream& operator << (QDataStream& d_stream, EgDataNode& d_object);    // transfer and file operations
QDataStream& operator >> (QDataStream& d_stream, EgDataNode& d_object);    // transfer and file operations


#endif // EG_DATANODE_H

/*
class EgLoadedLinkType
{
public:
      EgDataNodeIDtype dataNodeID;
      EgDataNode* dataNodePtr;
};
*/
