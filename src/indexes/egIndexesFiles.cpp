/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "../egDataNodesType.h"
#include "egIndexesFiles.h"

template <typename KeyType> void EgIndexFiles<KeyType>::RemoveFiles()
{
    if (indexFile.isOpen())
        indexFile.close();

    indexFile.setFileName("egdb/" + IndexFileName + ".odx");
    indexFile.remove();

    if (fingerFile.isOpen())
        fingerFile.close();

    fingerFile.setFileName("egdb/" + IndexFileName + ".odf");
    fingerFile.remove();
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenIndexFileToUpdate(QDataStream& theIndexStream)
{
    indexFile.close();

    indexFile.setFileName("egdb/" + IndexFileName + ".odx");

    theIndexStream.setDevice(&indexFile);

    if (!indexFile.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        EG_LOG_STUB << FN << "can't open index file " << indexFile.fileName();
        return -1;
    }

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenIndexFileToRead(QDataStream& theIndexStream)
{
    indexFile.close();

    indexFile.setFileName("egdb/" + IndexFileName + ".odx");

    if (!indexFile.exists())
    {
        EG_LOG_STUB << "Error: file doesn't exist " << indexFile.fileName() << FN;
        return -1;
    }

    theIndexStream.setDevice(&indexFile);

    if (!indexFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        if (! IndexFileName.contains(EgDataNodesNamespace::egGUIfileName))
            EG_LOG_STUB << "Error: can't open index file " << indexFile.fileName() << FN;
        return -2;
    }

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenIndexFileToCheck(const QString& IndexFilePath, QDataStream& theIndexStream)
{
    indexFile.close();

    indexFile.setFileName(IndexFilePath);

    if (!indexFile.exists())
    {
        EG_LOG_STUB << "Error: file doesn't exist " << IndexFilePath << FN;
        return -1;
    }

    theIndexStream.setDevice(&indexFile);

    if (!indexFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        EG_LOG_STUB << "Error: can't open index file " << indexFile.fileName() << FN;
        return -2;
    }

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenFingerFileToUpdate(QDataStream &theFingerStream)
{
    fingerFile.close();

    fingerFile.setFileName("egdb/" + IndexFileName + ".odf");
    theFingerStream.setDevice(&fingerFile);

    if (!fingerFile.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        EG_LOG_STUB << FN << "can't open finger file " << fingerFile.fileName();
        return -2;
    }

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenFingerFileToRead(QDataStream& theFingerStream)
{
    fingerFile.close();

    fingerFile.setFileName("egdb/" + IndexFileName + ".odf");

    if (!fingerFile.exists())
    {
        EG_LOG_STUB << "file doesn't exist " << IndexFileName + ".odf" << FN;
        return -1;
    }

    theFingerStream.setDevice(&fingerFile);

    if (!fingerFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        EG_LOG_STUB << "can't open finger file " << fingerFile.fileName() << FN;
        return -2;
    }

    return 0;
}


template <typename KeyType> int EgIndexFiles<KeyType>::OpenFingerFileToCheck(const QString& IndexFilePath, QDataStream &theFingerStream)
{
    fingerFile.close();

    fingerFile.setFileName(IndexFilePath);

    if (!fingerFile.exists())
    {
        EG_LOG_STUB << "file doesn't exist " << IndexFileName << FN;
        return -1;
    }

    theFingerStream.setDevice(&fingerFile);

    if (!fingerFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        EG_LOG_STUB << FN << "can't open finger file " << fingerFile.fileName() << FN;
        return -2;
    }

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenFilesToUpdate()
{
    // EG_LOG_STUB << "IndexFileName = " << IndexFileName << FN;

    dir = QDir::current();

    if (! dir.exists("egdb"))
    {
        EG_LOG_STUB  << "error: can't find the egdb dir " << FN;
        return -1;
    }

    fingersTree.IndexFileName = IndexFileName;

    int res = OpenIndexFileToUpdate(indexChunks.indexStream);
    res    += OpenFingerFileToUpdate(fingersTree.fingerStream);

    if (fingersTree.fingerStream.device()->size() && ! res) // is not empty FIXME check both files
    {
        // indexChunks.LoadRootHeader();
        fingersTree.LoadRootFinger();
    }
    // else
    //   EG_LOG_STUB << "Can't load Root Header " << FN;

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenFilesToRead()
{
    // EG_LOG_STUB << "In OpenIndexFilesToRead()" << FN;

    dir = QDir::current();

    // if (dir.dirName() != "egdb")
    if (! dir.exists("egdb"))
    {
        EG_LOG_STUB  << "error: can't find the egdb dir " << FN;
        return -1;
    }

    fingersTree.IndexFileName = IndexFileName;

    int res = OpenIndexFileToRead(indexChunks.indexStream);
    res    += OpenFingerFileToRead(fingersTree.fingerStream);

    if (res)    // error
    {
        CloseFiles();

        // if (dir.dirName() == "egdb")
        //    dir.setCurrent("..");

        return res;
    }

    if (fingersTree.fingerStream.device()->size()) // && indexChunks.indexStream.device()->size()) // is not empty
    {
        // indexChunks.LoadRootHeader();
        fingersTree.LoadRootFinger();
    }
    // else
    //    EG_LOG_STUB << "Can't load Root Header " << res << FN;

    return res;
}

template <typename KeyType> void EgIndexFiles<KeyType>::CloseFiles()
{
    indexFile.close();
    fingerFile.close();
}


template <typename KeyType> void EgIndexFiles<KeyType>::AddIndex()
{
        // have to set this fields to use indexes
    indexChunks.theKey = theIndex;
    indexChunks.oldDataOffset = dataOffset;

    // EG_LOG_STUB  << "IndexFileName = " << IndexFileName << FN;
    // EG_LOG_STUB  << "Key: " << theIndex << " , offset: " << dataOffset << FN;

        // check for empty file
    if ( ! indexChunks.indexStream.device()->size() ) // is empty
    {       
            // add first index and root finger
        fingersTree.InitRootFinger();
        fingersTree.StoreRootFinger();

        indexChunks.InitIndexChunk();

        indexChunks.StoreRootHeader();
        indexChunks.StoreIndexChunk(indexChunks.indexBA.data(), egIndexesNamespace::indexHeaderSize);

        return;
    }
    else    // indexes ain't empty
    {
        fingersTree.FindIndexChunkToInsert();   // FIXME check if no finger in paranoid mode
        indexChunks.InsertToIndexChunk();       // FIXME check if can't insert in paranoid mode
    }
}

template <typename KeyType> int EgIndexFiles<KeyType>::UpdateIndex(bool isChanged, bool isPrimary)
{
            // have to set this fields to use indexes
    indexChunks.theKey = theIndex;
    indexChunks.oldDataOffset = dataOffset;
    indexChunks.newDataOffset = newOffset;

    // EG_LOG_STUB << "Update index files name:  " << IndexFileName << FN;

    if (fingersTree.FindIndexChunkFirst(true))
    {
        EG_LOG_STUB << "IndexChunk not found " << FN;
        return -1;
    }

    if (! isChanged)
        indexChunks.UpdateIndex(isPrimary);
    else
    {
        if (indexChunks.DeleteIndex(isPrimary) == 1) // last index, recreate
        {
            RemoveFiles();
            indexChunks.theKey = newIndex;
            indexChunks.oldDataOffset = newOffset;

            fingersTree.InitRootFinger();
            fingersTree.StoreRootFinger();

            indexChunks.InitIndexChunk();

            indexChunks.StoreRootHeader();
            indexChunks.StoreIndexChunk(indexChunks.indexBA.data(), egIndexesNamespace::indexHeaderSize);

            return 0;
        }

                // have to set this fields to use indexes
        indexChunks.theKey = newIndex;
        indexChunks.oldDataOffset = newOffset;

        fingersTree.FindIndexChunkToInsert();   // FIXME check result

        // EG_LOG_STUB << "nextOffset: " << hex << (int) indexChunks. << FN;

        indexChunks.InsertToIndexChunk();
    }

    if (isPrimary)
        dataOffset = indexChunks.oldDataOffset;

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::DeleteIndex(bool isPrimary)
{
        // have to set this fields to use indexes
    indexChunks.theKey = theIndex;
    indexChunks.oldDataOffset = dataOffset;

    // EG_LOG_STUB << "Delete index file name:  " << IndexFileName << FN;

    if (fingersTree.FindIndexChunkFirst(true)) // true means exact equal
    {
        EG_LOG_STUB << "IndexChunk not found " << FN;
        return -1;
    }

    if (indexChunks.DeleteIndex(isPrimary) == 1) // last index
    {
        RemoveFiles();
        return 0;
    }

        // side effect: get actual index offset if primary index - FIXME check
    if (isPrimary)
        dataOffset = indexChunks.oldDataOffset;

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::LoadAllDataOffsets(QSet<quint64>& dataOffsets)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

    fingersTree.IndexFileName = IndexFileName;  // debug info

    indexChunks.LoadAllData(dataOffsets);

    CloseFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_EQ(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

    indexChunks.theKey   = Key;

    res = fingersTree.FindIndexChunkFirst(true);

    // EG_LOG_STUB << "filename: " << IndexFileName << " ,key: " << hex << (int) indexChunks.theKey
    //         << ", offset: " << hex << (int) indexChunks.indexesChunkOffset << "res = " << res << FN;


    if (! res)  // ok
        indexChunks.LoadDataByChunkEqual(index_offsets);

    CloseFiles();

    return res;
}


template <typename KeyType> int EgIndexFiles<KeyType>::Load_GE(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

        // have to set this field to use indexes
    indexChunks.theKey   = Key;

    res = fingersTree.FindIndexChunkFirst(true); // FIXME - process borders

    if (! res)  // ok
        indexChunks.LoadDataByChunkUp(index_offsets, EgIndexes<KeyType>::CompareGE);

    CloseFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_GT(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

        // have to set this field to use indexes
    indexChunks.theKey = Key;

    res = fingersTree.FindIndexChunkLast(false); // FIXME - process borders

    if (! res)
        indexChunks.LoadDataByChunkUp(index_offsets, EgIndexes<KeyType>::CompareGT);

    // EG_LOG_STUB  << "res = " << res << "index_offsets count = " << index_offsets.count() << FN;

    CloseFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_LE(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

            // have to set this field to use indexes
    indexChunks.theKey   = Key;

    res = fingersTree.FindIndexChunkLast(true); // FIXME - process borders

    if (! res)
        indexChunks.LoadDataByChunkDown(index_offsets, EgIndexes<KeyType>::CompareLE);

    CloseFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_LT(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

            // have to set this field to use indexes
    indexChunks.theKey = Key;

    res = fingersTree.FindIndexChunkFirst(false); // FIXME - process borders

    if (! res)
        indexChunks.LoadDataByChunkDown(index_offsets, EgIndexes<KeyType>::CompareLT);

    // EG_LOG_STUB  << "res = " << res << "index_offsets count = " << index_offsets.count() << FN;

    CloseFiles();

    return res;
}

/*
EgIndexFiles<qint32>  EgIndexFilesqint32(QString("EgIndexFilesqint32"));
EgIndexFiles<quint32> EgIndexFilesquint32(QString("EgIndexFilesquint32"));
EgIndexFiles<qint64>  EgIndexFilesqint64(QString("EgIndexFilesqint64"));
EgIndexFiles<quint64> EgIndexFilesquint64(QString("EgIndexFilesquint64"));
EgIndexFiles<float>   EgIndexFilesfloat(QString("EgIndexFilesfloat"));
EgIndexFiles<double>  EgIndexFilesdouble(QString("EgIndexFilesdouble"));
*/

