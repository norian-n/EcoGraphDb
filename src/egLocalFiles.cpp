/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egLocalFiles.h"
#include "egDataNodesType.h"

EgDataFiles::~EgDataFiles()
{
        // clean up index files control
    // QHash<QString, EgIndexFiles<qint32>*>::iterator indexesIter;

    for (auto indexesIter = indexFiles.begin(); indexesIter != indexFiles.end(); ++indexesIter)
        if (indexesIter.value())
            delete indexesIter.value();

    indexFiles.clear();

}

bool EgDataFiles::CheckMetaInfoFile()
{
    if (! metaInfo)
    {
        qDebug()  << "metaInfo is not set properly " << FN;

        return false;
    }

    if (! dir.exists("egdb/" + metaInfo-> typeName + ".ddt"))
    {
        // qDebug()  << "can't find the egdb dir " << FN;
        return false;
    }

    // FIXME TODO validate content

    return true;
}

int EgDataFiles::Init(EgDataNodeTypeMetaInfo& a_metaInfo)
{
    metaInfo = &a_metaInfo;

    primIndexFiles = new EgIndexFiles<qint32>(a_metaInfo.typeName + "_odb_pit"); // FIXME no literal

        // clean up QHash<QString, EgIndexFilesBase*>::iterator
    for (auto indexesIter = indexFiles.begin(); indexesIter != indexFiles.end(); ++indexesIter)
        if (indexesIter.value())
            delete indexesIter.value();

    indexFiles.clear();

    // qDebug() << FN << "metaInfo-> indexedToOrder.count() =" << metaInfo-> indexedToOrder.count();

        // add indexes files classes QHash<QString, EgIndexSettings> ::iterator
    // for (auto metaInfoIter = metaInfo-> indexedToOrder.begin(); metaInfoIter != metaInfo-> indexedToOrder.end(); ++metaInfoIter)
    EgIndexFilesBase* newIndexFiles = nullptr;

    for (auto metaInfoIter = metaInfo-> indexedFields.begin(); metaInfoIter != metaInfo-> indexedFields.end(); ++metaInfoIter)
    {

        switch (metaInfoIter.value().indexSize) {

        case 1:
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<float>(a_metaInfo.typeName + "_" + metaInfoIter.key()));
            break;

        case 2:
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<double>(a_metaInfo.typeName + "_" + metaInfoIter.key()));
            break;

        case 32:
            if (metaInfoIter.value().isSigned)
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<qint32>(a_metaInfo.typeName + "_" + metaInfoIter.key()));
            else
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<quint32>(a_metaInfo.typeName + "_" + metaInfoIter.key()));
            break;

        case 64:
            if (metaInfoIter.value().isSigned)
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<qint64>(a_metaInfo.typeName + "_" + metaInfoIter.key()));
            else
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<quint64>(a_metaInfo.typeName + "_" + metaInfoIter.key()));
            break;

        default:
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<qint32>(a_metaInfo.typeName + "_" + metaInfoIter.key()));
            break;
        }


        // qDebug() << FN << indIter.key() << newIndexFiles;

        // newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<qint32>(a_metaInfo.typeName + "_" + metaInfoIter.key()));

        if (newIndexFiles)
            indexFiles.insert(metaInfoIter.key(),  newIndexFiles);
    }

        // check if files exists
/*
    int res = LocalOpenFiles(); // FIXME just check if exist + indexes

    if (! res)
        LocalCloseFiles();
*/

    return 0; //res;

    // qDebug() << FN << "indexFiles.count() =" << indexFiles.count();
}

/*
inline void EgDataFiles::AppendNewData(QDataStream& dat, QList<EgPackedDataNode*>& a_list)
{
    QList<EgPackedDataNode*>::iterator cur_obj;
        // append to data file
    dat.device()-> seek(dat.device()-> size());
    // qDebug() << FN << "a_list.count() =" << a_list.count();
        // walk add list
    cur_obj = a_list.begin();
    while (cur_obj != a_list.end())
    {
        // qDebug() << FN << "Adding object" << (int)(*cur_obj)-> OBJ_ID << " on offset" << hex << (int) dat.device()-> pos();
        (*cur_obj)-> newOffset = dat.device()-> pos();    // save offset

            // write to data file
        dat << (*cur_obj)-> OBJ_ID; // (EgDataNodeIDtype)
        dat << (*cur_obj)-> bar;    // (QByteArray&)

        cur_obj++;
    }
}
*/

inline int EgDataFiles::LocalOpenFilesToRead()
{
    if (! dir.setCurrent("egdb"))
    {
        qDebug()  << "can't find the egdb dir " << FN;
        return -1;
    }
        // meta info file
    ddt_file.setFileName(metaInfo-> typeName + ".ddt");

    if (! ddt_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "can't open file " << metaInfo-> typeName + ".ddt" << FN;
        return -1;
    }

    ddt.setDevice(&ddt_file);

        // data nodes file
    dat_file.setFileName(metaInfo-> typeName + ".dat");

    if (! dat_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "can't open file " << metaInfo-> typeName + ".dat" << FN;
        return -1;
    }

    dat.setDevice(&dat_file);

        // primary index

    // QString IndexFileName = metaInfo-> typeName + "_odb_pit"; // FIXME const
    primIndexFiles-> OpenIndexFilesToRead();

        // other indexes files QHash<QString, EgIndexFiles<qint32>*>::iterator
    for (auto indexesIter = indexFiles.begin(); indexesIter != indexFiles.end(); ++indexesIter)
    {
        // IndexFileName = metaInfo-> typeName + "_" + indexesIter.key();
        indexesIter.value()-> OpenIndexFilesToRead();
    }

    return 0;
}

inline int EgDataFiles::LocalOpenFilesToUpdate()
{   
    if (! dir.exists("egdb"))
        dir.mkdir("egdb");

    dir.setCurrent("egdb");

        // meta info file
    ddt_file.setFileName(metaInfo-> typeName + ".ddt");

    if (!ddt_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open file " << metaInfo-> typeName + ".ddt";
        return -1;
    }

    ddt.setDevice(&ddt_file);

        // data nodes file
    dat_file.setFileName(metaInfo-> typeName + ".dat");

    if (!dat_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open file " << metaInfo-> typeName + ".dat";
        return -1;
    }

    dat.setDevice(&dat_file);

        // primary index

    // QString IndexFileName = metaInfo-> typeName + "_odb_pit"; // FIXME const
    primIndexFiles-> OpenIndexFilesToUpdate();

        // other indexes files QHash<QString, EgIndexFiles<qint32>*>::iterator
    for (auto indexesIter = indexFiles.begin(); indexesIter != indexFiles.end(); ++indexesIter)
    {
        // IndexFileName = metaInfo-> typeName + "_" + indexesIter.key();
        indexesIter.value()-> OpenIndexFilesToUpdate();
    }

    return 0;
}

inline void EgDataFiles::LocalCloseFiles()
{
    ddt_file.close();
    dat_file.close();

    primIndexFiles-> CloseIndexFiles();

        // other indexes files QHash<QString, EgIndexFiles<qint32>*>::iterator
    for (auto indexesIter = indexFiles.begin(); indexesIter != indexFiles.end(); ++indexesIter)
        indexesIter.value()-> CloseIndexFiles();

    dir.setCurrent("..");
}

/*
int EgDataFiles::LocalStoreFieldDesc(QByteArray* field_descs, QByteArray* control_descs)
{
        // open file
    QFile ddt_file(metaInfo-> typeName + ".ddt");
    QDataStream ddt(&ddt_file);
    if (!ddt_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open " << metaInfo-> typeName + ".ddt" << " file";
        return -1;
    }

    ddt << (EgDataNodeIDtype) 0; // obj_count;     // data objects (NOT field descriptors) count
    ddt << (EgDataNodeIDtype) 1; // next_obj_id;   // next_obj_id

    ddt << *field_descs;                // packed field descriptors
    ddt << *control_descs;              // packed control descriptors

    ddt_file.close();
    return 0;
}

int EgDataFiles::LocalLoadFieldDesc(QByteArray* field_descs, QByteArray* control_descs, EgDataNodeIDtype& obj_count, EgDataNodeIDtype& next_obj_id)
{
        // open file
    QFile ddt_file(metaInfo-> typeName + ".ddt");
    QDataStream ddt(&ddt_file);
    if (!ddt_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open " << metaInfo-> typeName + ".ddt" << " file";
        return -1;
    }

    field_descs->clear();
    control_descs->clear();

    ddt >> obj_count;       // data objects (NOT field descriptors) count
    ddt >> next_obj_id;     // next_obj_id

    ddt >> *field_descs;    // packed field descriptors
    ddt >> *control_descs;  // packed control descriptors

    ddt_file.close();
    return 0;
}
*/

int EgDataFiles::RemoveLocalFiles()
{
        // FIXME check dir
    return (int)(ddt_file.remove() && dat_file.remove());
}

int EgDataFiles::LocalLoadData(QSet<quint64>& dataOffsets, QMap<EgDataNodeIDtype, EgDataNode>& dataNodesMap)
{
    EgDataNode tmpNode2;
    int retCode = 0;

    tmpNode2.metaInfo = metaInfo;

    dat_file.setFileName("egdb/" + metaInfo-> typeName + ".dat");

    if (!dat_file.exists())
    {
        qDebug() << FN << "file doesn't exist' " << metaInfo-> typeName << ".dat";
        return -1;
    }

    if (!dat_file.open(QIODevice::ReadOnly))
    {
        qDebug() << FN << "can't open file for read " << metaInfo-> typeName << ".dat";
        return -1;
    }

    dat.setDevice(&dat_file);

    // qDebug() << FN << "Opened file for read " << metaInfo-> typeName << ".dat";

        // read data to dataNodesMap by dataOffsets
    for (QSet<quint64>::iterator offsets_iter = dataOffsets.begin(); offsets_iter != dataOffsets.end(); ++offsets_iter)
    {
        // qDebug() << FN <<  "Seek data file, pos =" << hex << (int) *offsets_iter;

            // go to rec position
        if (! dat.device()-> seek(*offsets_iter)) // move to data
        {
            qDebug() << FN <<  "Can't seek data file, pos =" << hex << (int) *offsets_iter;
            retCode = -2;
            continue;
            // dat_file.close();
            // return -2;
        }
            // read data
        tmpNode2.dataFields.clear();
        dat >> tmpNode2.dataNodeID;                
        dat >> tmpNode2;

        tmpNode2.dataFileOffset = *offsets_iter;

        // qDebug() << FN <<  "tmpNode2.dataNodeID =" << tmpNode2.dataNodeID;

        // TODO FIXME : implement local filter here


        if (FilterCallback)
            if (! FilterCallback(tmpNode2, filterValues))
                continue;

        dataNodesMap.insert(tmpNode2.dataNodeID, tmpNode2);
    }

    // qDebug() << FN <<  "dataNodesMap.count() =" << dataNodesMap.count();

    dat_file.close();

    return retCode;
}

int EgDataFiles::LocalStoreData(QMap<EgDataNodeIDtype, EgDataNode*>&  addedDataNodes, QMap<EgDataNodeIDtype, EgDataNode>& deletedDataNodes, QMap<EgDataNodeIDtype, EgDataNode*>&  updatedDataNodes)
{
    EgDataNodeIDtype obj_count;      // FIXME : thread safe solution for next_obj_id;
    EgDataNodeIDtype next_obj_id;    // FIXME : thread safe solution

        // check empty lists
    if (addedDataNodes.isEmpty() && deletedDataNodes.isEmpty() && updatedDataNodes.isEmpty()) // all are empty
    {
        return 0;
    }
        // TODO lock table

    if (LocalOpenFilesToUpdate())
        return -1;

        // update ObjDb metadata (data definition table)
    if (ddt.device()->size() > 0)
    {
        ddt >> obj_count;
        ddt >> next_obj_id;
        obj_count += addedDataNodes.count() - deletedDataNodes.count();
        next_obj_id += addedDataNodes.count(); // FIXME : thread-safe solution for server
            // update
        ddt.device()->seek(0);
        ddt << (EgDataNodeIDtype) obj_count;
        ddt << (EgDataNodeIDtype) next_obj_id;
    }
    else
    {
        qDebug() << FN << "Error: ddt file is empty: " << metaInfo-> typeName + ".ddt";
    }

        // process deleted objects
    if (! deletedDataNodes.isEmpty())
        LocalDeleteObjects(deletedDataNodes);

        // process updated objects
    if (! updatedDataNodes.isEmpty())
        LocalModifyObjects(dat, updatedDataNodes);

        // add new records
    if (! addedDataNodes.isEmpty())
        LocalAddObjects(dat, addedDataNodes);

        // close files
    LocalCloseFiles();

        // TODO : unlock table

    return 0;
}

inline void EgDataFiles::LocalAddObjects(QDataStream& dat, QMap<EgDataNodeIDtype, EgDataNode*>&  addedDataNodes)
{
    dat.device()-> seek(dat.device()-> size());

        // open index files
    /*
    primIndexFiles-> OpenIndexFilesToUpdate();

    for (QHash<QString, int> ::iterator indIter = metaInfo-> indexedToOrder.begin(); indIter != metaInfo-> indexedToOrder.end(); ++indIter)
        if (indexFiles.contains(indIter.key()))
            indexFiles[indIter.key()]-> OpenIndexFilesToUpdate();
        else
            qDebug() << FN << "Index not found: " << indIter.key();

    */


        // walk add list
    for (QMap<EgDataNodeIDtype, EgDataNode*>::iterator addIter = addedDataNodes.begin(); addIter != addedDataNodes.end(); ++addIter)
    {
        // qDebug() << FN << "Adding object" << (int) addIter.value()-> dataNodeID << " on offset" << hex << (int) dat.device()-> pos();

        primIndexFiles-> dataOffset = dat.device()-> pos();
        addIter.value()-> dataFileOffset = primIndexFiles-> dataOffset;    // save offset;

        dat << addIter.value()-> dataNodeID;
        dat << *(addIter.value());

            // add primary index

        primIndexFiles-> theIndex = addIter.value()-> dataNodeID;
        primIndexFiles-> AddIndex();

            // add other indexes
        // for (QHash<QString, int> ::iterator indIter = metaInfo-> indexedToOrder.begin(); indIter != metaInfo-> indexedToOrder.end(); ++indIter)
        for (auto indIter = metaInfo-> indexedFields.begin(); indIter != metaInfo-> indexedFields.end(); ++indIter)
        {
            if (indexFiles.contains(indIter.key()))
            {
                indexFiles[indIter.key()]-> setIndex(addIter.value()-> dataFields[indIter.value().fieldNum]); // TODO - calc index of QVariant
                indexFiles[indIter.key()]-> setDataOffset(primIndexFiles-> dataOffset);
                indexFiles[indIter.key()]-> AddIndex();
            }
            else
                qDebug() << FN << "Index not found: " << indIter.key();

        }
    }
}


inline void EgDataFiles::LocalDeleteObjects(QMap<EgDataNodeIDtype, EgDataNode>& deletedDataNodes)
{
    for (QMap<EgDataNodeIDtype, EgDataNode>::iterator delIter = deletedDataNodes.begin(); delIter != deletedDataNodes.end(); ++delIter)
    {
        // qDebug() << FN << "Del object ID = " << (int) delIter.value().dataNodeID << " with offset" << hex << (int) delIter.value().dataFileOffset;

            // del primary index
        primIndexFiles-> theIndex = delIter.value().dataNodeID;
        primIndexFiles-> dataOffset = delIter.value().dataFileOffset;
        primIndexFiles-> DeleteIndex();   // SIDE estimate old offset stored here

            // del other indexes
        // for (QHash<QString, int> ::iterator indIter = metaInfo-> indexedToOrder.begin(); indIter != metaInfo-> indexedToOrder.end(); ++indIter)
        for (auto indIter = metaInfo-> indexedFields.begin(); indIter != metaInfo-> indexedFields.end(); ++indIter)
        {
            if (indexFiles.contains(indIter.key()))
            {
                indexFiles[indIter.key()]-> setIndex(delIter.value().dataFields[indIter.value().fieldNum]); // TODO - calc index of QVariant
                indexFiles[indIter.key()]-> setDataOffset(primIndexFiles-> dataOffset);
                indexFiles[indIter.key()]-> DeleteIndex();
            }
            else
                qDebug() << FN << "Index not found: " << indIter.key();

        }
    }
}

inline int EgDataFiles::LocalModifyObjects(QDataStream& dat, QMap<EgDataNodeIDtype, EgDataNode*>&  updatedDataNodes)
{
    EgDataNode tmpNode;

        // walk updated nodes list    QMap<EgDataNodeIDtype, EgDataNode*>::iterator
    for (auto addIter = updatedDataNodes.begin(); addIter != updatedDataNodes.end(); ++addIter)
    {
        dat.device()-> seek(dat.device()-> size());

            // primary index data offset
        primIndexFiles-> newOffset = dat.device()-> pos();

        // qDebug()  << "Update object" << addIter.value()-> dataNodeID << " old offset" << hex << addIter.value()-> dataFileOffset
        //           << " new offset " << hex << primIndexFiles-> newOffset << FN;

        dat << addIter.value()-> dataNodeID;
        dat << *(addIter.value());

            // add primary index
        primIndexFiles-> theIndex = addIter.value()-> dataNodeID;
        primIndexFiles-> dataOffset = addIter.value()-> dataFileOffset;
        primIndexFiles-> UpdateIndex(false); // false means primary ID is not changed

        addIter.value()-> dataFileOffset = primIndexFiles-> newOffset;

        if (! metaInfo-> indexedFields.isEmpty())
        {
                // save old field values
            if (! dat.device()-> seek(primIndexFiles-> dataOffset))
            {
                qDebug() << FN << "can't seek to data offset" << hex << (int) primIndexFiles-> dataOffset;
                return -1;
            }

            // tmpNode.clear();

            dat >> tmpNode.dataNodeID;
            dat >> tmpNode.dataFields;

                // process other indexes QHash<QString, int> ::iterator
            // for (auto indIter = metaInfo-> indexedToOrder.begin(); indIter != metaInfo-> indexedToOrder.end(); ++indIter)
            for (auto indIter = metaInfo-> indexedFields.begin(); indIter != metaInfo-> indexedFields.end(); ++indIter)
            {
                if (indexFiles.contains(indIter.key()))
                {
                    // qDebug()  << "Update index from " << tmpNode.dataFields  // << tmpNode.dataFields[indIter.value()].toInt()
                    //          << " to " << addIter.value()-> dataFields[indIter.value()].toInt() << FN;

                    indexFiles[indIter.key()]-> setIndex(tmpNode.dataFields[indIter.value().fieldNum]); // TODO - calc index of QVariant
                    indexFiles[indIter.key()]-> setDataOffset(primIndexFiles-> dataOffset);


                    bool isUpdated = tmpNode.dataFields[indIter.value().fieldNum] != addIter.value()-> dataFields[indIter.value().fieldNum];

                    indexFiles[indIter.key()]-> setNewOffset(primIndexFiles-> newOffset);
                    indexFiles[indIter.key()]-> setNewIndex(addIter.value()-> dataFields[indIter.value().fieldNum]);
                    indexFiles[indIter.key()]-> UpdateIndex(isUpdated);

                    /*
                    if (tmpNode.dataFields[indIter.value()].toInt() != addIter.value()-> dataFields[indIter.value()].toInt())
                    {
                        // delete/add index
                        indexFiles[indIter.key()]-> DeleteIndex();

                        indexFiles[indIter.key()]-> theIndex   = addIter.value()-> dataFields[indIter.value()].toInt();
                        indexFiles[indIter.key()]-> dataOffset = primIndexFiles-> newOffset;
                        indexFiles[indIter.key()]-> AddObjToIndex();
                    }
                    else
                    {
                        // update
                        indexFiles[indIter.key()]-> newOffset = primIndexFiles-> newOffset;
                        indexFiles[indIter.key()]-> UpdateIndex(false);
                    }
                    */

                    // indexFiles[indIter.key()]-> UpdateIndex(false);
                }
                else
                    qDebug() << "Error: Index not found: " << indIter.key() << FN;

            }
        }
    }

    return 0;
}

int EgDataFiles::LocalCompressData() // FIXME remove deleted records TODO : optional remove to archive
{
    // FIXME

    return 0;
}
