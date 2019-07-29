/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egDataNode.h"
#include "egExtraInfo.h"

EgDataNode::EgDataNode(EgDataNodeTypeExtraInfo& a_extraInfo):
    extraInfo(&a_extraInfo)
{
    for (int i = 0; i < a_extraInfo.dataFields.count(); i++)
        dataFields << QVariant();

    if(dataNodeID) // DUMMY
        EG_LOG_STUB << FN;
}

EgDataNode::~EgDataNode()
{
    dataFields.clear();
    if (nodeLinks)
    {
        nodeLinks-> inLinks.clear();
        nodeLinks-> outLinks.clear();
        delete nodeLinks;
    }
}

void EgDataNode::clear()
{
    dataFields.clear();

    isAdded = false;

    if (extraInfo)
        for (int i = 0; i < extraInfo-> dataFields.count(); i++)
            dataFields << QVariant();

    if (nodeLinks)
    {
        nodeLinks-> inLinks.clear();
        nodeLinks-> outLinks.clear();
        delete nodeLinks;

        nodeLinks = nullptr;
    }

    dataFileOffset = 0;
    dataNodeID = 0;


}

QVariant& EgDataNode::operator [] (QString& fieldName)
{
    // static QVariant not_found("<Not found>"); // to return index error as QVariant.isNull()

    if (extraInfo)
        if (extraInfo-> nameToOrder.contains(fieldName))
            return dataFields[extraInfo-> nameToOrder[fieldName]];

    return egNotFound;
}

QVariant& EgDataNode::operator [] (const char* fieldCharName)
{
    QString fieldName(fieldCharName);
    // static QVariant not_found("<Not found>"); // to return index error as QVariant.isNull()

    if (extraInfo)
        if (extraInfo-> nameToOrder.contains(fieldName))
            return dataFields[extraInfo-> nameToOrder[fieldName]];

    return egNotFound;
}


QDataStream& operator << (QDataStream& dStream, EgDataNode& dNode)   // transfer and file operations
{
    dStream << dNode.dataNodeID;
    dStream << dNode.dataFileOffset;
    dStream << dNode.dataFields;

    return dStream;
}

QDataStream& operator >> (QDataStream& dStream, EgDataNode& dNode)         // unpack
{
    dStream >> dNode.dataNodeID;
    dStream >> dNode.dataFileOffset;
    dStream >> dNode.dataFields;

    return dStream;
}


/*
qint32 EgDataNode::DateToInt(QVariant& date_field)
{
    if (date_field.toDate().isNull() || !date_field.toDate().isValid())
        return 0;
    return date_field.toDate().year()*512 + date_field.toDate().month()*32 + date_field.toDate().day();
}

QVariant EgDataNode::IntToDate(qint32 date_int)
{
    QVariant my_date = QDate();
    int year, mon, day;

    if (!date_int)
        return my_date;

    year = date_int / 512;
    mon = (date_int % 512) / 32;
    day = date_int % 32;

    // qDebug() << "IntToDate():year mon day = " << year << " " << mon << " " << day;

    my_date = QVariant(QDate(year, mon, day));  // QDate ( int y, int m, int d ) Year*512 + Month*32 + Day

    return my_date;
}
*/
