/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egMetaInfo.h"
#include "egDataNodesType.h"

#include <QtDebug>

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
    QDir dir;

    if (! dir.exists("egdb"))
        dir.mkdir("egdb");

        // open file
    metaInfoFile.setFileName("egdb/" + typeName + ".ddt");
    localMetaInfoStream.setDevice(&metaInfoFile);

    if (!metaInfoFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << FN << "can't open " << typeName + ".ddt" << " file";

        return -1;
    }

    return 0;
}

int  EgDataNodeTypeMetaInfo::SendMetaInfoToStream(QDataStream& metaInfoStream)
{
    metaInfoStream << nodesCount;  // data nodes (NOT field descriptors) count
    metaInfoStream << nextObjID;   // incremental counter

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

    return 0;
}


int EgDataNodeTypeMetaInfo::LocalStoreMetaInfo()
{
    int res = 0;

    // if (myECoGraphDB && myECoGraphDB->connection)
      // res = client.OpenMetaInfoStoreStream();
    res = OpenLocalStoreStream();

    if (! res)
        res = SendMetaInfoToStream(localMetaInfoStream);

    metaInfoFile.close();

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
    metaInfoStream >> nextObjID;   // incremental counter

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

    // if (myECoGraphDB && myECoGraphDB->connection)
    res = OpenLocalLoadStream();

    if (! res)
        res = LoadMetaInfoFromStream(localMetaInfoStream);

    metaInfoFile.close();

    return res;
}

void EgDataNodeTypeMetaInfo::PrintMetaInfo()
{
     qDebug() << FN << "\nType name:" << typeName << " Obj Count:" << nodesCount << " Next ID:" << nextObjID;

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

/*

// store meta info
QDataStream& operator << (QDataStream& dStream, EgDataNodeTypeMetaInfo &metaInfo)
// QByteArray& operator << (QByteArray& packedMetaInfo, EgDataNodeTypeMetaInfo& metaInfo)
{
    dStream << metaInfo.dataFields;
    dStream << metaInfo.indexedToOrder.keys();

    return dStream;
}

// load meta info
QDataStream& operator >> (QDataStream& dStream, EgDataNodeTypeMetaInfo& metaInfo)
{

    return dStream;
}

*/
