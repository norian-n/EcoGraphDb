/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "egEntryNodes.h"
#include "egDataNodesType.h"

#include <QFile>

int EgEntryNodes::AddEntryNode(EgDataNodesType& egType, EgDataNode& entryNode)
{
    QFile dat_file;             // data file
    QDataStream dat;

    if (! entryNodes.contains(entryNode.dataNodeID))
    {
        entryNodes.insert(entryNode.dataNodeID, &entryNode);

        if (! dir.exists("egdb"))
        {
            qDebug()  << "Can't find the egdb dir " << FN;
            return -1;
        }

        dat_file.setFileName("egdb/" + egType.metaInfo.typeName + ".ent");

        if (!dat_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
        {
            qDebug() << "can't open file " << egType.metaInfo.typeName + ".ent" << FN;
            return -1;
        }

        dat.setDevice(&dat_file);

        dat.device()-> seek(dat.device()->size());

            // walk add list
        // for (QMap<EgDataNodeIDtype, EgDataNode*>::iterator Iter = entryNodes.begin(); Iter != entryNodes.end(); ++Iter)
        // {
            // qDebug() << "Adding link" << (int) addIter.key() << (int) addIter.value() << " on offset" << hex << (int) dat.device()-> pos() << FN;

        dat << entryNode.dataNodeID; // Iter.key();
            // dat << addIter.value();
        //}

        dat_file.close();
    }
    else
        return 1;

    return 0;
}

/*

int EgEntryNodes::StoreEntryNodes(EgDataNodesType& egType)
{
    QFile dat_file;             // data file
    QDataStream dat;

    if (entryNodes.empty())
        return 1;

    if (! dir.exists("egdb"))
    {
        qDebug()  << "can't find the egdb dir " << FN;
        return -1;
    }

    dat_file.setFileName("egdb/" + egType.metaInfo.typeName + ".ent");

    if (!dat_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "can't open file " << egType.metaInfo.typeName + ".ent" << FN;
        return -1;
    }

    dat.setDevice(&dat_file);

    dat.device()-> seek(0);

        // walk add list
    for (QMap<EgDataNodeIDtype, EgDataNode*>::iterator Iter = entryNodes.begin(); Iter != entryNodes.end(); ++Iter)
    {
        // qDebug() << "Adding link" << (int) addIter.key() << (int) addIter.value() << " on offset" << hex << (int) dat.device()-> pos() << FN;

        dat << Iter.key();
        // dat << addIter.value();
    }

    dat_file.close();

    return 0;
}
*/

int EgEntryNodes::LoadEntryNodes(EgDataNodesType& egType)
{
     QFile dat_file;             // data file
     QDataStream dat;

     EgDataNodeIDtype entryNodeID;

     if (! dir.exists("egdb"))
     {
         qDebug()  << "can't find the egdb dir " << FN;
         return -1;
     }

     dat_file.setFileName("egdb/" + egType.metaInfo.typeName + ".ent");

     if ( ! dat_file.exists())
     {
         // qDebug() << FN << "file doesn't exist " << IndexFileName + ".odx";
         return 1;
     }

     if (!dat_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
     {
         qDebug() << "can't open file " << "egdb/" + egType.metaInfo.typeName + ".ent" << FN;
         return -1;
     }

     dat.setDevice(&dat_file);

     while(! dat.atEnd())
     {
         dat >> entryNodeID;

         if (egType.dataNodes.contains(entryNodeID))
             entryNodes.insert(entryNodeID, &(egType.dataNodes[entryNodeID]));
         else
         {
             // qDebug()  << "Entry node ID not found in egType.dataNodes " << entryNodeID << FN;
             // qDebug()  << egType.dataNodes.keys() << FN;

             continue;
         }
     }

     dat_file.close();

     return 0;
}

