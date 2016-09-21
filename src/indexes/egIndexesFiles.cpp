/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
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

    int res = indexChunks.OpenIndexFilesToRead(IndexFileName);
    res += fingersTree.OpenFingerFileToRead(IndexFileName);

    fingersTree.IndexFileName = IndexFileName;

    if (fingersTree.fingerStream.device()->size() && ! res) // is not empty FIXME check both files
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
}


template <typename KeyType> void EgIndexFiles<KeyType>::AddObjToIndex()
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

template <typename KeyType> int EgIndexFiles<KeyType>::LoadAllDataOffsets(QSet<quint64>& index_offsets)
{
    int res = 0;

    if (! dir.setCurrent("egdb"))
    {
        qDebug()  << "can't find the egdb dir " << FN;
        return -1;
    }

    res = OpenIndexFilesToRead();

    dir.setCurrent("..");

    if (res)
        return res;

    indexChunks.LoadAllData(index_offsets);

    CloseIndexFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_EQ(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = 0;

    if (! dir.setCurrent("egdb"))
    {
        qDebug()  << "can't find the egdb dir " << FN;
        return -1;
    }

    res = OpenIndexFilesToRead();

    if (res)
    {
        dir.setCurrent("..");
        return res;
    }

    indexChunks.theKey   = Key;

    if (fingersTree.FindIndexChunkFirst(true) < 0)
        res = -1;

    // qDebug() << "filename: " << IndexFileName << " ,key: " << hex << (int) indexChunks.theKey
    //         << ", offset: " << hex << (int) indexChunks.indexesChunkOffset << "res = " << res << FN;


    if (! res)
        indexChunks.LoadDataByChunkEqual(index_offsets);

    CloseIndexFiles();

    dir.setCurrent("..");

    return res;
}


template <typename KeyType> int EgIndexFiles<KeyType>::Load_GE(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = 0;

    // qDebug() << "File name " << IndexFileName << FN;

    if (! dir.setCurrent("egdb"))
    {
        qDebug()  << "can't find the egdb dir " << FN;
        return -1;
    }

    res = OpenIndexFilesToRead();

    dir.setCurrent("..");

    if (res)
        return res;

    indexChunks.theKey   = Key;

    if (fingersTree.FindIndexChunkFirst(true) < 0) // FIXME - process borders
    {
        // qDebug() << "Index chunk not found " << IndexFileName << FN;
        res = -1;
    }

    if (! res)
        indexChunks.LoadDataByChunkUp(index_offsets, EgIndexes<KeyType>::CompareGE);

    CloseIndexFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_GT(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = 0;

    if (! dir.setCurrent("egdb"))
    {
        qDebug()  << "can't find the egdb dir " << FN;
        return -1;
    }

    res = OpenIndexFilesToRead();

    dir.setCurrent("..");


    if (res)
        return res;

    indexChunks.theKey = Key;

    if (fingersTree.FindIndexChunkFirst(false) < 0) // FIXME - process borders
        res = -1;

    if (! res)
        indexChunks.LoadDataByChunkUp(index_offsets, EgIndexes<KeyType>::CompareGT);

    CloseIndexFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_LE(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = 0;

    if (! dir.setCurrent("egdb"))
    {
        qDebug()  << "can't find the egdb dir " << FN;
        return -1;
    }

    res = OpenIndexFilesToRead();

    dir.setCurrent("..");

    if (res)
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
    int res = 0;

    if (! dir.setCurrent("egdb"))
    {
        qDebug()  << "can't find the egdb dir " << FN;
        return -1;
    }

    res = OpenIndexFilesToRead();

    dir.setCurrent("..");

    if (res)
        return res;

    indexChunks.theKey = Key;

    if (fingersTree.FindIndexChunkLast(false) < 0) // FIXME - process borders
        res = -1;

    if (! res)
        indexChunks.LoadDataByChunkDown(index_offsets, EgIndexes<KeyType>::CompareLT);

    CloseIndexFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::UpdateIndex(bool isChanged)
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
        indexChunks.UpdateIndex();
    else
    {
        indexChunks.DeleteIndex();

        indexChunks.oldDataOffset = newOffset;

        fingersTree.FindIndexChunkToInsert();

        // qDebug() << "nextOffset: " << hex << (int) indexChunks. << FN;

        indexChunks.InsertToIndexChunk();
    }

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::DeleteIndex()
{

    indexChunks.theKey = theIndex;
    indexChunks.oldDataOffset = dataOffset;

    if (fingersTree.FindIndexChunkFirst(true))
    {
        qDebug() << "IndexChunk not found " << FN;
        return -1;
    }

    indexChunks.DeleteIndex();

    return 0;
}

// template class EgIndexesComparator<qint32>;

template class EgIndexFiles<qint32>; // for unit tests
