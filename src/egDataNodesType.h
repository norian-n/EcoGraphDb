#ifndef EG_DATA_NODES_TYPE_H
#define EG_DATA_NODES_TYPE_H

#include "egDataNode.h"
#include "egMetaInfo.h"
#include "egLocalFiles.h"
#include "indexes/egIndexConditions.h"
#include "egDataNodesGUIconnect.h"
#include "egDataNodesLink.h"

class EgDataClient;     // server connection functionality
// class EgDataNodesLinkType;

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

  EgIndexConditions* index_tree;

  EgDataNodeTypeMetaInfo metaInfo;

  QMap <QString, EgDataNodesLinkType*>  myLinkTypes;

  QMap <QString, QMultiMap <EgDataNodeIDtype, EgLoadedLinkType> > inLinks;
  QMap <QString, QMultiMap <EgDataNodeIDtype, EgLoadedLinkType> > outLinks;

  EgDataNodesGUIconnect GUI;

  EgDataNode notFound;             // dummy node

  QMap <EgDataNodeIDtype, EgDataNode>   dataNodes;

  QMap <EgDataNodeIDtype, EgDataNode>   deletedDataNodes;
  QMap <EgDataNodeIDtype, EgDataNode*>  addedDataNodes;
  QMap <EgDataNodeIDtype, EgDataNode*>  updatedDataNodes;

  QSet <quint64> IndexOffsets;           // offsets returned by index tree, for index-based operations (AND, OR)

    // filter callback support
  QList <QVariant> filter_values;        // values for filter callback parameters

  EgDataNodesType();
  // EgDataNodesType(const QString& dclass_name); // : DClassName(dclass_name) {};
  ~EgDataNodesType();

        // basic operations
  int Connect(EgGraphDatabase& myDB,const QString& nodeTypeName, EgRemoteConnect* server = NULL);   // connect to local connection or server, load fields and controls descriptions

  int getMyLinkTypes(); // from myDB TODO

  int StoreData();
  int LoadData();

  int LoadAllData();            // select *

  int AddNewData(QList<QVariant>& myData);
  int AddNewData(EgDataNode& tmpObj);

  int MarkDeletedData(EgDataNodeIDtype nodeID);

  int SetModifiedData(QList<QVariant>& my_data, EgDataNodeIDtype nodeID);
  int SetModifiedData(EgDataNodeIDtype nodeID);

  EgDataNode &operator [](EgDataNodeIDtype objID); // {return GetObjByID(obj_id);}

        // set custom data filter
  void SetLocalFilter(FilterCallbackType f_callback);                           // set predefined local files filter callback

        // service
  int CompressData();           // FIXME delete records marked as deleted
  int PrintObjData();           // debug dump to qDebug()
  int RemoveLocalFiles();       // total destruction

  EgFieldIDtype FieldsCount()       { return metaInfo.dataFields.count(); }
  EgFieldIDtype ModelFieldsCount()  { return GUI.basicControlDescs.count(); }
  EgDataNodeIDtype ObjectsCount()   { return dataNodes.count(); }

  int AddArrowLink(QString linkName, EgDataNodeIDtype fromNode, EgDataNodesType& toType, EgDataNodeIDtype toNode);

  int StoreLinks();
  int LoadLinks();

  int StoreLink(QString linkName);
  int LoadLink(QString linkName);

private:    // no copy constructors
  EgDataNodesType(const EgDataNodesType&);
  EgDataNodesType& operator=(const EgDataNodesType&);

};


#endif // EG_DATA_NODES_TYPE_H
