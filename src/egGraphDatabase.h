#ifndef EG_GRAPH_DATABASE_H
#define EG_GRAPH_DATABASE_H

#include "egMetaInfo.h"
#include "egDataNodesType.h"

class EgGraphDatabase
{
public:

    bool isConnected;

    EgDataNodeTypeMetaInfo* metaInfo;

    EgDataNodesType controlDescs;


    QHash<QString, EgDataNodesLinkType>  linkTypes;

    EgGraphDatabase(): isConnected(false), metaInfo(NULL) {}
    // EgGraphDatabase(QString a_typeName) : typeName(a_typeName) {}

    ~EgGraphDatabase() { if (metaInfo) delete metaInfo; }

    int Connect();

    int LoadLinksMetaInfo();

    int CreateNodeType(QString typeName);
    int AddDataField(QString fieldName, bool indexed = false);    // add field descriptor, no GUI control data
    int CommitNodeType();

    int CreateLinksMetaInfo();
    int AddLinkType(QString linkName, QString firstDataNodeType, QString secondDataNodeType);

    int CreateControlDescs();
    int AddSimpleControlDesc(QString fieldName, QString fieldLabel, int fieldWidth);
    int CommitControlDesc();

    int StoreAllLinks();
    int LoadAllLinks(); // load all links to memory - debug use only

    // int GetDescsToUpdate(QString typeName);     // get metainfo to update control descriptions
    // int DeleteNodeType(QString typeName);   // TODO : password to delete

};


#endif // EG_GRAPH_DATABASE_H
