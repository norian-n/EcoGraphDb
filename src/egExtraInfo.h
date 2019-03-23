/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_META_INFO_H
#define EG_META_INFO_H

#include <QDir>
#include <QDataStream>

#include "egDataNode.h"

class EgGraphDatabase;
class EgServerConnection;

class EgDataNodeTypeExtraInfo
{
public:
    // EgDataNodeIDtype typeID;             // data class ID
    QString typeName;

    EgDataNodeIdType nodesCount = 0;        // data objects count
    EgDataNodeIdType nextNodeID = 1;        // next available data object ID

    EgNodeTypeSettings typeSettings;

    EgGraphDatabase* myECoGraphDB = nullptr;      // peer database
/*
    QString serverAddress;

    EgServerConnection* serverConnection = nullptr;
    QDataStream* serverStream = nullptr;
    */

    QList<QString> dataFields;          // fields names of DataNodeType

    QHash<QString, int> nameToOrder;    // map field names to data list order (dont mesh up with data index)
    QHash<QString, EgIndexSettings> indexedFields; // map indexed fields names to structure

        // load & store support
    QFile extraInfoFile;
    QDataStream localExtraInfoStream;


    EgDataNodeTypeExtraInfo(): typeName("Error_no_type_name") {}
    EgDataNodeTypeExtraInfo(const QString& a_typeName) : typeName(a_typeName)
    {
        /*
        typeSettings.useEntryNodes = false;
        typeSettings.useLocation = false;
        typeSettings.useNamedAttributes = false;
        typeSettings.useLinks = false;
        typeSettings.useGUIsettings = false;
        */
    }

    ~EgDataNodeTypeExtraInfo();

    void Clear() {nodesCount = 0; nextNodeID = 1; dataFields.clear(); nameToOrder.clear(); indexedFields.clear();}

    void AddDataField(const QString& fieldName, bool indexed = false);    // add field descriptor, no GUI control data

    void AddDataField(const QString& fieldName, EgIndexSettings indexSettings);

    int  LocalStoreExtraInfo();          // save to file or server
    int  LocalLoadExtraInfo();           // load from file or server

    int  ServerStoreExtraInfo();          // save to file or server
    int  ServerLoadExtraInfo();           // load from file or server

    int  OpenLocalStoreStream();      // save to local file
    void SendExtraInfoToStream(QDataStream& extraInfoStream);

    int  OpenLocalLoadStream();      // load from local file
    int  LoadExtraInfoFromStream(QDataStream& extraInfoStream);

    void PrintExtraInfo();               // debug print of field descriptions

};

QDataStream& operator << (QDataStream& dStream, EgNodeTypeSettings& typeSettings);    // transfer and file operations
QDataStream& operator >> (QDataStream& dStream, EgNodeTypeSettings& typeSettings);    // transfer and file operations

QDataStream& operator << (QDataStream& dStream, EgDataNodeTypeExtraInfo& metaInfo);
QDataStream& operator >> (QDataStream& dStream, EgDataNodeTypeExtraInfo& metaInfo);


#endif // EG_META_INFO_H
