/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EG_GRAPH_DATABASE_H
#define EG_GRAPH_DATABASE_H

#include "egMetaInfo.h"
#include "egDataNodesType.h"

class EgGraphDatabase
{
public:

    bool isConnected;

    EgDataNodeTypeMetaInfo* metaInfo;
    EgDataNodeTypeMetaInfo* locationMetaInfo;
    EgRemoteConnect*  connection;     // connection data (NULL means local files)

    EgDataNodesType controlDescs;

    QMap<QString, EgDataNodesLinkType>  linkTypes;
    QMap<QString, EgDataNodesType*>  connNodeTypes;

    EgGraphDatabase(): isConnected(false), metaInfo(NULL), locationMetaInfo(NULL), connection(NULL) {}

    ~EgGraphDatabase() { if (metaInfo) delete metaInfo; if (locationMetaInfo) delete locationMetaInfo; }

    int Connect();
    int Attach(EgDataNodesType* nType);

    int LoadLinksMetaInfo();

    int CreateNodeType(QString typeName, bool addLocation = false);
    int AddDataField(QString fieldName, bool indexed = false);    // add field descriptor, no GUI control data
    int AddLocationField(QString fieldName, bool indexed = false);
    int CommitNodeType();

    int CreateLinksMetaInfo();
    int AddLinkType(QString linkName, QString firstDataNodeType, QString secondDataNodeType);

    int StoreAllLinks();
    int LoadAllLinks(); // load all links to memory - debug use only

};

#endif // EG_GRAPH_DATABASE_H

// EgGraphDatabase(QString a_typeName) : typeName(a_typeName) {}

// int GetDescsToUpdate(QString typeName);     // get metainfo to update control descriptions
// int DeleteNodeType(QString typeName);   // TODO : password to delete

/*
    int CreateControlDescs();
    int AddSimpleControlDesc(QString fieldName, QString fieldLabel, int fieldWidth);
    int CommitControlDesc();
    */

