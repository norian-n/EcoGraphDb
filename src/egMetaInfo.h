/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef META_INFO_H
#define META_INFO_H

#include <QDir>

#include "egDataNode.h"

class EgGraphDatabase;

class EgDataNodeTypeMetaInfo
{
public:
    // EgDataNodeIDtype typeID;            // data class ID
    QString typeName;

    EgDataNodeIDtype nodesCount;          // data objects count
    EgDataNodeIDtype nextObjID;         // next available data object ID

    bool useLocationsNodes;             // locations info in separate nodes type, hardlinked by ID
    bool useNamedAttributes;

    EgGraphDatabase* myECoGraphDB;      // peer database

    QList<QString> dataFields;          // fields of DataNodeType

    QHash<QString, int> nameToOrder;    // map field names to data list order (dont mesh up with data index)
    QHash<QString, int> indexedToOrder; // map indexed fields names to data list order (dont mesh up with data index)

    EgDataNodeTypeMetaInfo(): typeName("Error_no_type_name"), nodesCount(0), nextObjID(1), useLocationsNodes(false), useNamedAttributes(false), myECoGraphDB(NULL) {}
    EgDataNodeTypeMetaInfo(QString a_typeName) : typeName(a_typeName), nodesCount(0), nextObjID(1), useLocationsNodes(false), useNamedAttributes(false), myECoGraphDB(NULL)  {}

    ~EgDataNodeTypeMetaInfo() {}

    void Clear() {nodesCount = 0; nextObjID = 1; dataFields.clear(); /*indexedFields.clear();*/ nameToOrder.clear(); indexedToOrder.clear();}

    void AddDataField(QString fieldName, bool indexed = false);    // add field descriptor, no GUI control data

    int  LocalStoreMetaInfo();               // save to file or server
    int  LocalLoadMetaInfo();                // load from file or server

    void PrintMetaInfo();               // debug print of field descriptions

};

// QDataStream& operator << (QDataStream& dStream, EgDataNodeTypeMetaInfo& metaInfo);
// QDataStream& operator >> (QDataStream& dStream, EgDataNodeTypeMetaInfo& metaInfo);


#endif // META_INFO_H
