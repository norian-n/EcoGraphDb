/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "../egDataNodesType.h"
#include "egIndexes.h"
#include "egFingers.h"

using namespace egIndexesNamespace;

template <typename KeyType> void EgIndexes<KeyType>::RemoveIndexFiles(const QString& IndexFileName)
{
    if (indexFile.isOpen())
        indexFile.close();

    indexFile.setFileName(IndexFileName + ".odx");
    indexFile.remove();
}

template <typename KeyType> void EgIndexes<KeyType>::PrintIndexesChunk(char* theChunk, const QString &theMessage)
{
    qDebug() << QByteArray(theChunk, indexChunkSize).toHex() << theMessage;
}

template <typename KeyType> int EgIndexes<KeyType>::OpenIndexFilesToUpdate(const QString& IndexFileName)
{
    indexFile.close();

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
    indexFile.close();

    indexFile.setFileName(IndexFileName + ".odx");
    indexStream.setDevice(&indexFile);

    if (!indexFile.exists())
    {
        qDebug() << FN << "file doesn't exist " << IndexFileName + ".odx";
        return -1;
    }

    if (!indexFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        if (! IndexFileName.contains(EgDataNodesGUInamespace::egGUIfileName))
            qDebug() << FN << "can't open index file " << indexFile.fileName();
        return -2;
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

        // parent finger offset
    localIndexStream << (quint64) fingersTree-> rootHeaderSize;

    memcpy(chunk, indexBA.constData(), indexChunkSize);

    indexesChunkOffset = indexHeaderSize;
}


template <typename KeyType> void EgIndexes<KeyType>::InitRootHeader()
{
    /*fingersTree-> fingersRootHeader.minKey = theKey;
    fingersTree-> fingersRootHeader.maxKey = theKey;
    fingersTree-> fingersRootHeader.myLevel = 1;
    fingersTree-> fingersRootHeader.itemsCount = 1;
    fingersTree-> fingersRootHeader.nextChunkOffset = 0;
    fingersTree-> fingersRootHeader.myChunkOffset = 0;
    fingersTree-> fingersRootHeader.myOffset = 0;
    */
}


template <typename KeyType> void EgIndexes<KeyType>::LoadRootHeader()
{
    /*
    indexStream.device()->seek(0);

    indexStream >> fingersTree-> fingersRootHeader.minKey;
    indexStream >> fingersTree-> fingersRootHeader.maxKey;
    indexStream >> fingersTree-> fingersRootHeader.myLevel;
    indexStream >> fingersTree-> fingersRootHeader.itemsCount;
    indexStream >> fingersTree-> fingersRootHeader.nextChunkOffset;
    */
}

template <typename KeyType> void EgIndexes<KeyType>::StoreRootHeader()
{
    indexStream.device()->seek(0);

    indexStream << (quint64) indexHeaderSize;
    indexStream << (quint64) 0xABCDEF; // reserved for chunks recycling
/*
    indexStream << (KeyType) fingersTree-> fingersRootHeader.minKey;
    indexStream << (KeyType) fingersTree-> fingersRootHeader.maxKey;

    if (! minMaxOnly)
    {
        indexStream << (fingersLevelType) fingersTree-> fingersRootHeader.myLevel;
        indexStream << (keysCountType) fingersTree-> fingersRootHeader.itemsCount;
        indexStream << (quint64) fingersTree-> fingersRootHeader.nextChunkOffset;
    }
    */
}

template <typename KeyType> int EgIndexes<KeyType>::StoreFingerOffset(quint64 chunkOffset, quint64 fingerOffset)
{
    indexStream.device()->seek(chunkOffset + egChunkVolume * oneIndexSize + sizeof(quint64) * 2 + sizeof(keysCountType));
    indexStream << fingerOffset;

    /*
    qDebug() << "chunkOffset = " << hex << (int) chunkOffset
             << " , target offset = " << hex << (int) (chunkOffset + egChunkVolume * oneIndexSize + sizeof(quint64) * 2 + sizeof(keysCountType))
             << " , fingerOffset = " << hex << (int) fingerOffset << FN;
    */

    return 0; // FIXME
}

/*
template <typename KeyType> int EgIndexes<KeyType>::StoreFingerOffset(quint64 fingerOffset)
{
    indexStream.device()->seek(indexesChunkOffset + egChunkVolume * oneIndexSize + sizeof(quint64) * 2 + sizeof(keysCountType));
    indexStream << fingerOffset;

    // qDebug() << "indexesChunkOffset = " << hex << (int) indexesChunkOffset
    //         << ", fingerOffset = " << hex << (int) fingerOffset << FN;

    return 0; // FIXME
}
*/

template <typename KeyType> int EgIndexes<KeyType>::StoreIndexChunk(char* chunkPtr)
{
    indexStream.device()->seek(indexesChunkOffset);
    indexStream.writeRawData(chunkPtr, indexChunkSize);

    if (! indexesChunkOffset)
        qDebug() << "ERROR: zero indexesChunkOffset" << FN;

    // qDebug() << "indexesChunkOffset = " << hex << (int) indexesChunkOffset << FN;

    return 0; // FIXME
}

template <typename KeyType> int EgIndexes<KeyType>::StoreIndexChunk(quint64 chunkOffset, char* chunkPtr)
{
    indexStream.device()->seek(chunkOffset);
    indexStream.writeRawData(chunkPtr, indexChunkSize);

    // qDebug() << "indexesChunkOffset = " << hex << (int) indexesChunkOffset << FN;

    return 0; // FIXME
}


template <typename KeyType> inline void EgIndexes<KeyType>::LoadIndexChunk(char* chunkPtr)
{
    indexStream.device()->seek(indexesChunkOffset);
    indexStream.readRawData(chunkPtr, indexChunkSize);

    if (! indexesChunkOffset)
        qDebug() << "ERROR: zero indexesChunkOffset" << FN;


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

    memcpy(indexBA.data(), chunk, indexChunkSize);

    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger);

    if (fingersTree-> currentFinger.itemsCount < egChunkVolume)
    {
        // indexPosition = FindIndexPosition(localIndexStream);

        indexPosition = FindPosByKeyLast(localIndexStream, EgIndexes<KeyType>::CompareLE) + 1;
        // qDebug() << "indexPosition = " << indexPosition << ", itemsCount = " << fingersTree-> currentFinger.itemsCount << FN;

        if (indexPosition < 0)
            indexPosition = 0; // FIXME check

            // check if not last position, move tail
        if (indexPosition < (fingersTree-> currentFinger.itemsCount)) // not last one
        {
            // qDebug() << "chunk before memmove" << QByteArray(chunk, egChunkVolume*oneIndexSize).toHex() << FN;
            memmove (chunk + (indexPosition+1)*oneIndexSize, chunk + indexPosition*oneIndexSize,  oneIndexSize*(fingersTree-> currentFinger.itemsCount - indexPosition)); //  + 1
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
        if (theKey >= fingersTree-> currentFinger.maxKey)
            AppendIndexChunk(localIndexStream);
        else
            SplitIndexChunk(localIndexStream);
    }

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::SplitIndexChunk(QDataStream& localIndexStream)
{
    // KeyType minKey, maxKey;

    fingersTree-> newFinger.nextChunkOffset = indexStream.device()->size(); // append to the end of indexes file

    // indexPosition = FindIndexPosition(localIndexStream);

    indexPosition = FindPosByKeyLast(localIndexStream, EgIndexes<KeyType>::CompareLE) + 1;

    // qDebug() << "indexPosition = " << indexPosition << ", itemsCount = " << fingersTree-> currentFinger.itemsCount << FN;

    // PrintIndexesChunk(chunk, "old chunk loaded " + FNS);

        // init new chunk
    memmove (new_chunk, zero_chunk, indexChunkSize);

        // split chunks

    // qDebug() << "new chunk before memmove" << QByteArray(new_chunk, egChunkVolume*oneIndexSize).toHex() << FN;
    memmove (new_chunk, chunk + egChunkVolume/2*oneIndexSize,  egChunkVolume/2*oneIndexSize);
    // qDebug() << "new chunk after split up" << QByteArray(new_chunk, egChunkVolume*oneIndexSize).toHex() << FN;
        // add zeroes to first
    memmove (chunk + egChunkVolume/2*oneIndexSize, zero_chunk,  egChunkVolume/2*oneIndexSize);
    // PrintIndexesChunk(chunk, "old chunk after split up " + FNS);


        // recalc position to splitted chunks

    if ( indexPosition < (egChunkVolume/2+1)) // insert to first part
    {
        // check if not last position, move tail
        if (indexPosition < (egChunkVolume/2)) // last one
        {
            // qDebug() << "indexPosition = " << indexPosition << ", itemsCount = " << fingersTree-> currentFinger.itemsCount << FN;
            // qDebug() << "chunk before memmove" << QByteArray(chunk, egChunkVolume*oneIndexSize).toHex() << FN;
            memmove (chunk + (indexPosition+1)*oneIndexSize, chunk + indexPosition*oneIndexSize,  oneIndexSize*(egChunkVolume/2 - indexPosition)); //  + 1
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
        localIndexStream >> fingersTree-> currentFinger.minKey;

        localIndexStream.device()->seek(egChunkVolume/2 * oneIndexSize);
        localIndexStream >> fingersTree-> currentFinger.maxKey;

            // update fingers               
        fingersTree-> currentFinger.itemsCount = egChunkVolume/2+1;

        // qDebug() << "fingersTree-> currentFinger.minKey =  " << hex << (int) fingersTree-> currentFinger.minKey << ", fingersTree-> currentFinger.maxKey = " << hex << (int) fingersTree-> currentFinger.maxKey << FN;

        memcpy(indexBA.data(), new_chunk, indexChunkSize);

        localIndexStream.device()->seek(0);
        localIndexStream >> fingersTree-> newFinger.minKey;

        localIndexStream.device()->seek((egChunkVolume/2 - 1) * oneIndexSize);
        localIndexStream >> fingersTree-> newFinger.maxKey;

        fingersTree-> newFinger.itemsCount = egChunkVolume/2;

        // qDebug() << "newFinger.minKey =  " << hex << (int) fingersTree-> newFinger.minKey << ", newFinger.maxKey = " << hex << (int) fingersTree-> newFinger.maxKey << FN;

    }
    else // insert to second Part
    {
        indexPosition -= egChunkVolume/2;

        // check if not last position, move tail
        if (indexPosition < (egChunkVolume/2)) // last one
        {
            // qDebug() << "indexPosition = " << indexPosition << ", itemsCount = " << fingersTree-> currentFinger.itemsCount << FN;
            // qDebug() << "new_chunk before memmove" << QByteArray(new_chunk, egChunkVolume*oneIndexSize).toHex() << FN;
            memmove (new_chunk + (indexPosition+1)*oneIndexSize, new_chunk + indexPosition*oneIndexSize,  oneIndexSize*(egChunkVolume/2 - indexPosition)); //  + 1
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
        localIndexStream >> fingersTree-> newFinger.minKey;

        localIndexStream.device()->seek(egChunkVolume/2 * oneIndexSize);
        localIndexStream >> fingersTree-> newFinger.maxKey;

        fingersTree-> newFinger.itemsCount = egChunkVolume/2+1;

        // qDebug() << "newFinger.minKey =  " << hex << (int) fingersTree-> newFinger.minKey << ", newFinger.maxKey = " << hex << (int) fingersTree-> newFinger.maxKey << FN;

        memcpy(indexBA.data(), chunk, indexChunkSize);

        localIndexStream.device()->seek(0);
        localIndexStream >> fingersTree-> currentFinger.minKey;

        localIndexStream.device()->seek((egChunkVolume/2 - 1) * oneIndexSize);
        localIndexStream >> fingersTree-> currentFinger.maxKey;

        fingersTree-> currentFinger.itemsCount = egChunkVolume/2;

        // qDebug() << "fingersTree-> currentFinger.minKey =  " << hex << (int) fingersTree-> currentFinger.minKey << ", fingersTree-> currentFinger.maxKey = " << hex << (int) fingersTree-> currentFinger.maxKey << FN;
    }

        // update index chain pointers and counters


    // qDebug() << "current chunk " << hex << (int) fingersTree-> currentFinger.nextChunkOffset
    //          << " ,next chunk "  << hex << (int) fingersTree-> newFinger.nextChunkOffset << FN;

    memcpy(indexBA.data(), chunk, indexChunkSize);

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    // localIndexStream >> prevOffsetPtr;
    localIndexStream >> nextOffsetPtr;                                                   // store next ptr

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    localIndexStream << fingersTree-> newFinger.nextChunkOffset;                        // new next ptr

    // qDebug() << "current next ptr =  " << hex << (int) fingersTree-> newFinger.nextChunkOffset << " ,prev ptr =  " << hex << (int) prevOffsetPtr << FN;

    // localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream << (keysCountType) (fingersTree-> currentFinger.itemsCount);

    memcpy(chunk, indexBA.constData(), indexChunkSize);

    memcpy(indexBA.data(), new_chunk, indexChunkSize);

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize);
    localIndexStream << fingersTree-> currentFinger.nextChunkOffset;    // prev ptr
    localIndexStream << nextOffsetPtr;                                  // stored next ptr

    // qDebug() << "new prev ptr =  " << hex << (int) fingersTree-> currentFinger.nextChunkOffset << ", next ptr = " << hex << (int) nextOffsetPtr << FN;

    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream << (keysCountType) (fingersTree-> newFinger.itemsCount);

    memcpy(new_chunk, indexBA.constData(), indexChunkSize);

        // update backlink
    if (nextOffsetPtr)
    {
        indexStream.device()->seek(nextOffsetPtr + egChunkVolume * oneIndexSize);
        indexStream << fingersTree-> newFinger.nextChunkOffset;
    }

    // PrintIndexesChunk(chunk, "old chunk before save " + FNS);
    // PrintIndexesChunk(new_chunk, "new chunk before save " + FNS);

        // save both chunks
    indexesChunkOffset = fingersTree-> currentFinger.nextChunkOffset;
    StoreIndexChunk(chunk);
    indexesChunkOffset = fingersTree-> newFinger.nextChunkOffset;
    StoreIndexChunk(new_chunk);

        // update fingers tree
    fingersTree-> UpdateFingersChainAfterSplit(); // false);

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

    // qDebug() << "current chunk " << hex << (int) fingersTree-> currentFinger.nextChunkOffset
    //         << " ,next chunk "  << hex << (int) fingersTree-> newFinger.nextChunkOffset << FN;

    memcpy(indexBA.data(), chunk, indexChunkSize);

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    // localIndexStream >> prevOffsetPtr;
    localIndexStream >> nextOffsetPtr;                                                      // store next ptr

    // qDebug() << "current next ptr =  " << hex << (int) fingersTree-> newFinger.nextChunkOffset << " ,prev ptr =  " << hex << (int) prevOffsetPtr << FN;

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    localIndexStream << fingersTree-> newFinger.nextChunkOffset;                            // new next ptr

    memcpy(chunk, indexBA.constData(), indexChunkSize);

    memcpy(indexBA.data(), new_chunk, indexChunkSize);

        // write index key and offset
    localIndexStream.device()->seek(0);
    localIndexStream << theKey;
    localIndexStream << oldDataOffset;

        // update pointers & count
    localIndexStream.device()->seek(egChunkVolume * oneIndexSize);
    localIndexStream << fingersTree-> currentFinger.nextChunkOffset;        // prev ptr
    localIndexStream << nextOffsetPtr;                                      // stored next ptr

    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream << (keysCountType) (fingersTree-> newFinger.itemsCount);

        // update backlink
    if (nextOffsetPtr)
    {
        indexStream.device()->seek(nextOffsetPtr + egChunkVolume * oneIndexSize);
        indexStream << fingersTree-> newFinger.nextChunkOffset;
    }

    // memcpy(new_chunk, indexBA.constData(), indexChunkSize);

        // save both chunks
    indexesChunkOffset = fingersTree-> currentFinger.nextChunkOffset;
    StoreIndexChunk(chunk);
    indexesChunkOffset = fingersTree-> newFinger.nextChunkOffset;
    StoreIndexChunk(indexBA.data());

        // update fingers tree
    fingersTree-> UpdateFingersChainAfterSplit(); // true);

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
    if ((fingersTree-> currentFinger.maxKey > fingersTree-> currentFinger.minKey))
        indexPosition = (theKey - fingersTree-> currentFinger.minKey)*(fingersTree-> currentFinger.itemsCount - 1)/(fingersTree-> currentFinger.maxKey - fingersTree-> currentFinger.minKey);
    else
       currentIndex = fingersTree-> currentFinger.itemsCount - 1;

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
    if ((theKey >  fingersTree-> currentFinger.maxKey)|| (fingersTree-> currentFinger.itemsCount <= 0))
    {
        qDebug() << "ERROR bad finger of " << indexFile.fileName() << " for Key = " << theKey << FN;

        fingersTree->PrintFingerInfo(fingersTree-> currentFinger, "currentFinger");
        return -1; // error
    }

    if ((theKey < fingersTree-> currentFinger.minKey) || (fingersTree-> currentFinger.itemsCount == 1))
    {
            // load proposed key
        localIndexesStream.device()->seek(0);
        localIndexesStream >> currentIndex;

        if (myCompareFunc(currentIndex,theKey))
            return 0; // got it
        else
            return -1;
    }

        // proportional index lookup
    if (fingersTree-> currentFinger.maxKey > fingersTree-> currentFinger.minKey)
        indexPosition = (theKey - fingersTree-> currentFinger.minKey)*(fingersTree-> currentFinger.itemsCount - 1)/(fingersTree-> currentFinger.maxKey - fingersTree-> currentFinger.minKey);
    else
    {
        if (myCompareFunc(fingersTree-> currentFinger.minKey, theKey))
            return 0; // got it
        else
            return -1;
    }

    // qDebug() << "indexPosition =  " << indexPosition << FN;


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
    /*
        // load proposed key
    localIndexesStream.device()->seek(indexPosition*oneIndexSize);
    localIndexesStream >> currentIndex;

    if (myCompareFunc(currentIndex,theKey))
        return indexPosition;
    else
        return -1;
        */

    return indexPosition;
}

template <typename KeyType> int EgIndexes<KeyType>::FindPosByKeyLast(QDataStream &localIndexesStream, CompareFunctionType myCompareFunc)
{

    KeyType currentIndex;

        // min/max check
    if ((theKey < fingersTree-> currentFinger.minKey) || (fingersTree-> currentFinger.itemsCount <= 0))
    {
        // qDebug() << "ERROR bad finger for Key = " << theKey << FN;

        // fingersTree->PrintFingerInfo(fingersTree-> currentFinger, "currentFinger");
        return -1; // error or between fingers
    }

    if ((theKey >  fingersTree-> currentFinger.maxKey) || (fingersTree-> currentFinger.itemsCount == 1))
    {
            // load proposed key
        localIndexesStream.device()->seek((fingersTree-> currentFinger.itemsCount - 1)*oneIndexSize);
        localIndexesStream >> currentIndex;

        /*
        qDebug() << "indexPosition 0 =  " << indexPosition
                 << " ,myCompareFunc =  " << myCompareFunc(currentIndex,theKey)
                 << " ,currentIndex = " << (int) currentIndex << " ,theKey = " << (int) theKey << FN;
        */

        if (myCompareFunc(currentIndex,theKey))
            return (fingersTree-> currentFinger.itemsCount - 1); // got it
        else
            return -1;
    }

        // proportional index lookup
    if (fingersTree-> currentFinger.maxKey > fingersTree-> currentFinger.minKey)
        indexPosition = (theKey - fingersTree-> currentFinger.minKey)*(fingersTree-> currentFinger.itemsCount - 1)/(fingersTree-> currentFinger.maxKey - fingersTree-> currentFinger.minKey);
    else
    {
        if (myCompareFunc(fingersTree-> currentFinger.maxKey, theKey))
            return (fingersTree-> currentFinger.itemsCount - 1); // got it
        else
            return -1;
    }

        // load proposed key
    localIndexesStream.device()->seek(indexPosition*oneIndexSize);
    localIndexesStream >> currentIndex;

    /*
    qDebug() << "indexPosition 1 =  " << indexPosition
             << " ,myCompareFunc =  " << myCompareFunc(currentIndex,theKey)
             << " ,currentIndex = " << (int) currentIndex << " ,theKey = " << (int) theKey << FN;
    */

        // compare
    if (myCompareFunc(currentIndex,theKey) && (indexPosition < (fingersTree-> currentFinger.itemsCount-1))) // (currentIndex <= theKey)
    {
            // move up until currentIndex >
        do
        {
            indexPosition++;
            localIndexesStream.device()->seek(indexPosition*oneIndexSize);
            localIndexesStream >> currentIndex;

            // qDebug() << "indexPosition 2 =  " << indexPosition << " ,currentIndex = " << hex << (int) currentIndex << FN;
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

    /*
    qDebug() << "indexPosition 3 =  " << indexPosition
             << " ,myCompareFunc =  " << myCompareFunc(currentIndex,theKey)
             << " ,currentIndex = " << (int) currentIndex << " ,theKey = " << (int) theKey << FN;
    */

    return indexPosition;
}


template <typename KeyType> void EgIndexes<KeyType>::LoadAllData(QSet<quint64>& index_offsets)
{
        // get first index offset from header
    indexStream.device()->seek(0);
    indexStream >> indexesChunkOffset;

        // load chunk
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(indexBA.data());

    indexPosition = 0;

    // qDebug() << "pos =  " << indexPosition << FN;

    LoadDataUp(index_offsets, localIndexStream);

}

template <typename KeyType> void EgIndexes<KeyType>::LoadDataByChunkUp(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc)
{
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(indexBA.data());

    indexPosition = FindPosByKeyFirst(localIndexStream, myCompareFunc);

    // qDebug() << "pos =  " << indexPosition << FN;

    if (indexPosition >= 0)
        LoadDataUp(index_offsets, localIndexStream);
}

template <typename KeyType> void EgIndexes<KeyType>::LoadDataUp(QSet<quint64>& index_offsets, QDataStream& localIndexStream)
{
    quint64 dataOffset;

    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream >> chunkCount;

    while (indexPosition < chunkCount)
    {
        localIndexStream.device()->seek(indexPosition*oneIndexSize + sizeof(KeyType));
        // localIndexStream >> currentIndex;
        localIndexStream >> dataOffset;

        index_offsets.insert(dataOffset);

        indexPosition++;
    }

        // get next chunk
    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64)));
    localIndexStream >> nextOffsetPtr;

    while (nextOffsetPtr)
    {
        indexesChunkOffset = nextOffsetPtr;
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
        localIndexStream >> nextOffsetPtr;
    }
}


template <typename KeyType> int EgIndexes<KeyType>::LoadDataByChunkDown(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc)
{
    // KeyType currentIndex;
    quint64 dataOffset;

    // quint64 nextChunkPtr;

    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(indexBA.data());

    // qDebug() << "indexBA =  " << indexBA.toHex() << FN;

    indexPosition = FindPosByKeyLast(localIndexStream, myCompareFunc);

    // qDebug() << "pos =  " << indexPosition << FN;

    if (indexPosition < 0)
        return indexPosition; // not found

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
    localIndexStream >> prevOffsetPtr;

    // qDebug() << "prevOffsetPtr =  " << hex << (int) prevOffsetPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

    while (prevOffsetPtr)
    {
        indexesChunkOffset = prevOffsetPtr;
        LoadIndexChunk(indexBA.data());

            // get chunk items count
        localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
        localIndexStream >> chunkCount;

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
        localIndexStream >> prevOffsetPtr;
    }

    return 0;
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
           if (indexPosition < 0)
              indexPosition = 0;    // FIXME

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

    // qDebug() << "theKey =  " << hex << (int) theKey  << " , oldDataOffset =  " << hex << (int) oldDataOffset << FN;

    // qDebug() << "indexBA =  " << indexBA.toHex() << FN;

    nextChunkPtr =  indexesChunkOffset;

    // qDebug() << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

    while (nextChunkPtr && (! keyOutOfRange))
    {
        // memcpy(indexBA.data(), chunk, indexChunkSize);

        if (firstChunk)
        {
           indexPosition = FindPosByKeyFirst(localIndexStream, CompareGE);
           if (indexPosition < 0)
              indexPosition = 0;    // FIXME

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

        // qDebug() << "indexPosition =  " << indexPosition;

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

    // qDebug() << "theKey = " << theKey << ", IndexFileName = " << fingersTree-> IndexFileName << FN;

    if (FindIndexByDataOffset(localIndexStream) == 0) // index found
        UpdateDataOffset(localIndexStream);
    else
        qDebug() << "Index to update not found, theKey = " << theKey << ", IndexFileName = " << fingersTree-> IndexFileName << FN;

}

template <typename KeyType> void EgIndexes<KeyType>::DeleteIndex()
{
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    // qDebug() << "theKey = " << theKey << ", IndexFileName = " << fingersTree-> IndexFileName << FN;

    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger, "DeleteIndex");

    if (FindIndexByDataOffset(localIndexStream) == 0) // index found
    {
        // qDebug() << "chunkCount 1 =  " << hex << (int) chunkCount << FN;

            // get finger ptr
        localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2) + sizeof(keysCountType));
        localIndexStream >> fingersTree-> currentFingerOffset;

        if (chunkCount > 1)
        {
            DeleteDataOffset(localIndexStream); // count decrase

            // qDebug() << "chunkCount 2 =  " << hex << (int) chunkCount << FN;
            // qDebug() << "fingersTree-> currentFingerOffset " << hex << (int) fingersTree-> currentFingerOffset << FN;

            fingersTree-> currentKeysCount = chunkCount;
            fingersTree-> UpdateFingerCountAfterDelete();

            if (fingersTree-> minValueChanged)
                fingersTree-> UpdateMinValueUp();
            else if (fingersTree-> maxValueChanged)
                fingersTree-> UpdateMaxValueUp();
        }
        else if (chunkCount == 1)
        {

            // qDebug() << "fingersTree-> currentFingerOffset " << hex << (int) fingersTree-> currentFingerOffset << FN;

            fingersTree-> DeleteParentFinger(); // probably recursive

            // fingersTree-> PrintAllChunksInfo("After finger delete" + FNS);

            RemoveChunkFromChain();
        }
        else
            qDebug() << "Bad indexes count at " << indexFile.fileName() << " Key = " << hex << (int) theKey << " Offset = " << hex << (int) oldDataOffset << FN;
    }
    else
        qDebug() << "Indexes chunk not found " << indexFile.fileName() << " Key = " << hex << (int) theKey << " Offset = " << hex << (int) oldDataOffset << FN;

}


template <typename KeyType> void EgIndexes<KeyType>::RemoveChunkFromChain()
{
    quint64 prevChunkPtr, nextChunkPtr;

        // get pointers
    indexStream.device()->seek(indexesChunkOffset + egChunkVolume * oneIndexSize);
    indexStream >> prevChunkPtr;
    indexStream >> nextChunkPtr;

    //qDebug() << "indexesChunkOffset = " << hex << (int) indexesChunkOffset <<
    //            ", nextChunkPtr = " << hex << (int) nextChunkPtr <<
    //            ", prevChunkPtr = " << hex << (int) prevChunkPtr << FN;

        // update backlinks
    if (prevChunkPtr)
    {        
        indexStream.device()->seek(prevChunkPtr + egChunkVolume * oneIndexSize + sizeof(quint64));
        indexStream << nextChunkPtr;
    }
    else // update header
    {
        indexStream.device()->seek(0);
        indexStream << nextChunkPtr;
    }

    if (nextChunkPtr)
    {
        indexStream.device()->seek(nextChunkPtr + egChunkVolume * oneIndexSize);
        indexStream << prevChunkPtr;
    }

    StoreIndexChunk(zero_chunk);

    // TODO add to vacant chunks chain

}

template class EgIndexes<qint32>; // for unit tests
