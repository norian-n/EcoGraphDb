/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egExtraInfo.h"
#include "egDataNodesType.h"
#include "egDataClient.h"
#include "egGraphDatabase.h"

#include <QtDebug>

EgDataNodeTypeExtraInfo::~EgDataNodeTypeExtraInfo()
{
    Clear();
    extraInfoFile.close();

    // if (serverConnection)
    //    delete serverConnection;
}

void EgDataNodeTypeExtraInfo::AddDataField(const QString& fieldName, bool indexed)
{
    int order = dataFields.count();

    dataFields.append(fieldName);

    nameToOrder.insert(fieldName, order);

    if (indexed)
    {
        // indexedToOrder.insert(fieldName, order);

        EgIndexSettings indexSettings;

        indexSettings.fieldNum = order;
        indexSettings.indexSize = 32;   // FIXME check
        indexSettings.isSigned = 0;
        indexSettings.functionID = 0;

        indexedFields.insert(fieldName, indexSettings);
    }
}

void EgDataNodeTypeExtraInfo::AddDataField(const QString& fieldName, EgIndexSettings indexSettings)
{
    int order = dataFields.count();

    dataFields.append(fieldName);

    nameToOrder.insert(fieldName, order);

    indexSettings.fieldNum = order;
    indexedFields.insert(fieldName, indexSettings);
}

int  EgDataNodeTypeExtraInfo::OpenLocalStoreStream()
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
    extraInfoFile.setFileName("egdb/" + typeName + ".ddt");
    localExtraInfoStream.setDevice(&extraInfoFile);

    if (!extraInfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "can't open " << extraInfoFile.fileName() << " file" << FN;

        return -1;
    }

    return 0;
}

void EgDataNodeTypeExtraInfo::SendExtraInfoToStream(QDataStream& extraInfoStream)
{
    extraInfoStream << nodesCount;  // data nodes (NOT field descriptors) count
    extraInfoStream << nextNodeID;   // incremental counter

    extraInfoStream << typeSettings.useEntryNodes;
    extraInfoStream << typeSettings.useLocation;
    extraInfoStream << typeSettings.useNamedAttributes;
    extraInfoStream << typeSettings.useLinks;
    extraInfoStream << typeSettings.useGUIsettings;

    extraInfoStream << dataFields;  // field descriptors
    // dStream << indexedToOrder.keys();

    extraInfoStream << (quint32) indexedFields.size();

    for (auto indIter = indexedFields.begin(); indIter != indexedFields.end(); ++indIter)
    {
        // qDebug() << indIter.key() << " " << indIter.value().fieldNum << FN;

        extraInfoStream << indIter.key();
        extraInfoStream << indIter.value().fieldNum;
        extraInfoStream << indIter.value().indexSize;
        extraInfoStream << indIter.value().isSigned;
        extraInfoStream << indIter.value().functionID;
    }
}


int EgDataNodeTypeExtraInfo::LocalStoreExtraInfo()
{
    int res = 0;

    res = OpenLocalStoreStream();

    if (! res)
        SendExtraInfoToStream(localExtraInfoStream);

        // localMetaInfoStream << *this;

    extraInfoFile.close();

    return res;
}

int EgDataNodeTypeExtraInfo::ServerStoreExtraInfo()
{
    int res = 0;

    if (myECoGraphDB-> serverConnection)
        res = myECoGraphDB-> serverConnection->OpenStoreStream(opcode_store_metainfo, myECoGraphDB-> serverStream, typeName);

    if (! res)
        SendExtraInfoToStream(*(myECoGraphDB-> serverStream));

        // *serverStream << *this;

    if (! res)
        res = myECoGraphDB-> serverConnection-> WaitForSending();

    myECoGraphDB-> serverConnection-> Disconnect();

    return res;
}


int EgDataNodeTypeExtraInfo::ServerLoadExtraInfo()
{
    int res = 0;

    if (myECoGraphDB-> serverConnection)
        res = myECoGraphDB-> serverConnection-> OpenLoadStream(opcode_load_metainfo, myECoGraphDB-> serverStream, typeName);

    if (! res)
        res = myECoGraphDB-> serverConnection-> WaitForSending(); // command

    // qDebug << serverConnection-> tcpSocket.state() << "" << ; WaitForReadyRead

    if (! res)
        res = myECoGraphDB-> serverConnection-> WaitForReadyRead();

    if (! res)
        res = LoadExtraInfoFromStream(*(myECoGraphDB-> serverStream));

    myECoGraphDB-> serverConnection-> Disconnect();


    return res;
}

int EgDataNodeTypeExtraInfo::OpenLocalLoadStream()
{
        // open file
    extraInfoFile.setFileName("egdb/" + typeName + ".ddt");
    localExtraInfoStream.setDevice(&extraInfoFile);

    if (!extraInfoFile.open(QIODevice::ReadOnly))
    {
        // if (! typeName.contains(EgDataNodesNamespace::egGUIfileName))
        qDebug() << FN << "can't open " << typeName + ".ddt" << " file";

        return -1;
    }

    return 0;
}

int EgDataNodeTypeExtraInfo::LoadExtraInfoFromStream(QDataStream& extraInfoStream)
{
    // QList<QString> indexedFieldsLocal;

    Clear(); // init metainfo

    extraInfoStream >> nodesCount;  // data nodes (NOT field descriptors) count
    extraInfoStream >> nextNodeID;   // incremental counter

    extraInfoStream >> typeSettings.useEntryNodes;
    extraInfoStream >> typeSettings.useLocation;
    extraInfoStream >> typeSettings.useNamedAttributes;
    extraInfoStream >> typeSettings.useLinks;
    extraInfoStream >> typeSettings.useGUIsettings;

    extraInfoStream  >> dataFields;  // field descriptors
    // metaInfoStream  >> indexedFieldsLocal;

    quint32 theSize = 0;
    EgIndexSettings theSettings;
    QString theName;

    extraInfoStream >> theSize;

    for (quint32 i = 0; i < theSize; i++)
    {
        extraInfoStream  >> theName;
        extraInfoStream  >> theSettings.fieldNum;
        extraInfoStream  >> theSettings.indexSize;
        extraInfoStream  >> theSettings.isSigned;
        extraInfoStream  >> theSettings.functionID;

        // qDebug() << theName << " " << theSettings.fieldNum << FN;

        indexedFields.insert(theName, theSettings);
    }

    int order = 0;

        // QList<QString>::iterator
    for (auto stringListIter = dataFields.begin(); stringListIter != dataFields.end(); stringListIter++)
        nameToOrder.insert(*stringListIter, order++);

    return 0;
}

int EgDataNodeTypeExtraInfo::LocalLoadExtraInfo()
{
    int res = 0;

    res = OpenLocalLoadStream();

    if (! res)
        res = LoadExtraInfoFromStream(localExtraInfoStream);

    extraInfoFile.close();

    return res;
}

void EgDataNodeTypeExtraInfo::PrintExtraInfo()
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
QDataStream& operator << (QDataStream& metaInfoStream, EgDataNodeTypeExtraInfo &metaInfo)
{
    metaInfoStream << metaInfo.nodesCount;  // data nodes (NOT field descriptors) count
    metaInfoStream << metaInfo.nextNodeID;   // incremental counter

    metaInfoStream << metaInfo.typeSettings.useEntryNodes;
    metaInfoStream << metaInfo.typeSettings.useLocation;
    metaInfoStream << metaInfo.typeSettings.useNamedAttributes;
    metaInfoStream << metaInfo.typeSettings.useLinks;
    metaInfoStream << metaInfo.typeSettings.useGUIsettings;

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
QDataStream& operator >> (QDataStream& metaInfoStream, EgDataNodeTypeExtraInfo& metaInfo)
{
    // QList<QString> indexedFieldsLocal;

    metaInfo.Clear(); // init metainfo

    metaInfoStream >> metaInfo.nodesCount;  // data nodes (NOT field descriptors) count
    metaInfoStream >> metaInfo.nextNodeID;   // incremental counter

    metaInfoStream >> metaInfo.typeSettings.useEntryNodes;
    metaInfoStream >> metaInfo.typeSettings.useLocation;
    metaInfoStream >> metaInfo.typeSettings.useNamedAttributes;
    metaInfoStream >> metaInfo.typeSettings.useLinks;
    metaInfoStream >> metaInfo.typeSettings.useGUIsettings;

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

