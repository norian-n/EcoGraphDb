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
        // indexedFields.append(fieldName);
        indexedToOrder.insert(fieldName, order);
    }
}

int EgDataNodeTypeMetaInfo::LocalStoreMetaInfo()
{
        // open file
    QFile ddt_file(typeName + ".ddt");
    QDataStream dStream(&ddt_file);
    if (!ddt_file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << FN << "can't open " << typeName + ".ddt" << " file";
        return -1;
    }

    dStream << objCount;  // data objects (NOT field descriptors) count
    dStream << nextObjID; // next_obj_id

    dStream << dataFields;
    dStream << indexedToOrder.keys();

    ddt_file.close();

    return 0;
}

int EgDataNodeTypeMetaInfo::LocalLoadMetaInfo()
{
    QList<QString> indexedFields;

    QList<QString>::iterator stringListIter;
    int order = 0;

        // open file
    QFile ddt_file(typeName + ".ddt");
    QDataStream dStream(&ddt_file);
    if (!ddt_file.open(QIODevice::ReadOnly))
    {
        if (! typeName.contains(EgDataNodesGUInamespace::egGUIfileName))
            qDebug() << FN << "can't open " << typeName + ".ddt" << " file";
        return -1;
    }

    Clear();

    dStream >> objCount;  // data objects (NOT field descriptors) count
    dStream >> nextObjID; // next_obj_id

    dStream  >> dataFields;
    dStream  >> indexedFields;

    for (stringListIter = dataFields.begin(); stringListIter != dataFields.end(); stringListIter++)
        nameToOrder.insert(*stringListIter, order++);

    for (stringListIter = indexedFields.begin(); stringListIter != indexedFields.end(); stringListIter++)
        if (nameToOrder.contains(*stringListIter))
            indexedToOrder.insert(*stringListIter, nameToOrder[*stringListIter]);

    ddt_file.close();

    return 0;
}

void EgDataNodeTypeMetaInfo::PrintMetaInfo()
{
     qDebug() << FN << "\nType name:" << typeName << " Obj Count:" << objCount << " Next ID:" << nextObjID;

     qDebug() << "Fields:";
     qDebug() << dataFields;

     qDebug() << "Fields order:";
     for (QHash<QString, int> ::iterator curDesc = nameToOrder.begin(); curDesc != nameToOrder.end(); ++curDesc)
         qDebug() << curDesc.value() << " " << curDesc.key();

     // qDebug() << "Indexes order:";
     // qDebug() << indexedFields;

     qDebug() << "Indexes order:";
     for (QHash<QString, int> ::iterator curDesc = indexedToOrder.begin(); curDesc != indexedToOrder.end(); ++curDesc)
         qDebug() << curDesc.value() << " " << curDesc.key();
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
