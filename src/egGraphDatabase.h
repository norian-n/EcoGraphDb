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

namespace egGraphDatabaseNamespace
{
    const char* const egDefaultLogFileName {"egGlobalLogFile"};
}

#ifdef EG_LIB_BUILD   // library build switch, define it in project or egCore.h

#include "ecographdb_global.h"

class ECOGRAPHDBSHARED_EXPORT EgGraphDatabase

#else   // no library build

class EgGraphDatabase

#endif

{
public:

    // TO DO : make it singleton with multi server connections

    bool isConnected = false;

    int fieldsCreated = 0;          // info for testing
    int locationFieldsCreated = 0;

        // node type creation-time temporary info
    EgDataNodeTypeExtraInfo* extraInfo = nullptr;
    EgDataNodeTypeExtraInfo* GUIcontrolsExtraInfo  = nullptr;
    EgDataNodeTypeExtraInfo* entryNodesExtraInfo  = nullptr;
    EgDataNodeTypeExtraInfo* locationExtraInfo = nullptr;
    EgDataNodeTypeExtraInfo* attributesExtraInfo = nullptr;

    // EgRemoteConnect*  connection = nullptr;     // connection data (nullptr means local files)

        // server section
    EgServerConnection* serverConnection = nullptr;
    QDataStream* serverStream = nullptr;

    QString serverAddress;

    QDir dir; // FIXME move to local files

    // EgDataNodesType controlDescs;

    QMap<QString, EgLinkNames>  linkTypes;  // FIXME EgLinkNames loaded from file
    QMap<QString, QString>  dataNodeTypes;          // loaded from file

    QMap<QString, EgLinkType*>  attachedLinkTypes;
    QMap<QString, EgDataNodesType*>  attachedNodeTypes; // added via AttachNodesType() call

#ifdef EG_LOG
        // logger

    egAsyncLoggerEngine logEngine;
    egAsyncLogger theLogger;

    EgGraphDatabase() :
        logEngine(egGraphDatabaseNamespace::egDefaultLogFileName),
        theLogger(logEngine)
    {
        if (logEngine.LaunchLoggerEngine()) // returns not zero
            qDebug()  << "Can't launch async logger engine, fix (probably file) issue and restart" << FN;
        else
            egGlobalLoggerPtr = &theLogger;
    }
#else
    EgGraphDatabase() {}
#endif

    ~EgGraphDatabase(); //  { if (metaInfo) delete metaInfo; if (locationMetaInfo) delete locationMetaInfo; if (attributesMetaInfo) delete attributesMetaInfo; }

    // int CreateRemoteEgDb(); // FIXME - Remote on server

    // int CheckServerConnection();

    int AttachNodesType(EgDataNodesType* nType);
    int AttachLinksType(EgLinkType* linkType);

    bool CheckLinksMetaInfoLocal();
    int  CreateLinksMetaInfo();
    int  LoadLinksMetaInfo();

    bool CheckNodeTypesMetaInfoLocal();
    int  CreateNodeTypesMetaInfo();
    int  LoadNodeTypesMetaInfo();

    int  AddDataNodeTypeMetaInfo(const QString& typeName);

    inline void ClearExtraInfo(EgDataNodeTypeExtraInfo* extraInfo);

    // TODO : CreateServiceNodeType(const QString& typeName, const QString& serverAddress = QString())

    int CreateNodeType(const QString& typeName, EgNodeTypeSettings& typeSettings);

    int AddDataField(const QString &fieldName, bool uint32index = false);    // add field descriptor, no GUI control data
    int AddLocationField(const QString &fieldName, bool uint32index = false);

    int AddDataField(const QString &fieldName, EgIndexSettings indexSettings);    // add field descriptor, no GUI control data
    int AddLocationField(const QString &fieldName, EgIndexSettings indexSettings);

    int CommitNodeType();

    int AddLinkType(const QString& linkName, const QString& firstDataNodeType, const QString& secondDataNodeType);

    // int StoreAllLinks();
    // int LoadAllLinks(); // load all links to memory - debug use only

};


#endif // EG_GRAPH_DATABASE_H


