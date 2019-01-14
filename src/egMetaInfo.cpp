/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egMetaInfo.h"
#include "egDataNodesType.h"
#include "egDataClient.h"

#include <QtDebug>

EgDataNodeTypeMetaInfo::~EgDataNodeTypeMetaInfo()
{
    Clear();
    metaInfoFile.close();

    if (serverConnection)
        delete serverConnection;
}

void EgDataNodeTypeMetaInfo::AddDataField(QString fieldName, bool indexed)
{
    int order = dataFields.count();

    dataFields.append(fieldName);

    nameToOrder.insert(fieldName, order);

    if (indexed)
    {
        // indexedToOrder.insert(fieldName, order);

        EgIndexSettings indexSettings;

        indexSettings.fieldNum = order;
        indexSettings.indexSize = 32;
        indexSettings.isSigned = 0;
        indexSettings.functionID = 0;

        indexedFields.insert(fieldName, indexSettings);
    }
}

void EgDataNodeTypeMetaInfo::AddDataField(QString fieldName, EgIndexSettings indexSettings)
{
    int order = dataFields.count();

    dataFields.append(fieldName);

    nameToOrder.insert(fieldName, order);

    indexSettings.fieldNum = order;
    indexedFields.insert(fieldName, indexSettings);
}

int  EgDataNodeTypeMetaInfo::OpenLocalStoreStream()
{
    QDir dir(QDir::current());

    if (dir.dirName() == "egdb")
    {
        qDebug() << "error: unexpected folder: " << dir.path() << FN;

        return -1;
    }

    // qDebug()  << "dir name: " << dir.dirName() << FN;
    // qDebug()  << "dir current path: " << dir.currentPath() << FN;
    // qDebug()  << dir.entryList() << FN;

        // open file
    metaInfoFile.setFileName("egdb/" + typeName + ".ddt");
    localMetaInfoStream.setDevice(&metaInfoFile);

    if (!metaInfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "can't open " << metaInfoFile.fileName() << " file" << FN;

        return -1;
    }

    return 0;
}

void EgDataNodeTypeMetaInfo::SendMetaInfoToStream(QDataStream& metaInfoStream)
{
    metaInfoStream << nodesCount;  // data nodes (NOT field descriptors) count
    metaInfoStream << nextNodeID;   // incremental counter

    metaInfoStream << useEntryNodes;
    metaInfoStream << useLocation;
    metaInfoStream << useNamedAttributes;
    metaInfoStream << useLinks;
    metaInfoStream << useGUIsettings;

    metaInfoStream << dataFields;  // field descriptors
    // dStream << indexedToOrder.keys();

    metaInfoStream << (quint32) indexedFields.size();

    for (auto indIter = indexedFields.begin(); indIter != indexedFields.end(); ++indIter)
    {
        // qDebug() << indIter.key() << " " << indIter.value().fieldNum << FN;

        metaInfoStream << indIter.key();
        metaInfoStream << indIter.value().fieldNum;
        metaInfoStream << indIter.value().indexSize;
        metaInfoStream << indIter.value().isSigned;
        metaInfoStream << indIter.value().functionID;
    }
}


int EgDataNodeTypeMetaInfo::LocalStoreMetaInfo()
{
    int res = 0;

    res = OpenLocalStoreStream();

    if (! res)
        SendMetaInfoToStream(localMetaInfoStream);

        // localMetaInfoStream << *this;

    metaInfoFile.close();

    return res;
}

int EgDataNodeTypeMetaInfo::LocalUpdateMetaInfo()
{
    int res = 0;

    res = OpenLocalStoreStream();

    if (! res)
        SendMetaInfoToStream(localMetaInfoStream);

        // localMetaInfoStream << *this;

    metaInfoFile.close();

    return res;
}

int EgDataNodeTypeMetaInfo::ServerStoreMetaInfo()
{
    int res = 0;

    if (serverConnection)
        res = serverConnection->OpenStoreStream(opcode_store_metainfo, serverStream, typeName);

    if (! res)
        SendMetaInfoToStream(*serverStream);

        // *serverStream << *this;

    if (! res)
        res = serverConnection-> WaitForSending();

    serverConnection-> Disconnect();

    return res;
}

int EgDataNodeTypeMetaInfo::ServerUpdateMetaInfo()
{
    int res = 0;

    if (serverConnection)
        res = serverConnection->OpenStoreStream(opcode_store_metainfo, serverStream, typeName);

    if (! res)
        SendMetaInfoToStream(*serverStream);

        // *serverStream << *this;

    if (! res)
        res = serverConnection-> WaitForSending();

    serverConnection-> Disconnect();

    return res;
}


int EgDataNodeTypeMetaInfo::ServerLoadMetaInfo()
{
    int res = 0;

    if (serverConnection)
        res = serverConnection-> OpenLoadStream(opcode_load_metainfo, serverStream, typeName);

    if (! res)
        res = serverConnection-> WaitForSending(); // command

    // qDebug << serverConnection-> tcpSocket.state() << "" << ; WaitForReadyRead

    if (! res)
        res = serverConnection-> WaitForReadyRead();

    if (! res)
        res = LoadMetaInfoFromStream(*serverStream);

    serverConnection-> Disconnect();


    return res;
}

int EgDataNodeTypeMetaInfo::OpenLocalLoadStream()
{
        // open file
    metaInfoFile.setFileName("egdb/" + typeName + ".ddt");
    localMetaInfoStream.setDevice(&metaInfoFile);

    if (!metaInfoFile.open(QIODevice::ReadOnly))
    {
        // if (! typeName.contains(EgDataNodesNamespace::egGUIfileName))
        qDebug() << FN << "can't open " << typeName + ".ddt" << " file";

        return -1;
    }

    return 0;
}

int EgDataNodeTypeMetaInfo::LoadMetaInfoFromStream(QDataStream& metaInfoStream)
{
    // QList<QString> indexedFieldsLocal;

    Clear(); // init metainfo

    metaInfoStream >> nodesCount;  // data nodes (NOT field descriptors) count
    metaInfoStream >> nextNodeID;   // incremental counter

    metaInfoStream >> useEntryNodes;
    metaInfoStream >> useLocation;
    metaInfoStream >> useNamedAttributes;
    metaInfoStream >> useLinks;
    metaInfoStream >> useGUIsettings;

    metaInfoStream  >> dataFields;  // field descriptors
    // metaInfoStream  >> indexedFieldsLocal;

    quint32 theSize = 0;
    EgIndexSettings theSettings;
    QString theName;

    metaInfoStream >> theSize;

    for (quint32 i = 0; i < theSize; i++)
    {
        metaInfoStream  >> theName;
        metaInfoStream  >> theSettings.fieldNum;
        metaInfoStream  >> theSettings.indexSize;
        metaInfoStream  >> theSettings.isSigned;
        metaInfoStream  >> theSettings.functionID;

        // qDebug() << theName << " " << theSettings.fieldNum << FN;

        indexedFields.insert(theName, theSettings);
    }

    int order = 0;

        // QList<QString>::iterator
    for (auto stringListIter = dataFields.begin(); stringListIter != dataFields.end(); stringListIter++)
        nameToOrder.insert(*stringListIter, order++);

    return 0;
}

int EgDataNodeTypeMetaInfo::LocalLoadMetaInfo()
{
    int res = 0;

    res = OpenLocalLoadStream();

    if (! res)
        res = LoadMetaInfoFromStream(localMetaInfoStream);

    metaInfoFile.close();

    return res;
}

void EgDataNodeTypeMetaInfo::PrintMetaInfo()
{
     qDebug() << FN << "\nType name:" << typeName << " Obj Count:" << nodesCount << " Next ID:" << nextNodeID;

     qDebug() << "Fields:";
     qDebug() << dataFields;

     qDebug() << "Fields order:";
     for (auto curDesc = nameToOrder.begin(); curDesc != nameToOrder.end(); ++curDesc)
         qDebug() << curDesc.value() << " " << curDesc.key();

     // qDebug() << "Indexes order:";
     // qDebug() << indexedFields;

     qDebug() << "Indexes order:";
     // for (auto curDesc = indexedToOrder.begin(); curDesc != indexedToOrder.end(); ++curDesc)
     //    qDebug() << curDesc.value() << " " << curDesc.key();
}


QDataStream& operator << (QDataStream& dStream, EgNodeTypeSettings& typeSettings)    // transfer and file operations
{
    dStream << typeSettings.useEntryNodes;
    dStream << typeSettings.useGUIsettings;
    dStream << typeSettings.useLinks;
    dStream << typeSettings.useLocation;
    dStream << typeSettings.useNamedAttributes;

    return dStream;
}

QDataStream& operator >> (QDataStream& dStream, EgNodeTypeSettings& typeSettings)    // transfer and file operations
{
    dStream >> typeSettings.useEntryNodes;
    dStream >> typeSettings.useGUIsettings;
    dStream >> typeSettings.useLinks;
    dStream >> typeSettings.useLocation;
    dStream >> typeSettings.useNamedAttributes;

    return dStream;
}

// store meta info
QDataStream& operator << (QDataStream& metaInfoStream, EgDataNodeTypeMetaInfo &metaInfo)
{
    metaInfoStream << metaInfo.nodesCount;  // data nodes (NOT field descriptors) count
    metaInfoStream << metaInfo.nextNodeID;   // incremental counter

    metaInfoStream << metaInfo.useEntryNodes;
    metaInfoStream << metaInfo.useLocation;
    metaInfoStream << metaInfo.useNamedAttributes;
    metaInfoStream << metaInfo.useLinks;
    metaInfoStream << metaInfo.useGUIsettings;

    metaInfoStream << metaInfo.dataFields;  // field descriptors
    // dStream << indexedToOrder.keys();

    metaInfoStream << (quint32) metaInfo.indexedFields.size();

    for (auto indIter = metaInfo.indexedFields.begin(); indIter != metaInfo.indexedFields.end(); ++indIter)
    {
        // qDebug() << indIter.key() << " " << indIter.value().fieldNum << FN;

        metaInfoStream << indIter.key();
        metaInfoStream << indIter.value().fieldNum;
        metaInfoStream << indIter.value().indexSize;
        metaInfoStream << indIter.value().isSigned;
        metaInfoStream << indIter.value().functionID;
    }

    return metaInfoStream;
}

// load meta info
QDataStream& operator >> (QDataStream& metaInfoStream, EgDataNodeTypeMetaInfo& metaInfo)
{
    // QList<QString> indexedFieldsLocal;

    metaInfo.Clear(); // init metainfo

    metaInfoStream >> metaInfo.nodesCount;  // data nodes (NOT field descriptors) count
    metaInfoStream >> metaInfo.nextNodeID;   // incremental counter

    metaInfoStream >> metaInfo.useEntryNodes;
    metaInfoStream >> metaInfo.useLocation;
    metaInfoStream >> metaInfo.useNamedAttributes;
    metaInfoStream >> metaInfo.useLinks;
    metaInfoStream >> metaInfo.useGUIsettings;

    metaInfoStream  >> metaInfo.dataFields;  // field descriptors
    // metaInfoStream  >> indexedFieldsLocal;

    quint32 theSize = 0;
    EgIndexSettings theSettings;
    QString theName;

    metaInfoStream >> theSize;

    for (quint32 i = 0; i < theSize; i++)
    {
        metaInfoStream  >> theName;
        metaInfoStream  >> theSettings.fieldNum;
        metaInfoStream  >> theSettings.indexSize;
        metaInfoStream  >> theSettings.isSigned;
        metaInfoStream  >> theSettings.functionID;

        // qDebug() << theName << " " << theSettings.fieldNum << FN;

        metaInfo.indexedFields.insert(theName, theSettings);
    }

    int order = 0;

        // QList<QString>::iterator
    for (auto stringListIter = metaInfo.dataFields.begin(); stringListIter != metaInfo.dataFields.end(); stringListIter++)
        metaInfo.nameToOrder.insert(*stringListIter, order++);

    return metaInfoStream;
}

