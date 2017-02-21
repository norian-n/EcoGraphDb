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
#include "egGUIconnect.h"
#include "egDataNodesLink.h"
#include "egEntryNodes.h"
#include "egNodesLocation.h"
#include "egNamedAttributes.h"

class EgDataClient;     // server connection functionality

struct EgRemoteConnect    // server connection info
{
    QString server_address;
};

class EgDataNodesType   // Data Objects head API
{
public:

    bool isConnected = false;

    EgRemoteConnect*  connection = nullptr;     // connection data (NULL means local files)

    EgDataFiles*  LocalFiles = nullptr;         // data files support functionality
    EgDataClient* ConnectonClient = nullptr;    // server connection client


    EgDataNodesLocation* locations = nullptr;
    EgEntryNodes* entryNodes = nullptr;
    EgDataNodesGUIsupport* GUIsupport = nullptr;
    EgNamedAttributes* namedAttributes = nullptr;

    EgDataNodesType* namedAttributesType; // FIXME remove

    EgIndexConditionsTree* index_tree;

    EgDataNodeTypeMetaInfo metaInfo;

    // EgEntryNodes entryNodesInst;    // FIXME remove
    EgDataNodesGUIsupport GUI; // FIXME remove

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

        // no copy constructors
    EgDataNodesType(const EgDataNodesType&) = delete;
    EgDataNodesType& operator=(const EgDataNodesType&) = delete;

        // basic operations

        // connect to local connection or server, load fields and controls descriptions
    int Connect(EgGraphDatabase& myDB,const QString& nodeTypeName, EgRemoteConnect* server = NULL);

    void ClearData();      // before load

    int LoadData(QString a_FieldName, int an_oper, QVariant a_value); // single index condition ("odb_pit", EQ, projectID);
    int LoadData(const EgIndexCondition &indexCondition);             // any index condition IC("owner", EQ, 2) &&  IC("status", EQ, 3)

    int LoadAllNodes();            // no locations, links, entry, GUI, etc.
    int LoadLinkedData(QString linkName, EgDataNodeIDtype fromNodeID); // only linked nodes from spec node
    // int LoadLocationsData();      // FIXME private, if not load all

    int AutoLoadAll();            // all existing info - locations, links, entry, GUI, etc.

    int StoreData();

    int AddDataNode(QList<QVariant>& myData);
    int AddDataNode(QList<QVariant>& myData, EgDataNodeIDtype &newID); // return ID
    int AddDataNode(EgDataNode& tmpObj);

    int AddHardLinked(QList<QVariant>& myData, EgDataNodeIDtype nodeID);

    // int AddLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID);

    int DeleteDataNode(EgDataNodeIDtype nodeID);

    int UpdateDataNode(QList<QVariant>& my_data, EgDataNodeIDtype nodeID);
    int UpdateDataNode(EgDataNodeIDtype nodeID);

    EgDataNode &operator [](EgDataNodeIDtype objID); // {return GetObjByID(obj_id);}

        // set custom data filter
    void SetLocalFilter(FilterFunctionType theFunction);                           // set predefined local files filter callback
    void SetFilterParams(QList<QVariant>& values);

        // service
    int CompressData();           // FIXME delete records marked as deleted
    int PrintObjData();           // FIXME debug dump

    int RemoveLocalFiles();       // total destruction

    EgFieldIDtype FieldsCount()       { return metaInfo.dataFields.count(); }
    EgFieldIDtype ModelFieldsCount()  { return GUI.basicControlDescs.count(); }
    EgDataNodeIDtype DataNodesCount()   { return dataNodes.count(); }

    int getGUIinfo();

        // links
    int AddArrowLink(QString linkName, EgDataNodeIDtype fromNode, EgDataNodesType& toType, EgDataNodeIDtype toNode);

    int StoreAllLinks();
    int LoadAllLinks();

    int StoreLinkType(QString linkName);
    int LoadLinkType(QString linkName);

    int getMyLinkTypes();         // from myDB TODO

    int AddEntryNode(EgDataNodeIDtype entryNodeID);

};

#endif // EG_DATA_NODES_TYPE_H

// class EgDataNodesLinkType;

// QMap <QString, QMultiMap <EgDataNodeIDtype, EgLoadedLinkType> > inLinks;
// QMap <QString, QMultiMap <EgDataNodeIDtype, EgLoadedLinkType> > outLinks;
