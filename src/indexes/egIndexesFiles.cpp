/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "../egDataNodesType.h"
#include "egIndexesFiles.h"

template <typename KeyType> void EgIndexFiles<KeyType>::RemoveIndexFiles()
{
    indexChunks.RemoveIndexFiles(IndexFileName);
    fingersTree.RemoveIndexFiles(IndexFileName);
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenIndexFilesToUpdate()
{
    // qDebug() << "IndexFileName = " << IndexFileName << FN;

    dir = QDir::current();

    if (! dir.exists("egdb"))
    {
        qDebug()  << "error: can't find the egdb dir " << FN;
        return -1;
    }


    int res = indexChunks.OpenIndexFilesToUpdate(IndexFileName);
    res += fingersTree.OpenFingerFileToUpdate(IndexFileName);

    fingersTree.IndexFileName = IndexFileName;

    if (fingersTree.fingerStream.device()->size() && ! res) // is not empty FIXME check both files
    {
        // indexChunks.LoadRootHeader();
        fingersTree.LoadRootHeader();
    }
    // else
    //   qDebug() << "Can't load Root Header " << FN;

    return res;
}



template <typename KeyType> int EgIndexFiles<KeyType>::OpenIndexFilesToRead()
{
    // qDebug() << "In OpenIndexFilesToRead()" << FN;

    dir = QDir::current();

    // if (dir.dirName() != "egdb")
    if (! dir.exists("egdb"))
    {
        qDebug()  << "error: can't find the egdb dir " << FN;
        return -1;
    }

    int res = indexChunks.OpenIndexFilesToRead(IndexFileName);
    res += fingersTree.OpenFingerFileToRead(IndexFileName);

    if (res)    // error
    {
        indexChunks.CloseIndexFiles();
        fingersTree.CloseIndexFiles();

        // if (dir.dirName() == "egdb")
        //    dir.setCurrent("..");

        return res;
    }

    fingersTree.IndexFileName = IndexFileName;

    if (fingersTree.fingerStream.device()->size()) // && indexChunks.indexStream.device()->size()) // is not empty
    {
        // indexChunks.LoadRootHeader();
        fingersTree.LoadRootHeader();
    }
    // else
    //    qDebug() << "Can't load Root Header " << res << FN;

    return res;
}

template <typename KeyType> void EgIndexFiles<KeyType>::CloseIndexFiles()
{
    // qDebug() << "In CloseIndexFiles()" << FN;

    indexChunks.CloseIndexFiles();
    fingersTree.CloseIndexFiles();

    // if (dir.dirName() == "egdb")
    //    dir.setCurrent("..");
}


template <typename KeyType> void EgIndexFiles<KeyType>::AddIndex()
{

    indexChunks.theKey = theIndex;
    indexChunks.oldDataOffset = dataOffset;

    // qDebug()  << "IndexFileName = " << IndexFileName << FN;

        // check for empty file
    if ( ! indexChunks.indexStream.device()->size() ) // is empty
    {
        // qDebug()  << "index" << the_index << "empty file, calling AppendNewChunk()" << FN;

        indexChunks.InitRootHeader();
        indexChunks.InitIndexChunk();

        fingersTree.InitRootHeader();
        fingersTree.InitFingersChunk();

        indexChunks.StoreRootHeader();
        fingersTree.StoreRootHeader();

        indexChunks.StoreIndexChunk(indexChunks.chunk);
        fingersTree.StoreFingersChunk(fingersTree.rootHeaderSize, fingersTree.fingersChunk);

        return;
    }
    else
    {
        // indexChunks.LoadRootHeader();
        fingersTree.FindIndexChunkToInsert();
        // fingersTree.FindIndexChunkToInsert();

        // qDebug() << "finger to insert key: " << hex << (int) indexChunks.theKey << ", offset: " << hex << (int) indexChunks.indexesChunkOffset << FN;

        indexChunks.InsertToIndexChunk();
    }
}

template <typename KeyType> int EgIndexFiles<KeyType>::UpdateIndex(bool isChanged, bool isPrimary)
{
    indexChunks.theKey = theIndex;
    indexChunks.oldDataOffset = dataOffset;
    indexChunks.newDataOffset = newOffset;

    if (fingersTree.FindIndexChunkFirst(true))
    {
        qDebug() << "IndexChunk not found " << FN;
        return -1;
    }

    if (! isChanged)
        indexChunks.UpdateIndex(isPrimary);
    else // FIXME : check if last index
    {
        indexChunks.DeleteIndex(isPrimary);

        indexChunks.theKey = newIndex;
        indexChunks.oldDataOffset = newOffset;

        fingersTree.FindIndexChunkToInsert();   // FIXME check result

        // qDebug() << "nextOffset: " << hex << (int) indexChunks. << FN;

        indexChunks.InsertToIndexChunk();
    }

    if (isPrimary)
        dataOffset = indexChunks.oldDataOffset;

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::DeleteIndex(bool isPrimary)
{

    indexChunks.theKey = theIndex;
    indexChunks.oldDataOffset = dataOffset;

    if (fingersTree.FindIndexChunkFirst(true))
    {
        qDebug() << "IndexChunk not found " << FN;
        return -1;
    }

    indexChunks.DeleteIndex(isPrimary);

    if (isPrimary)
        dataOffset = indexChunks.oldDataOffset;

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::LoadAllDataOffsets(QSet<quint64>& dataOffsets)
{
    int res = OpenIndexFilesToRead();

    if (res)    // error
        return res;

    fingersTree.IndexFileName = IndexFileName;  // debug info

    indexChunks.LoadAllData(dataOffsets);

    CloseIndexFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_EQ(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenIndexFilesToRead();

    if (res)    // error
        return res;

    indexChunks.theKey   = Key;

    if (fingersTree.FindIndexChunkFirst(true) < 0)
        res = -1;

    // qDebug() << "filename: " << IndexFileName << " ,key: " << hex << (int) indexChunks.theKey
    //         << ", offset: " << hex << (int) indexChunks.indexesChunkOffset << "res = " << res << FN;


    if (! res)  // ok
        indexChunks.LoadDataByChunkEqual(index_offsets);

    CloseIndexFiles();

    return res;
}


template <typename KeyType> int EgIndexFiles<KeyType>::Load_GE(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenIndexFilesToRead();

    if (res)    // error
        return res;

    indexChunks.theKey   = Key;

    if (fingersTree.FindIndexChunkFirst(true) < 0) // FIXME - process borders
    {
        // qDebug() << "Index chunk not found " << IndexFileName << FN;
        res = -1;
    }

    if (! res)  // ok
        indexChunks.LoadDataByChunkUp(index_offsets, EgIndexes<KeyType>::CompareGE);

    CloseIndexFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_GT(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenIndexFilesToRead();

    if (res)    // error
        return res;

    indexChunks.theKey = Key;

    if (fingersTree.FindIndexChunkLast(false) < 0) // FIXME - process borders
        res = -1;

    if (! res)
        indexChunks.LoadDataByChunkUp(index_offsets, EgIndexes<KeyType>::CompareGT);

    qDebug()  << "res = " << res << "index_offsets count = " << index_offsets.count() << FN;

    CloseIndexFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_LE(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenIndexFilesToRead();

    if (res)    // error
        return res;

    indexChunks.theKey   = Key;

    if (fingersTree.FindIndexChunkLast(true) < 0) // FIXME - process borders
        res = -1;

    if (! res)
        indexChunks.LoadDataByChunkDown(index_offsets, EgIndexes<KeyType>::CompareLE);

    CloseIndexFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_LT(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenIndexFilesToRead();

    if (res)    // error
        return res;

    indexChunks.theKey = Key;

    if (fingersTree.FindIndexChunkFirst(false) < 0) // FIXME - process borders
        res = -1;

    if (! res)
        indexChunks.LoadDataByChunkDown(index_offsets, EgIndexes<KeyType>::CompareLT);

    // qDebug()  << "res = " << res << "index_offsets count = " << index_offsets.count() << FN;

    CloseIndexFiles();

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

