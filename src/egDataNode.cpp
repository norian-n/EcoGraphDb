/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "egDataNode.h"
#include "egMetaInfo.h"

EgDataNode::EgDataNode(EgDataNodeTypeMetaInfo& a_metaInfo):
    dataNodeID(0)
  , dataFileOffset(0)
  , isAdded(false)
  , metaInfo(&a_metaInfo)
  , nodeLinks(NULL)
{
    for (int i = 0; i < a_metaInfo.dataFields.count(); i++)
        dataFields << QVariant();
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

    if (metaInfo)
        for (int i = 0; i < metaInfo-> dataFields.count(); i++)
            dataFields << QVariant();

    if (nodeLinks)
    {
        nodeLinks-> inLinks.clear();
        nodeLinks-> outLinks.clear();
        delete nodeLinks;

        nodeLinks = NULL;
    }

    dataFileOffset = 0;
    dataNodeID = 0;
}

QVariant& EgDataNode::operator [] (QString& fieldName)
{
    // static QVariant not_found("<Not found>"); // to return index error as QVariant.isNull()

    if (metaInfo)
        if (metaInfo-> nameToOrder.contains(fieldName))
            return dataFields[metaInfo-> nameToOrder[fieldName]];

    return egNotFound;
}

QVariant& EgDataNode::operator [] (const char* fName)
{
    QString fieldName(fName);
    // static QVariant not_found("<Not found>"); // to return index error as QVariant.isNull()

    if (metaInfo)
        if (metaInfo-> nameToOrder.contains(fieldName))
            return dataFields[metaInfo-> nameToOrder[fieldName]];

    return egNotFound;
}


QDataStream& operator << (QDataStream& d_stream, EgDataNode& d_object)   // transfer and file operations
{
    d_stream << d_object.dataFields;     // write to stream

    return d_stream;
}

QDataStream& operator >> (QDataStream& d_stream, EgDataNode& d_object)         // unpack
{
    d_stream >> d_object.dataFields;

    return d_stream;
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
