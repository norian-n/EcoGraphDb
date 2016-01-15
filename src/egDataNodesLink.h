#ifndef EGDATANODESLINK_H
#define EGDATANODESLINK_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QFile>


#include "egCore.h"

class EgGraphDatabase;

namespace EgDataNodesLinkNamespace
{
    const char* const egLinkTypesFileName("egLinkTypesMetaInfo");
}

// class EgDataNodesType;

class EgDataNodesLinkType
{
public:

    // EgDataNodesType* firstNodesType;
    // EgDataNodesType* secondNodesType;

    EgGraphDatabase* egDatabase;

    QString linkName;           // link file name

    QString firstTypeName;
    QString secondTypeName;

    QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>  addedLinks;
    QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>  deletedLinks;

    QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>  loadedLinks;

    EgDataNodesLinkType() {}
    ~EgDataNodesLinkType() {}



    // int AddLink (EgDataNode& left_obj, EgDataNode& right_obj); // link objects
    int AddLink (EgDataNodeIDtype leftNodeID, EgDataNodeIDtype rightNodeID);

    // int PrintLinks();        // debug dump

    int LoadLinks();            // load data links from file or server

    int StoreLinks();           // save data links to file or server

    int ResolveLinks();         // move loaded links to data nodes if loaded

};


#endif // EGDATANODESLINK_H
