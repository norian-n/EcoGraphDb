/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EG_DATA_NODES_TYPE_H
#define EG_DATA_NODES_TYPE_H

#include "egDataNode.h"
#include "egMetaInfo.h"
#include "egLocalFiles.h"
#include "indexes/egIndexConditions.h"
#include "egDataNodesGUIconnect.h"
#include "egDataNodesLink.h"
#include "egEntryNodes.h"

class EgDataClient;     // server connection functionality

struct EgRemoteConnect    // server connection info
{
    QString server_address;
};

class EgDataNodesType   // Data Objects head API
{
public:

  EgDataFiles*  LocalFiles;         // data files support functionality
  EgDataClient* ConnectonClient;    // server connection client
  EgRemoteConnect*  connection;     // connection data (NULL means local files)

  EgDataNodesType* locationNodesType;

  EgIndexConditionsTree* index_tree;

  EgDataNodeTypeMetaInfo metaInfo;

  EgEntryNodes entryNodesInst;

  EgDataNodesGUIconnect GUI;

  EgDataNode notFound;             // dummy node

  QMap <QString, EgDataNodesLinkType*>  myLinkTypes;

  QMap <EgDataNodeIDtype, EgDataNode>   dataNodes;

  QMap <EgDataNodeIDtype, EgDataNode>   deletedDataNodes; // TODO process entryNodes on delete
  QMap <EgDataNodeIDtype, EgDataNode*>  addedDataNodes;
  QMap <EgDataNodeIDtype, EgDataNode*>  updatedDataNodes;

  QSet <quint64> IndexOffsets;           // offsets returned by index tree, for index-based operations (AND, OR)

  EgDataNodesType();
  // EgDataNodesType(const QString& dclass_name); // : DClassName(dclass_name) {};
  ~EgDataNodesType();

        // basic operations

        // connect to local connection or server, load fields and controls descriptions
  int Connect(EgGraphDatabase& myDB,const QString& nodeTypeName, EgRemoteConnect* server = NULL);
  int getGUIinfo();

  int getMyLinkTypes(); // from myDB TODO

  int StoreData();

  int LoadData(QString a_FieldName, int an_oper, QVariant a_value); // single index condition ("odb_pit", EQ, projectID);
  int LoadData(const EgIndexCondition &indexCondition);             // any index condition IC("owner", EQ, 2) &&  IC("status", EQ, 3)

  int LoadAllData();            // select *
  int LoadLinkedData(QString linkName, EgDataNodeIDtype fromNodeID);

  int AddDataNode(QList<QVariant>& myData);
  int AddDataNode(QList<QVariant>& myData, EgDataNodeIDtype &newID); // return ID
  int AddDataNode(EgDataNode& tmpObj);

  int AddHardLinked(QList<QVariant>& myData, EgDataNodeIDtype nodeID);

  int AddLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID);

  int DeleteDataNode(EgDataNodeIDtype nodeID);

  int UpdateDataNode(QList<QVariant>& my_data, EgDataNodeIDtype nodeID);
  int UpdateDataNode(EgDataNodeIDtype nodeID);

  EgDataNode &operator [](EgDataNodeIDtype objID); // {return GetObjByID(obj_id);}

        // set custom data filter
  void SetLocalFilter(FilterFunctionType theFunction);                           // set predefined local files filter callback
  void SetFilterParams(QList<QVariant>& values);

        // service
  int CompressData();           // FIXME delete records marked as deleted
  int PrintObjData();           // debug dump to qDebug()
  int RemoveLocalFiles();       // total destruction

  EgFieldIDtype FieldsCount()       { return metaInfo.dataFields.count(); }
  EgFieldIDtype ModelFieldsCount()  { return GUI.basicControlDescs.count(); }
  EgDataNodeIDtype DataNodesCount()   { return dataNodes.count(); }

  int AddArrowLink(QString linkName, EgDataNodeIDtype fromNode, EgDataNodesType& toType, EgDataNodeIDtype toNode);

  int StoreLinks();
  int LoadLinks();

  int StoreLink(QString linkName);
  int LoadLink(QString linkName);

  int AddEntryNode(EgDataNodeIDtype entryNodeID);

private:    // no copy constructors
  EgDataNodesType(const EgDataNodesType&);
  EgDataNodesType& operator=(const EgDataNodesType&);

};

#endif // EG_DATA_NODES_TYPE_H

// class EgDataNodesLinkType;

// QMap <QString, QMultiMap <EgDataNodeIDtype, EgLoadedLinkType> > inLinks;
// QMap <QString, QMultiMap <EgDataNodeIDtype, EgLoadedLinkType> > outLinks;
