/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_GRAPH_DATABASE_H
#define EG_GRAPH_DATABASE_H

#include <QDir>

#include "egExtraInfo.h"
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
    EgDataNodeTypeExtraInfo* extraInfo = nullptr;
    EgDataNodeTypeExtraInfo* GUIcontrolsExtraInfo  = nullptr;
    EgDataNodeTypeExtraInfo* entryNodesExtraInfo  = nullptr;
    EgDataNodeTypeExtraInfo* locationExtraInfo = nullptr;
    EgDataNodeTypeExtraInfo* attributesExtraInfo = nullptr;

    // EgRemoteConnect*  connection = nullptr;     // connection data (nullptr means local files)

    QDir dir;
    QString currentServerAddress;

    // EgDataNodesType controlDescs;

    QMap<QString, EgDataNodesLinkType>  linkTypes;
    QMap<QString, EgDataNodesType*>  connectedNodeTypes;

    EgGraphDatabase() {} // : isConnected(false), metaInfo(nullptr), locationMetaInfo(nullptr), attributesMetaInfo(nullptr), connection(nullptr) {}

    ~EgGraphDatabase(); //  { if (metaInfo) delete metaInfo; if (locationMetaInfo) delete locationMetaInfo; if (attributesMetaInfo) delete attributesMetaInfo; }

    // int CreateRemoteEgDb(); // FIXME - Remote on server
    int Connect();
    int Attach(EgDataNodesType* nType, const QString& serverAddress = QString());

    bool CheckLinksMetaInfoLocal();
    int  CreateLinksMetaInfo();
    int  LoadLinksMetaInfo();

    bool CheckNodeTypesMetaInfoLocal();
    int  CreateNodeTypesMetaInfo();

    inline void ClearMetaInfo(EgDataNodeTypeExtraInfo* extraInfo);

    int CreateNodeType(const QString& typeName, EgNodeTypeSettings& typeSettings, const QString& serverAddress = QString());

    int AddDataField(const QString &fieldName, bool uint32index = false);    // add field descriptor, no GUI control data
    int AddLocationField(const QString &fieldName, bool uint32index = false);

    int AddDataField(const QString &fieldName, EgIndexSettings indexSettings);    // add field descriptor, no GUI control data
    int AddLocationField(const QString &fieldName, EgIndexSettings indexSettings);

    int CommitNodeType();

    int AddDataNodeType(const QString& typeName);
    int AddLinkType(const QString& linkName, const QString& firstDataNodeType, const QString& secondDataNodeType);

    // int StoreAllLinks();
    // int LoadAllLinks(); // load all links to memory - debug use only

};


#endif // EG_GRAPH_DATABASE_H


