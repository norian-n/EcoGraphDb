/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
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
    QDir dir(QDir::current());

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

int EgDataFiles::Init(EgDataNodeTypeExtraInfo& an_extraInfo)
{
    metaInfo = &an_extraInfo;

    primIndexFiles = new EgIndexFiles<qint32>(an_extraInfo.typeName + "_odb_pit"); // FIXME no literal

        // clean up QHash<QString, EgIndexFilesBase*>::iterator
    for (auto indexesIter = indexFiles.begin(); indexesIter != indexFiles.end(); ++indexesIter)
        if (indexesIter.value())
            delete indexesIter.value();

    indexFiles.clear();

    EgIndexFilesBase* newIndexFiles = nullptr;

    for (auto metaInfoIter = metaInfo-> indexedFields.begin(); metaInfoIter != metaInfo-> indexedFields.end(); ++metaInfoIter)
    {

        switch (metaInfoIter.value().indexSize) {

        case 1:
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<float>(an_extraInfo.typeName + "_" + metaInfoIter.key()));
            break;

        case 2:
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<double>(an_extraInfo.typeName + "_" + metaInfoIter.key()));
            break;

        case 32:
            if (metaInfoIter.value().isSigned)
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<qint32>(an_extraInfo.typeName + "_" + metaInfoIter.key()));
            else
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<quint32>(an_extraInfo.typeName + "_" + metaInfoIter.key()));
            break;

        case 64:
            if (metaInfoIter.value().isSigned)
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<qint64>(an_extraInfo.typeName + "_" + metaInfoIter.key()));
            else
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<quint64>(an_extraInfo.typeName + "_" + metaInfoIter.key()));
            break;

        default:
                newIndexFiles = static_cast<EgIndexFilesBase*> (new EgIndexFiles<qint32>(an_extraInfo.typeName + "_" + metaInfoIter.key()));
            break;
        }

        if (newIndexFiles)
            indexFiles.insert(metaInfoIter.key(),  newIndexFiles);
    }

    return 0; //res;

    // qDebug() << FN << "indexFiles.count() =" << indexFiles.count();
}

inline int EgDataFiles::LocalOpenFilesToRead()
{
    QDir dir(QDir::current());

    if (! dir.exists("egdb"))
    {
        qDebug()  << "can't find the egdb dir " << FN;
        return -1;
    }
        // meta info file
    ddt_file.setFileName("egdb/" + metaInfo-> typeName + ".ddt");

    if (! ddt_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "can't open file " << metaInfo-> typeName + ".ddt" << FN;
        return -1;
    }

    ddt.setDevice(&ddt_file);

        // data nodes file
    dat_file.setFileName("egdb/" + metaInfo-> typeName + ".dat");

    if (! dat_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "can't open file " << metaInfo-> typeName + ".dat" << FN;
        return -1;
    }

    dat.setDevice(&dat_file);

        // primary index

    // QString IndexFileName = metaInfo-> typeName + "_odb_pit"; // FIXME const
    primIndexFiles-> OpenFilesToRead();

        // other indexes files QHash<QString, EgIndexFiles<qint32>*>::iterator
    for (auto indexesIter = indexFiles.begin(); indexesIter != indexFiles.end(); ++indexesIter)
    {
        // IndexFileName = metaInfo-> typeName + "_" + indexesIter.key();
        indexesIter.value()-> OpenFilesToRead();
    }

    return 0;
}

inline int EgDataFiles::LocalOpenFilesToUpdate()
{   
    // if (! dir.exists("egdb"))
    //    dir.mkdir("egdb");

    dir = QDir::current();

    if (! dir.exists("egdb"))
    {
        qDebug()  << "error: can't open the egdb dir " << FN;
        return -1;
    }

        // meta info file
    ddt_file.setFileName("egdb/" + metaInfo-> typeName + ".ddt");

    if (!ddt_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "error: can't open file " << metaInfo-> typeName + ".ddt" << FN;
        return -1;
    }

    ddt.setDevice(&ddt_file);

        // data nodes file
    dat_file.setFileName("egdb/" + metaInfo-> typeName + ".dat");

    if (!dat_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "error: can't open file " << metaInfo-> typeName + ".dat" << FN;
        return -1;
    }

    dat.setDevice(&dat_file);

        // primary index

    // QString IndexFileName = metaInfo-> typeName + "_odb_pit"; // FIXME const
    primIndexFiles-> OpenFilesToUpdate();

        // other indexes files QHash<QString, EgIndexFiles<qint32>*>::iterator
    for (auto indexesIter = indexFiles.begin(); indexesIter != indexFiles.end(); ++indexesIter)
    {
        // IndexFileName = metaInfo-> typeName + "_" + indexesIter.key();
        indexesIter.value()-> OpenFilesToUpdate();
    }

    return 0;
}

void EgDataFiles::LocalCloseFiles()
{
    // dir = QDir::current();

    ddt_file.close();
    dat_file.close();

    primIndexFiles-> CloseFiles();

        // other indexes files QHash<QString, EgIndexFiles<qint32>*>::iterator
    for (auto indexesIter = indexFiles.begin(); indexesIter != indexFiles.end(); ++indexesIter)
        indexesIter.value()-> CloseFiles();

    // if (dir.dirName() == "egdb")
    //    dir.setCurrent("..");
}


int EgDataFiles::RemoveLocalFiles()
{
    dir = QDir::current();

    // if (dir.dirName() != "egdb")
    //    dir.setCurrent("egdb");

            // FIXME remove index files

    return (int)(ddt_file.remove() && dat_file.remove());
}


void EgDataFiles::ReceiveDataNodes(QMap<EgDataNodeIdType, EgDataNode>& dataNodesMap, QDataStream& in)
{
    EgDataNode tmpNode;

    dataNodesMap.clear();

    uint32_t count = 0;

    in >> count;

    // qDebug()  << "count = " << count << FN ;

    for (uint32_t i =0; i < count; ++i)
    {
        tmpNode.dataFields.clear();
        in >> tmpNode;

        // qDebug()  << "tmpNode.dataNodeID = " << tmpNode.dataNodeID << FN ;

        dataNodesMap.insert(tmpNode.dataNodeID, tmpNode);
    }

}

int EgDataFiles::LocalLoadDataNodes(const QSet<quint64> &dataOffsets, QList<EgDataNode>& dataNodes)
{
    EgDataNode tmpNode;
    int retCode = 0;

    tmpNode.extraInfo = metaInfo;

        // open data nodes file
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

        // read data to dataNodesMap by dataOffsets QSet<quint64>::iterator
    for (auto offsets_iter = dataOffsets.begin(); offsets_iter != dataOffsets.end(); ++offsets_iter)
    {
        // qDebug() << FN <<  "Seek data file, pos =" << hex << (int) *offsets_iter;

            // go to rec position
        if (! dat.device()-> seek(*offsets_iter)) // move to data
        {
            qDebug() << FN <<  "Can't seek data file, pos =" << hex << (int) *offsets_iter;
            retCode = -2;
            continue;
        }
            // read data
        tmpNode.dataFields.clear();

        dat >> tmpNode.dataNodeID;
        dat >> tmpNode.dataFields;

        tmpNode.dataFileOffset = *offsets_iter;

            // implement local filter here
        if (FilterCallback)
            if (! FilterCallback(tmpNode, filterValues))
                continue;

        dataNodes.append(tmpNode);
    }

    // qDebug() << FN <<  "dataNodesMap.count() =" << dataNodesMap.count();

    dat_file.close();

    return retCode;
}

int EgDataFiles::LocalLoadData(QSet<quint64>& dataOffsets, QMap<EgDataNodeIdType, EgDataNode>& dataNodesMap)
{
    EgDataNode tmpNode2;
    int retCode = 0;

    tmpNode2.extraInfo = metaInfo;

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
        }
            // read data
        tmpNode2.dataFields.clear();
        dat >> tmpNode2.dataNodeID;                
        dat >> tmpNode2.dataFields;

        tmpNode2.dataFileOffset = *offsets_iter;

            // implement local filter here
        if (FilterCallback)
            if (! FilterCallback(tmpNode2, filterValues))
                continue;

        dataNodesMap.insert(tmpNode2.dataNodeID, tmpNode2);
    }

    dat_file.close();

    return retCode;
}

int EgDataFiles::LocalStoreData(QMap<EgDataNodeIdType, EgDataNode*>&  addedDataNodes, QMap<EgDataNodeIdType, EgDataNode>& deletedDataNodes, QMap<EgDataNodeIdType, EgDataNode*>&  updatedDataNodes)
{
    if (LocalOpenFilesToUpdate())
        return -1;

        // process deleted nodes
    if (! deletedDataNodes.isEmpty())
        // LocalDeleteObjects(deletedDataNodes);
        LocalDeleteNodesList(deletedDataNodes.values());

        // process updated nodes
    if (! updatedDataNodes.isEmpty())
        LocalModifyNodesMap(updatedDataNodes);

        // add new
    if (! addedDataNodes.isEmpty())
        LocalAddNodesMap(addedDataNodes);

        // close files
    LocalCloseFiles();

        // TODO : unlock table

    return 0;
}

inline void EgDataFiles::LocalAddNodesMap(const QMap<EgDataNodeIdType, EgDataNode *> &addedDataNodes)
{
    dat.device()-> seek(dat.device()-> size());

        // walk add list QMap<EgDataNodeIDtype, EgDataNode*>::iterator
    for (auto addIter = addedDataNodes.begin(); addIter != addedDataNodes.end(); ++addIter)
    {
        // qDebug() << "Adding object" << (int) addIter.value()-> dataNodeID << " on offset" << hex << (int) dat.device()-> pos() << FN;

        primIndexFiles-> dataOffset = dat.device()-> pos();
        addIter.value()-> dataFileOffset = primIndexFiles-> dataOffset;    // save offset;

            // store data node
        dat << addIter.value()-> dataNodeID;
        dat << addIter.value()-> dataFields;

            // add primary index
        primIndexFiles-> theIndex = addIter.value()-> dataNodeID;
        primIndexFiles-> AddIndex();

            // add other indexes
        // for (QHash<QString, int> ::iterator indIter = metaInfo-> indexedToOrder.begin(); indIter != metaInfo-> indexedToOrder.end(); ++indIter)
        for (auto indIter = metaInfo-> indexedFields.begin(); indIter != metaInfo-> indexedFields.end(); ++indIter)
        {
            if (indexFiles.contains(indIter.key()))
            {
                // qDebug() << "Adding index" << addIter.value()-> dataFields[indIter.value().fieldNum].toInt() << " on offset" << hex << (int) primIndexFiles-> dataOffset << FN;

                indexFiles[indIter.key()]-> setIndex(addIter.value()-> dataFields[indIter.value().fieldNum]); // TODO - calc index of QVariant
                indexFiles[indIter.key()]-> setDataOffset(primIndexFiles-> dataOffset);
                indexFiles[indIter.key()]-> AddIndex();
            }
            else
                qDebug() << "Index not found: " << indIter.key() << FN;

        }
    }
}

void EgDataFiles::LocalAddNodesList(const QList<EgDataNode>&  addedDataNodes)
{
    dat.device()-> seek(dat.device()-> size());

        // walk add list QMap<EgDataNodeIDtype, EgDataNode*>::iterator
    for (auto addIter : addedDataNodes)
    {
        // qDebug() << FN << "Adding object" << (int) addIter.value()-> dataNodeID << " on offset" << hex << (int) dat.device()-> pos();

        primIndexFiles-> dataOffset = dat.device()-> pos();
        addIter.dataFileOffset = primIndexFiles-> dataOffset;    // save offset;

            // store data node
        dat << addIter.dataNodeID;
        dat << addIter.dataFields;;

            // add primary index
        primIndexFiles-> theIndex = addIter.dataNodeID;
        primIndexFiles-> AddIndex();

            // add other indexes
        // for (QHash<QString, int> ::iterator indIter = metaInfo-> indexedToOrder.begin(); indIter != metaInfo-> indexedToOrder.end(); ++indIter)
        for (auto indIter = metaInfo-> indexedFields.begin(); indIter != metaInfo-> indexedFields.end(); ++indIter)
        {
            if (indexFiles.contains(indIter.key()))
            {
                indexFiles[indIter.key()]-> setIndex(addIter.dataFields[indIter.value().fieldNum]); // TODO - calc index of QVariant
                indexFiles[indIter.key()]-> setDataOffset(primIndexFiles-> dataOffset);
                indexFiles[indIter.key()]-> AddIndex();
            }
            else
                qDebug() << "Index not found: " << indIter.key() << FN;

        }
    }
}

void EgDataFiles::SendNodesToStream(QMap<EgDataNodeIdType, EgDataNode*>&  dataNodesMap, QDataStream &nodesStream)
{
    nodesStream << (uint32_t) dataNodesMap.count();

    for (auto sendIter = dataNodesMap.begin(); sendIter != dataNodesMap.end(); ++sendIter)
    {
        // qDebug() << "Sending node " << (int) addIter.value()-> dataNodeID << FN ;

        nodesStream << *(sendIter.value());
    }
}

void EgDataFiles::SendNodesToStream(QMap<EgDataNodeIdType, EgDataNode>&  dataNodesMap, QDataStream &nodesStream)
{
    nodesStream << (uint32_t) dataNodesMap.count();

    for (auto sendIter = dataNodesMap.begin(); sendIter != dataNodesMap.end(); ++sendIter)
    {
        // qDebug() << "Sending node " << (int) addIter.value()-> dataNodeID << FN ;

        nodesStream << sendIter.value();
    }
}

void EgDataFiles::LocalDeleteNodesList(const QList<EgDataNode>&  deletedDataNodes)
{
    for (auto delIter : deletedDataNodes)
    {
        // qDebug() << FN << "Del object ID = " << (int) delIter.dataNodeID << " with offset" << hex << (int) delIter.dataFileOffset;

            // del primary index
        primIndexFiles-> theIndex = delIter.dataNodeID;
        primIndexFiles-> dataOffset = delIter.dataFileOffset;
        primIndexFiles-> DeleteIndex(true);   // SIDE estimate old offset stored here

            // del other indexes
        // for (QHash<QString, int> ::iterator indIter = metaInfo-> indexedToOrder.begin(); indIter != metaInfo-> indexedToOrder.end(); ++indIter)
        for (auto indIter = metaInfo-> indexedFields.begin(); indIter != metaInfo-> indexedFields.end(); ++indIter)
        {
            if (indexFiles.contains(indIter.key()))
            {
                indexFiles[indIter.key()]-> setIndex(delIter.dataFields[indIter.value().fieldNum]); // TODO - calc index of QVariant
                indexFiles[indIter.key()]-> setDataOffset(primIndexFiles-> dataOffset);
                indexFiles[indIter.key()]-> DeleteIndex(false);
            }
            else
                qDebug() << "Index not found: " << indIter.key() << FN;

        }
    }
}


inline int EgDataFiles::LocalModifyNodesMap(const QMap<EgDataNodeIdType, EgDataNode*>&  updatedDataNodes)
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
        dat << addIter.value()-> dataFields;

            // add primary index
        primIndexFiles-> theIndex = addIter.value()-> dataNodeID;
        primIndexFiles-> dataOffset = addIter.value()-> dataFileOffset;
        primIndexFiles-> UpdateIndex(false, true); // false means primary ID is not changed

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
                    indexFiles[indIter.key()]-> UpdateIndex(isUpdated, false);

                }
                else
                    qDebug() << "Error: Index not found: " << indIter.key() << FN;

            }
        }
    }

    return 0;
}

int EgDataFiles::LocalModifyNodesList(const QList<EgDataNode> &updatedDataNodes)
{
    EgDataNode tmpNode;

        // walk updated nodes list    QMap<EgDataNodeIDtype, EgDataNode*>::iterator
    for (auto updIter : updatedDataNodes)
    {
        dat.device()-> seek(dat.device()-> size());

            // primary index data offset
        primIndexFiles-> newOffset = dat.device()-> pos();

        // qDebug()  << "Update object" << addIter.value()-> dataNodeID << " old offset" << hex << addIter.value()-> dataFileOffset
        //           << " new offset " << hex << primIndexFiles-> newOffset << FN;

        dat << updIter.dataNodeID;
        dat << updIter.dataFields;

            // add primary index
        primIndexFiles-> theIndex = updIter.dataNodeID;
        primIndexFiles-> dataOffset = updIter.dataFileOffset;   // obsolete, set by UpdateIndex(), FIXME check
        primIndexFiles-> UpdateIndex(false, true); // first bool means primary ID is not changed, second - is primary

        updIter.dataFileOffset = primIndexFiles-> newOffset;

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


                    bool isUpdated = tmpNode.dataFields[indIter.value().fieldNum] != updIter.dataFields[indIter.value().fieldNum];

                    indexFiles[indIter.key()]-> setNewOffset(primIndexFiles-> newOffset);
                    indexFiles[indIter.key()]-> setNewIndex(updIter.dataFields[indIter.value().fieldNum]);
                    indexFiles[indIter.key()]-> UpdateIndex(isUpdated, false);

                }
                else
                    qDebug() << "Error: Index not found: " << indIter.key() << FN;

            }
        }
    }

    return 0;
}

int EgDataFiles::LocalCompressData() // FIXME TODO copy all except deleted records
{
    // select all

    // delete data files and indexes

    // add data as new items

    return 0;
}
