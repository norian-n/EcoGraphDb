#include "egDataNodesType.h"
#include "egIndexes.h"
#include "egFingers.h"

using namespace egIndexes3Namespace;

template <typename KeyType> void EgIndexes<KeyType>::RemoveIndexFiles(const QString& IndexFileName)
{
    indexFile.setFileName(IndexFileName + ".odx");
    indexFile.remove();
}

template <typename KeyType> void EgIndexes<KeyType>::PrintIndexesChunk(char* theChunk, const QString &theMessage)
{
    qDebug() << QByteArray(theChunk, indexChunkSize).toHex() << theMessage;
}

template <typename KeyType> int EgIndexes<KeyType>::OpenIndexFilesToUpdate(const QString& IndexFileName)
{
    indexFile.setFileName(IndexFileName + ".odx");
    indexStream.setDevice(&indexFile);

    if (!indexFile.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open index file " << indexFile.fileName();
        return -1;
    }

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::OpenIndexFilesToRead(const QString& IndexFileName)
{
    indexFile.setFileName(IndexFileName + ".odx");
    indexStream.setDevice(&indexFile);

    if (!indexFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        if (! IndexFileName.contains(EgDataNodesGUInamespace::egGUIfileName))
            qDebug() << FN << "can't open index file " << indexFile.fileName();
        return -1;
    }

    return 0;
}

template <typename KeyType> void EgIndexes<KeyType>::CloseIndexFiles()
{
    indexFile.close();
}

template <typename KeyType> inline void EgIndexes<KeyType>::InitIndexChunk()
{
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);
    memcpy(indexBA.data(), zero_chunk, indexChunkSize);

        // write initial values of meta info
    localIndexStream << theKey;    // key
    localIndexStream << oldDataOffset;  // data node offset

    // qDebug() << indexBA.toHex() << FN;

        // update count
    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream << (keysCountType) 1;


    memcpy(chunk, indexBA.constData(), indexChunkSize);

    indexesChunkOffset = rootHeaderSize;
}


template <typename KeyType> void EgIndexes<KeyType>::InitRootHeader()
{
    fingersTree-> fingersRootHeader.minKey = theKey;
    fingersTree-> fingersRootHeader.maxKey = theKey;
    fingersTree-> fingersRootHeader.myLevel = 0;
    fingersTree-> fingersRootHeader.itemsCount = 1;
    fingersTree-> fingersRootHeader.nextChunkOffset = 0;
    fingersTree-> fingersRootHeader.myChunkOffset = 0;
    fingersTree-> fingersRootHeader.myOffset = 0;
}


template <typename KeyType> void EgIndexes<KeyType>::LoadRootHeader()
{
    indexStream.device()->seek(0);

    indexStream >> fingersTree-> fingersRootHeader.minKey;
    indexStream >> fingersTree-> fingersRootHeader.maxKey;
    indexStream >> fingersTree-> fingersRootHeader.myLevel;
    indexStream >> fingersTree-> fingersRootHeader.itemsCount;
    indexStream >> fingersTree-> fingersRootHeader.nextChunkOffset;
}

template <typename KeyType> void EgIndexes<KeyType>::StoreRootHeader(bool minMaxOnly)
{
    indexStream.device()->seek(0);

    indexStream << (KeyType) fingersTree-> fingersRootHeader.minKey;
    indexStream << (KeyType) fingersTree-> fingersRootHeader.maxKey;

    if (! minMaxOnly)
    {
        indexStream << (fingersLevelType) fingersTree-> fingersRootHeader.myLevel;
        indexStream << (keysCountType) fingersTree-> fingersRootHeader.itemsCount;
        indexStream << (quint64) fingersTree-> fingersRootHeader.nextChunkOffset;
    }
}

template <typename KeyType> int EgIndexes<KeyType>::StoreFingerOffset(quint64 fingerOffset)
{
    indexStream.device()->seek(indexesChunkOffset + egChunkVolume * oneIndexSize + sizeof(quint64) * 2 + sizeof(keysCountType));
    indexStream << fingerOffset;

    // qDebug() << "indexesChunkOffset = " << hex << (int) indexesChunkOffset
    //          << ", fingerOffset = " << hex << (int) fingerOffset << FN;


    return 0; // FIXME
}

template <typename KeyType> int EgIndexes<KeyType>::StoreIndexChunk(char* chunkPtr)
{
    indexStream.device()->seek(indexesChunkOffset);
    indexStream.writeRawData(chunkPtr, indexChunkSize);

    return 0; // FIXME
}


template <typename KeyType> inline void EgIndexes<KeyType>::LoadIndexChunk(char* chunkPtr)
{
    indexStream.device()->seek(indexesChunkOffset);
    indexStream.readRawData(chunkPtr, indexChunkSize);

/*        // check for debug
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    memcpy(indexBA.data(), chunk, egChunkVolume * KeySize);

        // write initial values of meta info
    localIndexStream >> theKey;    // key
    localIndexStream >> oldDataOffset;  // data node offset

    qDebug() << hex << (int) theKey << FN;
    qDebug() << hex << (int) oldDataOffset << FN;
    */
}

template <typename KeyType> int EgIndexes<KeyType>::InsertToIndexChunk()
{
        // FIXME - check chunk position
    // qDebug() << "indexesChunkOffset = " <<  hex << (int) indexesChunkOffset << FN;
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(chunk);

    // memcpy(indexBA.data(), chunk, indexChunkSize);

    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger);

    if (fingersTree-> currentFinger.itemsCount < egChunkVolume)
    {
        indexPosition = FindIndexPosition(localIndexStream);
        // qDebug() << "indexPosition = " << indexPosition << ", itemsCount = " << fingersTree-> currentFinger.itemsCount << FN;

            // check if not last position, move tail
        if (indexPosition != (fingersTree-> currentFinger.itemsCount)) // last one
        {
            // qDebug() << "chunk before memmove" << QByteArray(chunk, egChunkVolume*oneIndexSize).toHex() << FN;
            memmove (chunk + (indexPosition+1)*oneIndexSize, chunk + indexPosition*oneIndexSize,  oneIndexSize*(fingersTree-> currentFinger.itemsCount - indexPosition + 1));
            // PrintIndexesChunk(chunk, "chunk after memmove " + FNS);
        }

        memcpy(indexBA.data(), chunk, indexChunkSize);

            // write index key and offset
        localIndexStream.device()->seek(indexPosition*oneIndexSize);
        localIndexStream << theKey;
        localIndexStream << oldDataOffset;

        // memcpy(chunk, indexBA.constData(), indexChunkSize);

            // update count
        localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
        localIndexStream << (keysCountType) (fingersTree-> currentFinger.itemsCount + 1);

        // PrintIndexesChunk(indexBA.data(), "chunk after insert " + FNS);

        StoreIndexChunk(indexBA.data());

            // update fingers
        fingersTree-> UpdateFingerAfterInsert();
        fingersTree-> UpdateFingersChainAfterInsert();
    }
    else    // overflow - append or split chunk
    {
        if (theKey >= fingersTree-> fingersRootHeader.maxKey)
            AppendIndexChunk(localIndexStream);
        else
            SplitIndexChunk(localIndexStream);
    }

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::SplitIndexChunk(QDataStream& localIndexStream)
{
    KeyType minKey, maxKey;

    fingersTree-> newFinger.nextChunkOffset = indexStream.device()->size(); // append to the end of indexes file

    indexPosition = FindIndexPosition(localIndexStream);

    // qDebug() << "initialindexPosition = " << indexPosition << ", itemsCount = " << fingersTree-> currentFinger.itemsCount << FN;

        // init new chunk
    memmove (new_chunk, zero_chunk, indexChunkSize);

        // split chunks

    // qDebug() << "new chunk before memmove" << QByteArray(new_chunk, egChunkVolume*oneIndexSize).toHex() << FN;
    memmove (new_chunk, chunk + egChunkVolume/2*oneIndexSize,  egChunkVolume/2*oneIndexSize);
    // qDebug() << "new chunk after split up" << QByteArray(new_chunk, egChunkVolume*oneIndexSize).toHex() << FN;
        // add zeroes to first
    memmove (chunk + egChunkVolume/2*oneIndexSize, zero_chunk,  egChunkVolume/2*oneIndexSize);
    // qDebug() << "old chunk after split up " << QByteArray(chunk, egChunkVolume*oneIndexSize).toHex() << FN;


        // recalc position to splitted chunks

    if ( indexPosition <= egChunkVolume/2) // insert to first part
    {
        // check if not last position, move tail
        if (indexPosition != (egChunkVolume/2)) // last one
        {
            // qDebug() << "indexPosition = " << indexPosition << ", itemsCount = " << fingersTree-> currentFinger.itemsCount << FN;
            // qDebug() << "chunk before memmove" << QByteArray(chunk, egChunkVolume*oneIndexSize).toHex() << FN;
            memmove (chunk + (indexPosition+1)*oneIndexSize, chunk + indexPosition*oneIndexSize,  oneIndexSize*(egChunkVolume/2 - indexPosition + 1));
            // qDebug() << "chunk after memmove " << QByteArray(chunk, egChunkVolume*oneIndexSize).toHex() << FN;
        }

        memcpy(indexBA.data(), chunk, indexChunkSize);

        // write index key and offset
        localIndexStream.device()->seek(indexPosition*oneIndexSize);
        localIndexStream << theKey;
        localIndexStream << oldDataOffset;

        // qDebug() << "old chunk after all " << indexBA.toHex() << FN;

        memcpy(chunk, indexBA.constData(), indexChunkSize);

        localIndexStream.device()->seek(0);
        localIndexStream >> minKey;

        localIndexStream.device()->seek(egChunkVolume/2 * oneIndexSize);
        localIndexStream >> maxKey;

            // update fingers               
        fingersTree-> currentFinger.itemsCount = egChunkVolume/2+1;
        fingersTree-> currentFinger.minKey = minKey; // get first index
        fingersTree-> currentFinger.maxKey = maxKey; // get last index

        // qDebug() << "fingersTree-> currentFinger.minKey =  " << hex << (int) fingersTree-> currentFinger.minKey << ", fingersTree-> currentFinger.maxKey = " << hex << (int) fingersTree-> currentFinger.maxKey << FN;

        memcpy(indexBA.data(), new_chunk, indexChunkSize);

        localIndexStream.device()->seek(0);
        localIndexStream >> minKey;

        localIndexStream.device()->seek((egChunkVolume/2 - 1) * oneIndexSize);
        localIndexStream >> maxKey;

        fingersTree-> newFinger.itemsCount = egChunkVolume/2;
        fingersTree-> newFinger.minKey = minKey; // get first index
        fingersTree-> newFinger.maxKey = maxKey; // get last index

        // qDebug() << "newFinger.minKey =  " << hex << (int) fingersTree-> newFinger.minKey << ", newFinger.maxKey = " << hex << (int) fingersTree-> newFinger.maxKey << FN;

    }
    else // insert to second Part
    {
        indexPosition -= egChunkVolume/2;

        // check if not last position, move tail
        if (indexPosition != (egChunkVolume/2)) // last one
        {
            // qDebug() << "indexPosition = " << indexPosition << ", itemsCount = " << fingersTree-> currentFinger.itemsCount << FN;
            // qDebug() << "new_chunk before memmove" << QByteArray(new_chunk, egChunkVolume*oneIndexSize).toHex() << FN;
            memmove (new_chunk + (indexPosition+1)*oneIndexSize, new_chunk + indexPosition*oneIndexSize,  oneIndexSize*(egChunkVolume/2 - indexPosition + 1));
            // qDebug() << "new_chunk after memmove " << QByteArray(new_chunk, egChunkVolume*oneIndexSize).toHex() << FN;
        }

        memcpy(indexBA.data(), new_chunk, indexChunkSize);

            // write index key and offset
        localIndexStream.device()->seek(indexPosition*oneIndexSize);
        localIndexStream << theKey;
        localIndexStream << oldDataOffset;

        // qDebug() << "new_chunk after all " << indexBA.toHex() << FN;

        memcpy(new_chunk, indexBA.constData(), indexChunkSize);

            // update fingers

        localIndexStream.device()->seek(0);
        localIndexStream >> minKey;

        localIndexStream.device()->seek(egChunkVolume/2 * oneIndexSize);
        localIndexStream >> maxKey;

        fingersTree-> newFinger.itemsCount = egChunkVolume/2+1;
        fingersTree-> newFinger.minKey = minKey; // get first index
        fingersTree-> newFinger.maxKey = maxKey; // get last index

        // qDebug() << "newFinger.minKey =  " << hex << (int) fingersTree-> newFinger.minKey << ", newFinger.maxKey = " << hex << (int) fingersTree-> newFinger.maxKey << FN;

        memcpy(indexBA.data(), chunk, indexChunkSize);

        localIndexStream.device()->seek(0);
        localIndexStream >> minKey;

        localIndexStream.device()->seek((egChunkVolume/2 - 1) * oneIndexSize);
        localIndexStream >> maxKey;

        fingersTree-> currentFinger.itemsCount = egChunkVolume/2;
        fingersTree-> currentFinger.minKey = minKey; // get first index
        fingersTree-> currentFinger.maxKey = maxKey; // get last index

        // qDebug() << "fingersTree-> currentFinger.minKey =  " << hex << (int) fingersTree-> currentFinger.minKey << ", fingersTree-> currentFinger.maxKey = " << hex << (int) fingersTree-> currentFinger.maxKey << FN;
    }

        // update index chain pointers and counters
    quint64 fwdOffsetPtr;

    memcpy(indexBA.data(), chunk, indexChunkSize);

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    localIndexStream >> fwdOffsetPtr;

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    localIndexStream << fingersTree-> newFinger.nextChunkOffset;

    // localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream << (keysCountType) (fingersTree-> currentFinger.itemsCount);

    memcpy(chunk, indexBA.constData(), indexChunkSize);

    memcpy(indexBA.data(), new_chunk, indexChunkSize);

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize);
    localIndexStream << fingersTree-> currentFinger.nextChunkOffset;
    localIndexStream << fwdOffsetPtr;

    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream << (keysCountType) (fingersTree-> newFinger.itemsCount);

    memcpy(new_chunk, indexBA.constData(), indexChunkSize);

        // save both chunks
    indexesChunkOffset = fingersTree-> currentFinger.nextChunkOffset;
    StoreIndexChunk(chunk);
    indexesChunkOffset = fingersTree-> newFinger.nextChunkOffset;
    StoreIndexChunk(new_chunk);

        // update fingers tree
    fingersTree-> UpdateFingersChainAfterSplit(false);

    return 0;
}


template <typename KeyType> int EgIndexes<KeyType>::AppendIndexChunk(QDataStream& localIndexStream)
{
    fingersTree-> newFinger.nextChunkOffset = indexStream.device()->size(); // append to the end of indexes file

    fingersTree-> newFinger.itemsCount = 1;
    fingersTree-> newFinger.minKey = theKey;
    fingersTree-> newFinger.maxKey = theKey;

    memmove (new_chunk, zero_chunk, indexChunkSize);

        // update index chain pointers and counters
    quint64 fwdOffsetPtr;

    memcpy(indexBA.data(), chunk, indexChunkSize);

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    localIndexStream >> fwdOffsetPtr;

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    localIndexStream << fingersTree-> newFinger.nextChunkOffset;

    memcpy(chunk, indexBA.constData(), indexChunkSize);

    memcpy(indexBA.data(), new_chunk, indexChunkSize);

        // write index key and offset
    localIndexStream.device()->seek(0);
    localIndexStream << theKey;
    localIndexStream << oldDataOffset;

        // update pointers & count
    localIndexStream.device()->seek(egChunkVolume * oneIndexSize);
    localIndexStream << fingersTree-> currentFinger.nextChunkOffset;
    localIndexStream << fwdOffsetPtr;

    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream << (keysCountType) (fingersTree-> newFinger.itemsCount);

    // memcpy(new_chunk, indexBA.constData(), indexChunkSize);

        // save both chunks
    indexesChunkOffset = fingersTree-> currentFinger.nextChunkOffset;
    StoreIndexChunk(chunk);
    indexesChunkOffset = fingersTree-> newFinger.nextChunkOffset;
    StoreIndexChunk(indexBA.data());

        // update fingers tree
    fingersTree-> UpdateFingersChainAfterSplit(true);

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::FindIndexPosition(QDataStream& localIndexesStream)
{
    KeyType currentIndex;

        // min/max lookup
    if (theKey >=  fingersTree-> currentFinger.maxKey)
        return fingersTree-> currentFinger.itemsCount;

    else if (theKey < fingersTree-> currentFinger.minKey)
        return 0;

        // proportional index lookup
    indexPosition = (theKey - fingersTree-> currentFinger.minKey)*(fingersTree-> currentFinger.itemsCount)/(fingersTree-> currentFinger.maxKey - fingersTree-> currentFinger.minKey);

        // load proposed key
    localIndexesStream.device()->seek(indexPosition*oneIndexSize);
    localIndexesStream >> currentIndex;

        // compare
    if (currentIndex > theKey)
    {
            // move down until currentIndex <=
        do
        {
            indexPosition--;
            localIndexesStream.device()->seek(indexPosition*oneIndexSize);
            localIndexesStream >> currentIndex;
        }
        while (currentIndex > theKey);
        indexPosition++; // correction
    }
    else
    {
            // move up until currentIndex >
        do
        {
            indexPosition++;
            localIndexesStream.device()->seek(indexPosition*oneIndexSize);
            localIndexesStream >> currentIndex;
        }
        while (currentIndex <= theKey);
    }

    return indexPosition;
}

template <typename KeyType> int EgIndexes<KeyType>::FindPosByKeyFirst(QDataStream& localIndexesStream, CompareFunctionType myCompareFunc)
{
    KeyType currentIndex;

        // min/max lookup
    if (theKey >  fingersTree-> currentFinger.maxKey)
        return -1; // error

    if (theKey <= fingersTree-> currentFinger.minKey)
        return 0; // got it

        // proportional index lookup
    indexPosition = (theKey - fingersTree-> currentFinger.minKey)*(fingersTree-> currentFinger.itemsCount)/(fingersTree-> currentFinger.maxKey - fingersTree-> currentFinger.minKey);

        // load proposed key
    localIndexesStream.device()->seek(indexPosition*oneIndexSize);
    localIndexesStream >> currentIndex;

    // qDebug() << "indexPosition =  " << indexPosition << " ,currentIndex = " << hex << (int) currentIndex << FN;

        // compare
    if (myCompareFunc(currentIndex,theKey) && (indexPosition > 0)) // (currentIndex >= theKey)
    {
            // move down until currentIndex <
        do
        {
            indexPosition--;
            localIndexesStream.device()->seek(indexPosition*oneIndexSize);
            localIndexesStream >> currentIndex;
        }
        while (myCompareFunc(currentIndex,theKey) && (indexPosition > 0)); // (currentIndex >= theKey)

        if (! myCompareFunc(currentIndex,theKey) && (indexPosition < (fingersTree-> currentFinger.itemsCount-1))) // (currentIndex < theKey)
            indexPosition++; // correction
    }
    else if (! myCompareFunc(currentIndex,theKey) && (indexPosition < (fingersTree-> currentFinger.itemsCount-1))) // (currentIndex < theKey)
    {
            // move up until currentIndex >=
        do
        {
            indexPosition++;
            localIndexesStream.device()->seek(indexPosition*oneIndexSize);
            localIndexesStream >> currentIndex;
        }
        while (! myCompareFunc(currentIndex,theKey) && (indexPosition < (fingersTree-> currentFinger.itemsCount-1))); // (currentIndex < theKey)
    }

    // qDebug() << "indexPosition =  " << indexPosition << " ,currentIndex = " << hex << (int) currentIndex << FN;

    return indexPosition;
}

template <typename KeyType> int EgIndexes<KeyType>::FindPosByKeyLast(QDataStream &localIndexesStream, CompareFunctionType myCompareFunc)
{
    KeyType currentIndex;

        // min/max check
    if (theKey >=  fingersTree-> currentFinger.maxKey)
        return (fingersTree-> currentFinger.itemsCount - 1);

    if (theKey < fingersTree-> currentFinger.minKey)
        return -1; // error

        // proportional index lookup
    indexPosition = (theKey - fingersTree-> currentFinger.minKey)*(fingersTree-> currentFinger.itemsCount)/(fingersTree-> currentFinger.maxKey - fingersTree-> currentFinger.minKey);

        // load proposed key
    localIndexesStream.device()->seek(indexPosition*oneIndexSize);
    localIndexesStream >> currentIndex;

    // qDebug() << "indexPosition =  " << indexPosition << " ,currentIndex = " << hex << (int) currentIndex << FN;

        // compare
    if (myCompareFunc(currentIndex,theKey) && (indexPosition < (fingersTree-> currentFinger.itemsCount-1))) // (currentIndex <= theKey)
    {
            // move up until currentIndex >
        do
        {
            indexPosition++;
            localIndexesStream.device()->seek(indexPosition*oneIndexSize);
            localIndexesStream >> currentIndex;
        }
        while (myCompareFunc(currentIndex,theKey) && (indexPosition < (fingersTree-> currentFinger.itemsCount-1))); // (currentIndex <= theKey)

        if (! myCompareFunc(currentIndex,theKey)  && (indexPosition > 0)) // (currentIndex > theKey)
            indexPosition--; // correction
    }
    else if (! myCompareFunc(currentIndex,theKey)  && (indexPosition > 0)) // (currentIndex > theKey)
    {
            // move down until currentIndex <=
        do
        {
            indexPosition--;
            localIndexesStream.device()->seek(indexPosition*oneIndexSize);
            localIndexesStream >> currentIndex;
        }
        while (! myCompareFunc(currentIndex,theKey)  && (indexPosition > 0)); // (currentIndex > theKey)
    }

    // qDebug() << "indexPosition =  " << indexPosition << " ,currentIndex = " << hex << (int) currentIndex << FN;

    return indexPosition;
}


template <typename KeyType> void EgIndexes<KeyType>::LoadDataByChunkUp(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc)
{
    // KeyType currentIndex;
    quint64 dataOffset;

    quint64 nextChunkPtr;

    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(indexBA.data());

    indexPosition = FindPosByKeyFirst(localIndexStream, myCompareFunc);

    // qDebug() << "pos =  " << indexPosition << FN;

    while (indexPosition < fingersTree-> currentFinger.itemsCount)
    {
        localIndexStream.device()->seek(indexPosition*oneIndexSize + sizeof(KeyType));
        // localIndexStream >> currentIndex;
        localIndexStream >> dataOffset;

        index_offsets.insert(dataOffset);

        indexPosition++;
    }

        // get next chunk
    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64)));
    localIndexStream >> nextChunkPtr;

    while (nextChunkPtr)
    {
        indexesChunkOffset = nextChunkPtr;
        LoadIndexChunk(indexBA.data());

            // get chunk items count
        localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
        localIndexStream >> chunkCount;

        // qDebug() << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

        indexPosition = 0;

        while (indexPosition < chunkCount)
        {
            localIndexStream.device()->seek(indexPosition*oneIndexSize + sizeof(KeyType));
            // localIndexStream >> currentIndex;
            localIndexStream >> dataOffset;

            index_offsets.insert(dataOffset);

            indexPosition++;
        }

            // get next chunk ptr
        localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64)));
        localIndexStream >> nextChunkPtr;
    }
}


template <typename KeyType> void EgIndexes<KeyType>::LoadDataByChunkDown(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc)
{
    // KeyType currentIndex;
    quint64 dataOffset;

    quint64 nextChunkPtr;

    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(indexBA.data());

    indexPosition = FindPosByKeyLast(localIndexStream, myCompareFunc);

    if (indexPosition < 0)
        return; // error

    // qDebug() << "pos =  " << indexPosition << FN;

    while (indexPosition >= 0)
    {
        localIndexStream.device()->seek(indexPosition*oneIndexSize + sizeof(KeyType));
        // localIndexStream >> currentIndex;
        localIndexStream >> dataOffset;

        index_offsets.insert(dataOffset);

        indexPosition--;
    }

        // get next chunk
    localIndexStream.device()->seek(egChunkVolume * oneIndexSize);
    localIndexStream >> nextChunkPtr;

    while (nextChunkPtr)
    {
        indexesChunkOffset = nextChunkPtr;
        LoadIndexChunk(indexBA.data());

            // get chunk items count
        localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
        localIndexStream >> chunkCount;

        // qDebug() << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

        indexPosition = chunkCount - 1;

        while (indexPosition >= 0)
        {
            localIndexStream.device()->seek(indexPosition*oneIndexSize + sizeof(KeyType));
            // localIndexStream >> currentIndex;
            localIndexStream >> dataOffset;

            index_offsets.insert(dataOffset);

            indexPosition--;
        }

            // get next chunk ptr
        localIndexStream.device()->seek(egChunkVolume * oneIndexSize);
        localIndexStream >> nextChunkPtr;
    }
}

template <typename KeyType> void EgIndexes<KeyType>::LoadDataByChunkEqual(QSet<quint64>& index_offsets)
{
    KeyType currentIndex;
    quint64 dataOffset;

    quint64 nextChunkPtr;

    bool keyOutOfRange = false;
    bool firstChunk = true;

    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);
/*
    LoadIndexChunk(indexBA.data());

    // memcpy(indexBA.data(), chunk, indexChunkSize);

    // qDebug() << "indexBA =  " << indexBA.toHex() << FN;

    indexPosition = FindPosByKeyFirst(localIndexStream, CompareGE); // FIXME

    // qDebug() << "pos =  " << indexPosition << FN;

    do
    {
        localIndexStream.device()->seek(indexPosition*oneIndexSize);
        localIndexStream >> currentIndex;
        localIndexStream >> dataOffset;

        if (currentIndex == theKey)
            index_offsets.insert(dataOffset);
        else
            keyOutOfRange = true;

        indexPosition++;
    }
    while ((! keyOutOfRange) && (indexPosition < fingersTree-> currentFinger.itemsCount));

    // keyOutOfRange &= isExact; // if (! keyOutOfRange)
    // {
        // get next chunk
    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64)));
    localIndexStream >> nextChunkPtr;
    */

    nextChunkPtr =  indexesChunkOffset;

    // qDebug() << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

    while (nextChunkPtr && (! keyOutOfRange))
    {
        indexesChunkOffset = nextChunkPtr;
        LoadIndexChunk(indexBA.data());
        // memcpy(indexBA.data(), chunk, indexChunkSize);

        if (firstChunk)
        {
           indexPosition = FindPosByKeyFirst(localIndexStream, CompareGE);
           firstChunk = false;

           chunkCount = fingersTree-> currentFinger.itemsCount;
        }
        else
        {
            indexPosition = 0;

                // get chunk items count
            localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
            localIndexStream >> chunkCount;
        }

        // qDebug() << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

        // indexPosition = 0;

        do
        {
            localIndexStream.device()->seek(indexPosition*oneIndexSize);
            localIndexStream >> currentIndex;
            localIndexStream >> dataOffset;

            if (currentIndex == theKey)
                index_offsets.insert(dataOffset);
            else
                keyOutOfRange = true;

            indexPosition++;
        }
        while ((! keyOutOfRange) && (indexPosition < chunkCount));

            // get next chunk ptr
        localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64)));
        localIndexStream >> nextChunkPtr;
    }
}


template <typename KeyType> int EgIndexes<KeyType>::FindIndexByDataOffset(QDataStream &localIndexStream)
{
    KeyType currentIndex;
    quint64 dataOffset;

    quint64 nextChunkPtr;

    bool keyOutOfRange = false;
    bool firstChunk = true;

    // QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(indexBA.data());

    // memcpy(indexBA.data(), chunk, indexChunkSize);

    // qDebug() << "indexBA =  " << indexBA.toHex() << FN;

    nextChunkPtr =  indexesChunkOffset;

    // qDebug() << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

    while (nextChunkPtr && (! keyOutOfRange))
    {
        // memcpy(indexBA.data(), chunk, indexChunkSize);

        if (firstChunk)
        {
           indexPosition = FindPosByKeyFirst(localIndexStream, CompareGE);
           firstChunk = false;

           chunkCount = fingersTree-> currentFinger.itemsCount;
        }
        else
        {
            indexPosition = 0;

                // get chunk items count
            localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
            localIndexStream >> chunkCount;
        }

        // qDebug() << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

        // indexPosition = 0;

        do
        {
            localIndexStream.device()->seek(indexPosition*oneIndexSize);
            localIndexStream >> currentIndex;
            localIndexStream >> dataOffset;

            // qDebug() << "currentIndex =  " << hex << (int) currentIndex  << " , dataOffset =  " << hex << (int) dataOffset << FN;

            if (currentIndex == theKey)
            {
                if (dataOffset == oldDataOffset)
                    return 0; // found indexPosition
            }
            else
                keyOutOfRange = true;

            indexPosition++;
        }
        while ((! keyOutOfRange) && (indexPosition < chunkCount));

            // get next chunk ptr
        localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64)));
        localIndexStream >> nextChunkPtr;

        if (nextChunkPtr)
        {
            indexesChunkOffset = nextChunkPtr;
            LoadIndexChunk(indexBA.data());
        }
    }

    return 1; // not found
}

template <typename KeyType> int EgIndexes<KeyType>::UpdateDataOffset(QDataStream &localIndexStream)
{
    localIndexStream.device()->seek(indexPosition*oneIndexSize + sizeof(KeyType));
    localIndexStream << newDataOffset;

    StoreIndexChunk(indexBA.data());

    return 0;
}


template <typename KeyType> int EgIndexes<KeyType>::DeleteDataOffset(QDataStream &localIndexStream)
{
    int newMaxKey, newMinKey;

        // get count
    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream >> chunkCount;

    // memcpy(chunk, indexBA.data(), indexChunkSize);

    // qDebug() << "indexPosition =  " << hex << (int) indexPosition << " ,chunkCount =  " << hex << (int) chunkCount << FN;

        // if not last, move tail
    if (indexPosition < (chunkCount - 1)) // FIXME move tail
    {
        // qDebug() << "indexPosition = " << indexPosition << ", itemsCount = " << chunkCount << FN;
        // qDebug() << "chunk before memmove" << indexBA.toHex() << FN;
        memmove (indexBA.data() + indexPosition*oneIndexSize, indexBA.data() + (indexPosition+1)*oneIndexSize,  oneIndexSize*(chunkCount - indexPosition - 1));
    }

        // write zeroes
    memcpy(indexBA.data() + (chunkCount - 1)*oneIndexSize, zero_chunk, oneIndexSize);

    --chunkCount; // DECREASE

    fingersTree-> minValueChanged = false;
    fingersTree-> maxValueChanged = false;

        // get new min/max
    if (indexPosition == 0)
    {
        fingersTree-> minValueChanged = true;

        localIndexStream.device()->seek(0);
        localIndexStream >> fingersTree->newMinValue;
    }
    else if (indexPosition == chunkCount)
    {
        fingersTree-> maxValueChanged = true;

        localIndexStream.device()->seek((chunkCount-1)*oneIndexSize);
        localIndexStream >> fingersTree->newMaxValue;
    }

        // write decreased count
    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream << chunkCount;

    // qDebug() << "chunk after memmove " << indexBA.toHex() << FN;

        // save
    StoreIndexChunk(indexBA.data());

    return 0;
}

template <typename KeyType> void EgIndexes<KeyType>::UpdateIndex()
{
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    if (FindIndexByDataOffset(localIndexStream) == 0) // index found
        UpdateDataOffset(localIndexStream);
    // else
    // TODO error

}

template <typename KeyType> void EgIndexes<KeyType>::DeleteIndex()
{
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    if (FindIndexByDataOffset(localIndexStream) == 0) // index found
    {

            // get parent ptr
        localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2) + sizeof(keysCountType));
        localIndexStream >> fingersTree-> parentFingerOffset;

        // qDebug() << "delete index of parent offset: " << hex << (int) fingersTree-> parentFingerOffset << FN;

        // qDebug() << "chunkCount =  " << hex << (int) chunkCount << FN;

        if (chunkCount > 1)
        {
            DeleteDataOffset(localIndexStream);

            fingersTree-> UpdateFingerAfterDelete();
        }
        else if (chunkCount == 1)
        {
            RemoveChunkFromChain();

            fingersTree-> DeleteFinger();
        }
        // TODO else error
    }
    else
        qDebug() << "index not found " << FN;

}


template <typename KeyType> void EgIndexes<KeyType>::RemoveChunkFromChain()
{
    quint64 prevChunkPtr, nextChunkPtr;

    // TODO get pointers

    if (prevChunkPtr)
    {
        // get ptr to update
    }

    if (nextChunkPtr)
    {
        // get ptr to update
    }

    // TODO fill chunk by zeroes and add to vacant chunks chain

}

template class EgIndexes<qint32>; // for unit tests
