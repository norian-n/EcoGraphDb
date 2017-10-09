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

int EgDataNodeTypeMetaInfo::LocalStoreMetaInfo()
{
    QDir dir;

    if (! dir.exists("egdb"))
        dir.mkdir("egdb");

        // open file
    QFile ddt_file("egdb/" + typeName + ".ddt");
    QDataStream dStream(&ddt_file);

    if (!ddt_file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << FN << "can't open " << typeName + ".ddt" << " file";        

        return -1;
    }

    dStream << nodesCount;  // data nodes (NOT field descriptors) count
    dStream << nextObjID;   // incremental counter

    dStream << useEntryNodes;
    dStream << useLocation;
    dStream << useNamedAttributes;
    dStream << useLinks;
    dStream << useGUIsettings;

    dStream << dataFields;  // field descriptors
    // dStream << indexedToOrder.keys();

    dStream << (quint32) indexedFields.size();

    for (auto indIter = indexedFields.begin(); indIter != indexedFields.end(); ++indIter)
    {
        // qDebug() << indIter.key() << " " << indIter.value().fieldNum << FN;

        dStream << indIter.key();
        dStream << indIter.value().fieldNum;
        dStream << indIter.value().indexSize;
        dStream << indIter.value().isSigned;
        dStream << indIter.value().functionID;
    }

    ddt_file.close();    

    return 0;
}

int EgDataNodeTypeMetaInfo::LocalLoadMetaInfo()
{
    QList<QString> indexedFieldsLocal;

    Clear(); // init metainfo

        // open file
    QFile ddt_file("egdb/" + typeName + ".ddt");
    QDataStream dStream(&ddt_file);

/*    if (!ddt_file.exists())
    {
        qDebug() << FN << "file doesn't exist " << typeName + ".ddt";
        return -1;
    }
*/

    if (!ddt_file.open(QIODevice::ReadOnly))
    {
        if (! typeName.contains(EgDataNodesNamespace::egGUIfileName))
            qDebug() << FN << "can't open " << typeName + ".ddt" << " file";
        return -1;
    }

    dStream >> nodesCount;  // data nodes (NOT field descriptors) count
    dStream >> nextObjID;   // incremental counter

    dStream >> useEntryNodes;
    dStream >> useLocation;
    dStream >> useNamedAttributes;
    dStream >> useLinks;
    dStream >> useGUIsettings;

    dStream  >> dataFields;  // field descriptors
    // dStream  >> indexedFieldsLocal;

    quint32 theSize = 0;
    EgIndexSettings theSettings;
    QString theName;

    dStream >> theSize;

    for (quint32 i = 0; i < theSize; i++)
    {
        dStream  >> theName;
        dStream  >> theSettings.fieldNum;
        dStream  >> theSettings.indexSize;
        dStream  >> theSettings.isSigned;
        dStream  >> theSettings.functionID;

        // qDebug() << theName << " " << theSettings.fieldNum << FN;

        indexedFields.insert(theName, theSettings);
    }

    int order = 0;

        // QList<QString>::iterator
    for (auto stringListIter = dataFields.begin(); stringListIter != dataFields.end(); stringListIter++)
        nameToOrder.insert(*stringListIter, order++);
/*
        // QList<QString>::iterator
    for (auto stringListIter = indexedFieldsLocal.begin(); stringListIter != indexedFieldsLocal.end(); stringListIter++)
        if (nameToOrder.contains(*stringListIter))
            indexedToOrder.insert(*stringListIter, nameToOrder[*stringListIter]);
            */

    ddt_file.close();

    return 0;
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
