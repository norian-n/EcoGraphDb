/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_GRAPH_DATABASE_H
#define EG_GRAPH_DATABASE_H

#include <QDir>

#include "egMetaInfo.h"
#include "egDataNodesType.h"

#ifdef EG_LIB_BUILD   // library build switch, define it in project or egCore.h

#include "ecographdb_global.h"

class ECOGRAPHDBSHARED_EXPORT EgGraphDatabase

#else   // no library build

class EgGraphDatabase

#endif

{
public:

    bool isConnected = false;

    int fieldsCreated = 0; // info for testing
    int locationFieldsCreated = 0;

        // node type creation-time temporary info
    EgDataNodeTypeMetaInfo* metaInfo = nullptr;
    EgDataNodeTypeMetaInfo* GUIcontrolsMetaInfo  = nullptr;
    EgDataNodeTypeMetaInfo* entryNodesMetaInfo  = nullptr;
    EgDataNodeTypeMetaInfo* locationMetaInfo = nullptr;
    EgDataNodeTypeMetaInfo* attributesMetaInfo = nullptr;

    EgRemoteConnect*  connection = nullptr;     // connection data (nullptr means local files)

    QDir dir;

    // EgDataNodesType controlDescs;

    QMap<QString, EgDataNodesLinkType>  linkTypes;
//     QMap<QString, EgDataNodesType*>  connNodeTypes;

    EgGraphDatabase() {} // : isConnected(false), metaInfo(nullptr), locationMetaInfo(nullptr), attributesMetaInfo(nullptr), connection(nullptr) {}

    ~EgGraphDatabase(); //  { if (metaInfo) delete metaInfo; if (locationMetaInfo) delete locationMetaInfo; if (attributesMetaInfo) delete attributesMetaInfo; }

    // int CreateRemoteEgDb(); // FIXME - Remote on server
    int Connect();
    // int Attach(EgDataNodesType* nType);

    bool CheckLinksMetaInfo();
    int  CreateLinksMetaInfo();
    int  LoadLinksMetaInfo();

    inline void ClearMetaInfo(EgDataNodeTypeMetaInfo* metaInfo);

    int CreateNodeType(QString typeName, EgNodeTypeSettings& typeSettings);

    // int CreateNodeType(QString typeName, bool addLocation = false, bool addAttributes = false); // FIXME delete

    int AddDataField(QString fieldName, bool uint32index = false);    // add field descriptor, no GUI control data
    int AddLocationField(QString fieldName, bool uint32index = false);

    int AddDataField(QString fieldName, EgIndexSettings indexSettings);    // add field descriptor, no GUI control data
    int AddLocationField(QString fieldName, EgIndexSettings indexSettings);

    int CommitNodeType();

    int AddLinkType(QString linkName, QString firstDataNodeType, QString secondDataNodeType);

    // int StoreAllLinks();
    // int LoadAllLinks(); // load all links to memory - debug use only

};

#endif // EG_GRAPH_DATABASE_H


