/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_DATA_NODES_TYPE_H
#define EG_DATA_NODES_TYPE_H

#include "egDataNode.h"
#include "egExtraInfo.h"
#include "egLocalFiles.h"

#include "egGUIconnect.h"
#include "egDataNodesLink.h"
#include "egEntryNodes.h"
#include "egNodesLocation.h"
#include "egNamedAttributes.h"

#include "indexes/egIndexConditions.h"

class EgServerConnection;     // server connection functionality

#ifdef EG_LIB_BUILD     // library build switch, define it in project or egCore.h

#include "ecographdb_global.h"

class ECOGRAPHDBSHARED_EXPORT EgDataNodesType

#else                   // not a library build

class EgDataNodesType

#endif

{
public:
    bool isConnected = false;

    EgEntryNodes* entryNodes = nullptr;
    EgDataNodesGUIsupport* GUI = nullptr;
    EgDataNodesLocation* locations = nullptr;
    EgNamedAttributes* namedAttributes = nullptr;

    EgDataFiles*  LocalFiles = nullptr;             // data files support functionality
    EgIndexConditionsTree* index_tree = nullptr;    // indexed fields operations

    EgDataNodeTypeExtraInfo extraInfo;                // general data nodes type info

    EgDataNode notFound;                            // dummy data node

    QMap <QString, EgDataNodesLinkType*>  myLinkTypes;

    QSet <quint64> IndexOffsets;           // offsets returned by index tree, for index-based operations (AND, OR)

        // data nodes content and changes
    QMap <EgDataNodeIdType, EgDataNode>   dataNodes;

    QMap <EgDataNodeIdType, EgDataNode>   deletedDataNodes; // TODO process entryNodes on delete
    QMap <EgDataNodeIdType, EgDataNode*>  addedDataNodes;
    QMap <EgDataNodeIdType, EgDataNode*>  updatedDataNodes;


    EgDataNodesType();
    ~EgDataNodesType();

        // no copy constructors yet
    EgDataNodesType(const EgDataNodesType&) = delete;
    EgDataNodesType& operator=(const EgDataNodesType&) = delete;

        // basic operations

        // register node type at EgGraphDatabase, load metadata
    int Connect(EgGraphDatabase& myDB, const QString& nodeTypeName);

    int ConnectLinkType(const QString& linkTypeName);

        // minimal config, no type extensions, no gui, dont use it in apps
    int ConnectServiceNodeType(EgGraphDatabase& myDB, const QString& nodeTypeName);

    inline void initNotFoundVirtualNode();

    void ClearData();                   // data nodes content and support data cleanup

    int LoadDataByIndexes(QString a_FieldName, int an_oper, QVariant a_value); // single index condition ("odb_pit", EQ, projectID);
    int LoadDataByIndexes(const EgIndexCondition &indexCondition);             // any index condition IC("owner", EQ, 2) &&  IC("status", EQ, 3)

    int LoadAllDataNodes();                 // no locations, links, entry, GUI, etc.
    int LoadLinkedData(QString linkName, EgDataNodeIdType fromNodeID); // only nodes linked to this one
    // int LoadLocationsData();         // FIXME private, if not load all

    int AutoLoadAllData();              // all existing info - locations, links, entry, GUI, etc.

        // set custom data filter
    void SetLocalFilter(FilterFunctionType theFunction); // set predefined local files filter function/callback
    void SetFilterParams(QList<QVariant>& values);

    int StoreData();

    int AddDataNode(QList<QVariant>& myData);
    int AddDataNode(QList<QVariant>& myData, EgDataNodeIdType &newID); // return ID
    int AddDataNode(EgDataNode& tmpObj);

    int AddHardLinked(QList<QVariant>& myData, EgDataNodeIdType nodeID);

    int AddLocation(QList<QVariant>& locationData, EgDataNodeIdType nodeID);

    int GetLocation(QList<QVariant>& locationData, EgDataNodeIdType nodeID);

    // int AddLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID);

    int DeleteDataNode(EgDataNodeIdType nodeID);

    int UpdateDataNode(QList<QVariant>& my_data, EgDataNodeIdType nodeID);
    int UpdateDataNode(EgDataNodeIdType nodeID);

    EgDataNode &operator [](EgDataNodeIdType objID); // {return GetObjByID(obj_id);}

        // links
    int AddArrowLink(const QString &linkName, EgDataNodeIdType fromNode, EgDataNodesType& toType, EgDataNodeIdType toNode);

    int StoreAllLinks();
    int LoadAllLinks();

    int StoreLinkType(QString linkName);
    int LoadLinkType(QString linkName);
    // int LoadLinkedIDsOnly(QString linkName); // TODO need connected type names map in the database

    int getMyLinkTypes();         // load from myDB on connect

    int AddEntryNode(EgDataNodeIdType entryNodeID); // e.g. tree root

        // service
    int CompressData();           // FIXME delete records marked as deleted
    int PrintObjData();           // FIXME debug dump

    int RemoveLocalFiles();       // total destruction

    EgFieldIDtype FieldsCount()       { return extraInfo.dataFields.count(); }
    EgFieldIDtype ModelFieldsCount()  { if (GUI) return GUI-> basicControlDescs.count(); else return 0; }
    EgDataNodeIdType DataNodesCount() { return dataNodes.count(); }

};

#endif // EG_DATA_NODES_TYPE_H

