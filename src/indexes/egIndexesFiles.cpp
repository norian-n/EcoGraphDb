#include "../egDataNodesType.h"
#include "egIndexesFiles.h"

template <typename KeyType> void EgIndexFiles<KeyType>::RemoveIndexFiles()
{
    indexChunks.RemoveIndexFiles(IndexFileName);
    fingersTree.RemoveIndexFiles(IndexFileName);
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenIndexFilesToUpdate()
{
    int res = indexChunks.OpenIndexFilesToUpdate(IndexFileName);
    res += fingersTree.OpenIndexFilesToUpdate(IndexFileName);

    if (indexChunks.indexStream.device()->size() && ! res) // is not empty 
        indexChunks.LoadRootHeader();
    // else
    //    qDebug() << "Can't load Root Header " << FN;

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenIndexFilesToRead()
{
    int res = indexChunks.OpenIndexFilesToRead(IndexFileName);
    res += fingersTree.OpenIndexFilesToRead(IndexFileName);

    if (indexChunks.indexStream.device()->size() && ! res) // is not empty
        indexChunks.LoadRootHeader();
    // else
    //    qDebug() << "Can't load Root Header " << res << FN;

    return res;
}

template <typename KeyType> void EgIndexFiles<KeyType>::CloseIndexFiles()
{
    indexChunks.CloseIndexFiles();
    fingersTree.CloseIndexFiles();
}


template <typename KeyType> void EgIndexFiles<KeyType>::AddObjToIndex()
{

    indexChunks.theKey = theIndex;
    indexChunks.oldDataOffset = dataOffset;

        // check for empty file
    if ( ! indexChunks.indexStream.device()->size() ) // is empty
    {
        // qDebug()  << "index" << the_index << "empty file, calling AppendNewChunk() << FN";

        indexChunks.InitRootHeader();
        indexChunks.InitIndexChunk();
        fingersTree.InitFingersChunk();

        indexChunks.StoreRootHeader();
        indexChunks.StoreIndexChunk(indexChunks.chunk);
        fingersTree.StoreFingersChunk(0, fingersTree.fingersChunk);

        return;
    }
    else
    {
        // indexChunks.LoadRootHeader();
        fingersTree.FindIndexChunkToInsert();

        // qDebug() << "finger to insert key: " << hex << (int) indexChunks.theKey << ", offset: " << hex << (int) indexChunks.indexesChunkOffset << FN;

        indexChunks.InsertToIndexChunk();
    }
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_EQ(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = 0;

    OpenIndexFilesToRead();

    indexChunks.theKey   = Key;

    if (fingersTree.FindIndexChunkFirst(true) < -1)
        res = -1;

    if (! res)
        indexChunks.LoadDataByChunkEqual(index_offsets);

    CloseIndexFiles();

    return res;
}


template <typename KeyType> int EgIndexFiles<KeyType>::Load_GE(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = 0;

    // qDebug() << "File name " << IndexFileName << FN;

    OpenIndexFilesToRead();

    indexChunks.theKey   = Key;

    if (fingersTree.FindIndexChunkFirst(true) < -1) // FIXME - process borders
    {
        qDebug() << "Index chunk not found " << IndexFileName << FN;
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

    OpenIndexFilesToRead();

    indexChunks.theKey = Key;

    if (fingersTree.FindIndexChunkFirst(false) < -1) // FIXME - process borders
        res = -1;

    if (! res)
        indexChunks.LoadDataByChunkUp(index_offsets, EgIndexes<KeyType>::CompareGT);

    CloseIndexFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_LE(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = 0;

    OpenIndexFilesToRead();

    indexChunks.theKey   = Key;

    if (fingersTree.FindIndexChunkLast(true) < -1) // FIXME - process borders
        res = -1;

    if (! res)
        indexChunks.LoadDataByChunkDown(index_offsets, EgIndexes<KeyType>::CompareLE);

    CloseIndexFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_LT(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = 0;

    OpenIndexFilesToRead();

    indexChunks.theKey = Key;

    if (fingersTree.FindIndexChunkLast(false) < -1) // FIXME - process borders
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
        return -1;

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
