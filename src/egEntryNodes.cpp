/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egEntryNodes.h"
#include "egDataNodesType.h"

#include <QFile>

EgEntryNodes::EgEntryNodes(EgDataNodesType* theNodesType):
    nodesType(theNodesType)
  , entryStorage(new EgDataNodesType())
{
    //  metaInfo.myECoGraphDB

    entryStorage-> extraInfo.typeSettings.useEntryNodes = false; // no recursion
}

EgEntryNodes::~EgEntryNodes()
{
    if (entryStorage)
        delete entryStorage;
}

int EgEntryNodes::AddEntryNode (EgDataNodeIdType nodeID)
{
    QList<QVariant> myData;

    myData << nodeID;

    entryStorage-> AddHardLinked(myData, nodeID);

    entryStorage-> StoreData();

    // EG_LOG_STUB << "nodeID added " << nodeID << FN;

    return 0;
}

int EgEntryNodes::DeleteEntryNode (EgDataNodeIdType nodeID)
{
    entryStorage-> DeleteDataNode(nodeID);

    entryStorage-> StoreData();

    return 0;
}

int EgEntryNodes::LoadEntryNodes()
{
        // FIXME check if connected

    entryStorage-> LoadAllDataNodes();

    for (auto dataNodeIter = entryStorage-> dataNodes.begin(); dataNodeIter != entryStorage-> dataNodes.end(); ++dataNodeIter)
    {
        if (nodesType->dataNodes.contains(dataNodeIter.key()))
            entryNodesList.append(dataNodeIter.key());
        else
            EG_LOG_STUB  << "Can't find the data node ID of entry: " << dataNodeIter.key() << FN;
    }

    return 0;
}

int EgEntryNodes::StoreEntryNodes()
{
    entryStorage-> StoreData();

    return 0;
}
/*
int EgEntryNodes::AddEntryNode(EgDataNodesType& egType, EgDataNode& entryNode)
{
    QFile dat_file;             // data file
    QDataStream dat;

    if (! entryNodesMap.contains(entryNode.dataNodeID))
    {
        entryNodesMap.insert(entryNode.dataNodeID, &entryNode);

        if (! dir.exists("egdb"))
        {
            EG_LOG_STUB  << "Can't find the egdb dir " << FN;
            return -1;
        }

        dat_file.setFileName("egdb/" + egType.metaInfo.typeName + ".ent");

        if (!dat_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
        {
            EG_LOG_STUB << "can't open file " << egType.metaInfo.typeName + ".ent" << FN;
            return -1;
        }

        dat.setDevice(&dat_file);

        dat.device()-> seek(dat.device()->size());

            // walk add list
        // for (QMap<EgDataNodeIDtype, EgDataNode*>::iterator Iter = entryNodes.begin(); Iter != entryNodes.end(); ++Iter)
        // {
            // EG_LOG_STUB << "Adding link" << (int) addIter.key() << (int) addIter.value() << " on offset" << hex << (int) dat.device()-> pos() << FN;

        dat << entryNode.dataNodeID; // Iter.key();
            // dat << addIter.value();
        //}

        dat_file.close();
    }
    else
        return 1;

    return 0;
}
*/

/*

int EgEntryNodes::StoreEntryNodes(EgDataNodesType& egType)
{
    QFile dat_file;             // data file
    QDataStream dat;

    if (entryNodes.empty())
        return 1;

    if (! dir.exists("egdb"))
    {
        EG_LOG_STUB  << "can't find the egdb dir " << FN;
        return -1;
    }

    dat_file.setFileName("egdb/" + egType.metaInfo.typeName + ".ent");

    if (!dat_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) // WriteOnly Append | QIODevice::Truncate
    {
        EG_LOG_STUB << "can't open file " << egType.metaInfo.typeName + ".ent" << FN;
        return -1;
    }

    dat.setDevice(&dat_file);

    dat.device()-> seek(0);

        // walk add list
    for (QMap<EgDataNodeIDtype, EgDataNode*>::iterator Iter = entryNodes.begin(); Iter != entryNodes.end(); ++Iter)
    {
        // EG_LOG_STUB << "Adding link" << (int) addIter.key() << (int) addIter.value() << " on offset" << hex << (int) dat.device()-> pos() << FN;

        dat << Iter.key();
        // dat << addIter.value();
    }

    dat_file.close();

    return 0;
}
*/

/*

int EgEntryNodes::LoadEntryNodes(EgDataNodesType& egType)
{
     QFile dat_file;             // data file
     QDataStream dat;

     EgDataNodeIDtype entryNodeID;

     if (! dir.exists("egdb"))
     {
         EG_LOG_STUB  << "can't find the egdb dir " << FN;
         return -1;
     }

     dat_file.setFileName("egdb/" + egType.metaInfo.typeName + ".ent");

     if ( ! dat_file.exists())
     {
         // EG_LOG_STUB << FN << "file doesn't exist " << IndexFileName + ".odx";
         return 1;
     }

     if (!dat_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
     {
         EG_LOG_STUB << "can't open file " << "egdb/" + egType.metaInfo.typeName + ".ent" << FN;
         return -1;
     }

     dat.setDevice(&dat_file);

     while(! dat.atEnd())
     {
         dat >> entryNodeID;

         if (egType.dataNodes.contains(entryNodeID))
             entryNodesMap.insert(entryNodeID, &(egType.dataNodes[entryNodeID]));
         else
         {
             // EG_LOG_STUB  << "Entry node ID not found in egType.dataNodes " << entryNodeID << FN;
             // EG_LOG_STUB  << egType.dataNodes.keys() << FN;

             continue;
         }
     }

     dat_file.close();

     return 0;
}
*/
