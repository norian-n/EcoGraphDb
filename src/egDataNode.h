#ifndef EG_DATANODE_H
#define EG_DATANODE_H

#include <QString>
#include <QVariant>
#include <QStandardItemModel>
#include <QDate>

#include "egCore.h"

class EgDataNodeTypeMetaInfo;

class EgLoadedLinkType
{
public:
      EgDataNodeIDtype dataNodeID;
      EgDataNode* dataNodePtr;
};


class EgDataNode           // Data Object Instance 1
{
public:

  EgDataNodeIDtype dataNodeID;
  quint64 dataFileOffset;

  bool isAdded;                         // new data node

  EgDataNodeTypeMetaInfo* metaInfo;     // data type info

  QList<QVariant> dataFields;           // data itself

  // QHash <QString, QList<EgDataNode*> > inLinks;
  // QHash <QString, QList<EgDataNode*> > outLinks;

  EgDataNode(): isAdded(false), metaInfo(NULL) {}
  EgDataNode(EgDataNodeTypeMetaInfo& a_metaInfo);

  ~EgDataNode() {}

  void clear();

  QVariant& operator [] (QString& fieldName);  // value by name 1
  QVariant& operator [] (const char* fName);   // value by name 2

};

QDataStream& operator << (QDataStream& d_stream, EgDataNode& d_object);    // transfer and file operations
QDataStream& operator >> (QDataStream& d_stream, EgDataNode& d_object);    // transfer and file operations


#endif // EG_DATANODE_H
