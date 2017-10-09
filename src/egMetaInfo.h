/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EG_META_INFO_H
#define EG_META_INFO_H

#include <QDir>

#include "egDataNode.h"

class EgGraphDatabase;

class EgDataNodeTypeMetaInfo
{
public:
    // EgDataNodeIDtype typeID;            // data class ID
    QString typeName;

    EgDataNodeIDtype nodesCount = 0;          // data objects count
    EgDataNodeIDtype nextObjID = 1;         // next available data object ID

        // copy of EgNodeTypeSettings fields
    bool useEntryNodes = false;         // start points for graph operations
    bool useLocation = false;           // locations info in separate nodes type, hardlinked by ID
    bool useNamedAttributes = false;    // named attributes/properties in separate nodes type
    bool useLinks = false;
    bool useGUIsettings = false;

    EgGraphDatabase* myECoGraphDB = nullptr;      // peer database

    QList<QString> dataFields;          // fields of DataNodeType

    QHash<QString, int> nameToOrder;    // map field names to data list order (dont mesh up with data index)
    // QHash<QString, int> indexedToOrder; // map indexed fields names to data list order (dont mesh up with data index)
    QHash<QString, EgIndexSettings> indexedFields; // map indexed fields names to structure


    EgDataNodeTypeMetaInfo(): typeName("Error_no_type_name") {}
    EgDataNodeTypeMetaInfo(QString a_typeName) : typeName(a_typeName) {}

    ~EgDataNodeTypeMetaInfo() {}

    void Clear() {nodesCount = 0; nextObjID = 1; dataFields.clear(); nameToOrder.clear(); indexedFields.clear();}

    void AddDataField(QString fieldName, bool indexed = false);    // add field descriptor, no GUI control data

    void AddDataField(QString fieldName, EgIndexSettings indexSettings);

    int  LocalStoreMetaInfo();               // save to file or server
    int  LocalLoadMetaInfo();                // load from file or server

    void PrintMetaInfo();               // debug print of field descriptions

};

// QDataStream& operator << (QDataStream& dStream, EgDataNodeTypeMetaInfo& metaInfo);
// QDataStream& operator >> (QDataStream& dStream, EgDataNodeTypeMetaInfo& metaInfo);


#endif // EG_META_INFO_H
