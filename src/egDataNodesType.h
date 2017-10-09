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

#ifdef EG_LIB_BUILD     // library build switch, define it in project or egCore.h

#include "ecographdb_global.h"

class ECOGRAPHDBSHARED_EXPORT EgDataNodesType

#else                   // not a library build

class EgDataNodesType

#endif

{
public:
    bool isConnected = false;

    EgRemoteConnect*  connection = nullptr;         // connection data (NULL means local files)

    EgDataFiles*  LocalFiles = nullptr;             // data files support functionality
    EgDataClient* ConnectonClient = nullptr;        // server connection client

    EgEntryNodes* entryNodes = nullptr;
    EgDataNodesGUIsupport* GUI = nullptr;
    EgDataNodesLocation* locations = nullptr;
    EgNamedAttributes* namedAttributes = nullptr;

    EgIndexConditionsTree* index_tree = nullptr;    // indexed fields operations

    EgDataNodeTypeMetaInfo metaInfo;                // general data nodes type info

    EgDataNode notFound;                            // dummy data node

    QMap <QString, EgDataNodesLinkType*>  myLinkTypes;

        // data nodes content and changes
    QMap <EgDataNodeIDtype, EgDataNode>   dataNodes;

    QMap <EgDataNodeIDtype, EgDataNode>   deletedDataNodes; // TODO process entryNodes on delete
    QMap <EgDataNodeIDtype, EgDataNode*>  addedDataNodes;
    QMap <EgDataNodeIDtype, EgDataNode*>  updatedDataNodes;

    QSet <quint64> IndexOffsets;           // offsets returned by index tree, for index-based operations (AND, OR)

    EgDataNodesType();
    ~EgDataNodesType();

        // no copy constructors yet
    EgDataNodesType(const EgDataNodesType&) = delete;
    EgDataNodesType& operator=(const EgDataNodesType&) = delete;

        // basic operations

        // connect to local connection or server, load fields and controls descriptions FIXME TODO disconnect
    int Connect(EgGraphDatabase& myDB,const QString& nodeTypeName, EgRemoteConnect* server = NULL);

    void ClearData();                   // data nodes content and support data cleanup

    int LoadData(QString a_FieldName, int an_oper, QVariant a_value); // single index condition ("odb_pit", EQ, projectID);
    int LoadData(const EgIndexCondition &indexCondition);             // any index condition IC("owner", EQ, 2) &&  IC("status", EQ, 3)

    int LoadAllNodes();                 // no locations, links, entry, GUI, etc.
    int LoadLinkedData(QString linkName, EgDataNodeIDtype fromNodeID); // only linked nodes from spec node
    // int LoadLocationsData();         // FIXME private, if not load all

    int AutoLoadAllData();              // all existing info - locations, links, entry, GUI, etc.

        // set custom data filter
    void SetLocalFilter(FilterFunctionType theFunction); // set predefined local files filter function/callback
    void SetFilterParams(QList<QVariant>& values);

    int StoreData();

    int AddDataNode(QList<QVariant>& myData);
    int AddDataNode(QList<QVariant>& myData, EgDataNodeIDtype &newID); // return ID
    int AddDataNode(EgDataNode& tmpObj);

    int AddHardLinked(QList<QVariant>& myData, EgDataNodeIDtype nodeID);

    int AddLocation(QList<QVariant>& locationData, EgDataNodeIDtype nodeID);

    int GetLocation(QList<QVariant>& locationData, EgDataNodeIDtype nodeID);

    // int AddLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID);

    int DeleteDataNode(EgDataNodeIDtype nodeID);

    int UpdateDataNode(QList<QVariant>& my_data, EgDataNodeIDtype nodeID);
    int UpdateDataNode(EgDataNodeIDtype nodeID);

    EgDataNode &operator [](EgDataNodeIDtype objID); // {return GetObjByID(obj_id);}

        // links
    int AddArrowLink(const QString &linkName, EgDataNodeIDtype fromNode, EgDataNodesType& toType, EgDataNodeIDtype toNode);

    int StoreAllLinks();
    int LoadAllLinks();

    int StoreLinkType(QString linkName);
    int LoadLinkType(QString linkName);
    // int LoadLinkedIDsOnly(QString linkName); // TODO need connected type names map in the database

    int getMyLinkTypes();         // load from myDB on connect

    int AddEntryNode(EgDataNodeIDtype entryNodeID); // e.g. tree root

        // service
    int CompressData();           // FIXME delete records marked as deleted
    int PrintObjData();           // FIXME debug dump

    int RemoveLocalFiles();       // total destruction

    EgFieldIDtype FieldsCount()       { return metaInfo.dataFields.count(); }
    EgFieldIDtype ModelFieldsCount()  { if (GUI) return GUI-> basicControlDescs.count(); else return 0; }
    EgDataNodeIDtype DataNodesCount() { return dataNodes.count(); }

};

#endif // EG_DATA_NODES_TYPE_H

