/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "../egDataNodesType.h"
#include "egIndexes.h"
#include "egFingers.h"

using namespace egIndexesNamespace;

template <typename KeyType> void EgIndexes<KeyType>::PrintIndexesChunk(char* theChunk, const QString &theMessage)
{
    qDebug() << QByteArray(theChunk, indexChunkSize).toHex() << theMessage;
}

template <typename KeyType> inline void EgIndexes<KeyType>::InitIndexChunk()
{
    memset(indexBA.data(), 0, indexChunkSize);

        // write initial values
    localStream->device()-> seek(0);
    *localStream << theKey;
    *localStream << oldDataOffset;  // data node offset

        // update count
    localStream->device()-> seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    *localStream << (keysCountType) 1;
}

template <typename KeyType> void EgIndexes<KeyType>::StoreRootHeader()
{
    indexStream.device()->seek(0);

    indexStream << (quint64) indexHeaderSize;   // first chunk offset
    indexStream << (quint64) 0xABCDEF; // reserved for chunks recycling

}

template <typename KeyType> int EgIndexes<KeyType>::StoreFingerOffset(quint64 chunkOffset, quint64 fingerOffset)
{
    indexStream.device()->seek(chunkOffset + egChunkVolume * oneIndexSize + sizeof(quint64) * 2 + sizeof(keysCountType));
    indexStream << fingerOffset;

    return 0; // FIXME add paranoid mode
}


template <typename KeyType> int EgIndexes<KeyType>::GetFingerOffset(quint64& fingerOffset)
{
    localStream->device()-> seek(egChunkVolume * oneIndexSize + sizeof(quint64) * 2 + sizeof(keysCountType));
    *localStream >> fingerOffset;

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::GetChainPointers(quint64& fwdPtr, quint64& backPtr)
{
    localStream->device()-> seek(egChunkVolume * oneIndexSize);
    *localStream >> fwdPtr;
    *localStream >> backPtr;

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::StoreIndexChunk(const char* chunkPtr, const quint64 chunkOffset)
{
    // PrintIndexesChunk(indexBA.data(), "chunk to save " + FNS);

    indexStream.device()-> seek(chunkOffset); // FIXME check result in paranoid mode

    // qDebug() << "seek result = " << seekRes << "fingersTree-> currentFinger.nextChunkOffset = " << hex << (int) fingersTree-> currentFinger.nextChunkOffset << FN;

    return indexStream.device()-> write(chunkPtr, indexChunkSize);
}


template <typename KeyType> inline int EgIndexes<KeyType>::StoreIndexChunk()
{
    indexStream.device()-> seek(fingersTree-> currentFinger.nextChunkOffset); // FIXME check result in paranoid mode

    return indexStream.device()-> write(indexBA.constData(), indexChunkSize);
}

template <typename KeyType> int EgIndexes<KeyType>::LoadIndexChunk()
{
    indexStream.device()->seek(fingersTree-> currentFinger.nextChunkOffset);

    // if (! indexStream.device()->seek(fingersTree-> currentFinger.nextChunkOffset)) // FIXME implement paranoid mode
    //    qDebug() << "ERROR: bad position of fingersTree-> currentFinger.nextChunkOffset = " << hex << fingersTree-> currentFinger.nextChunkOffset << FN;

    return indexStream.readRawData(indexBA.data(), indexChunkSize);
}

template <typename KeyType> inline int EgIndexes<KeyType>::LoadIndexChunk(char* chunkPtr, quint64 chunkOffset)
{
    indexStream.device()->seek(chunkOffset);

    // if (! indexStream.device()->seek(fingersTree-> currentFinger.nextChunkOffset)) // FIXME implement paranoid mode
    //    qDebug() << "ERROR: bad position of fingersTree-> currentFinger.nextChunkOffset = " << hex << fingersTree-> currentFinger.nextChunkOffset << FN;

    return indexStream.readRawData(chunkPtr, indexChunkSize);
}

template <typename KeyType> inline void EgIndexes<KeyType>::MoveTailToInsert(char* chunkPtr, int indexPosition)
{
    // qDebug() << "chunk before memmove" << QByteArray(chunk, egChunkVolume*oneIndexSize).toHex() << FN;
    memmove (chunkPtr + (indexPosition+1)*oneIndexSize, chunkPtr + indexPosition*oneIndexSize,
             oneIndexSize*(fingersTree-> currentFinger.itemsCount - indexPosition));
    // PrintIndexesChunk(chunk, "chunk after memmove " + FNS);
}

template <typename KeyType> inline void EgIndexes<KeyType>::ReadIndexValues(QDataStream &localIndexesStream, EgIndexStruct<KeyType>& indexStruct)
{
    localIndexesStream.device()->seek(indexPosition*oneIndexSize);
    localIndexesStream >> indexStruct.indexKey;
    localIndexesStream >> indexStruct.dataOffset;
}

template <typename KeyType> inline void EgIndexes<KeyType>::ReadIndexOnly(QDataStream &localIndexesStream, KeyType& currentIndex)
{
    localIndexesStream.device()->seek(indexPosition*oneIndexSize);
    localIndexesStream >> currentIndex;
}

template <typename KeyType> inline void EgIndexes<KeyType>::WriteIndexValues(QDataStream &localIndexesStream, EgIndexStruct<KeyType>& indexStruct, int position)
{
    localIndexesStream.device()->seek(position*oneIndexSize);
    localIndexesStream << indexStruct.indexKey;
    localIndexesStream << indexStruct.dataOffset;
}


template <typename KeyType> inline void EgIndexes<KeyType>::UpdateChunkCount(QDataStream &localIndexesStream, keysCountType newCount)
{
    localIndexesStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexesStream << (keysCountType) newCount;
}


template <typename KeyType> inline int EgIndexes<KeyType>::InsertInsideIndexChunk(QDataStream &localIndexesStream)
{
    FindIndexPositionToInsert(localIndexesStream); // sets indexPosition

    if (indexPosition < fingersTree-> currentFinger.itemsCount) // not last
        MoveTailToInsert(indexBA.data(), indexPosition);

            // write index key and offset
    indexData.indexKey = theKey;
    indexData.dataOffset = oldDataOffset;

    WriteIndexValues(localIndexesStream, indexData, indexPosition);

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::InsertToIndexChunk()
{
    LoadIndexChunk(); // fingersTree-> currentFinger.nextChunkOffset to indexBA.data()

    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger);

    if (fingersTree-> currentFinger.itemsCount < egChunkVolume) // chunk has empty space
    {
        InsertInsideIndexChunk(*localStream);
        UpdateChunkCount(*localStream, fingersTree-> currentFinger.itemsCount + 1);

        StoreIndexChunk(); // indexBA.data() to fingersTree-> currentFinger.nextChunkOffset

        // PrintIndexesChunk(indexBA.data(), "chunk after insert " + FNS);

            // update fingers
        fingersTree-> UpdateFingersChainAfterInsert();
    }
    else    // overflow - append or split chunk
    {
        if (theKey >= fingersTree-> currentFinger.maxKey)
            AppendIndexChunk(*localStream);
        else
            SplitIndexChunk(*localStream);
    }

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::SplitIndexChunk(QDataStream& localIndexStream)
{
    char new_chunk[indexChunkSize];
    memset(&new_chunk, 0, indexChunkSize);

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize);
    localIndexStream >> prevOffsetPtr; // store for chain update
    localIndexStream >> nextOffsetPtr;

    fingersTree-> newFinger.nextChunkOffset = indexStream.device()->size(); // append to the end of indexes file
    fingersTree-> newFinger.myLevel = 0;

    // indexPosition = FindIndexPosition(localIndexStream);
    // indexPosition = FindPosByKeyLast(localIndexStream, EgIndexes<KeyType>::CompareLE) + 1;

    FindIndexPositionToInsert(localIndexStream); // sets indexPosition

    // qDebug() << "indexPosition = " << indexPosition << FN;

    if (indexPosition < egChunkVolume ) // not last
        memmove (indexBA.data() + (indexPosition+1)*oneIndexSize, indexBA.data() + indexPosition*oneIndexSize,
                 oneIndexSize*(egChunkVolume - indexPosition + 1)); // MoveTail

            // write index key and offset
    indexData.indexKey = theKey;
    indexData.dataOffset = oldDataOffset;

    WriteIndexValues(localIndexStream, indexData, indexPosition);

        // move second part to new chunk
    memmove(&new_chunk, indexBA.data() + (egChunkVolume/2+1)*oneIndexSize, egChunkVolume/2*oneIndexSize);

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize);
    localIndexStream << prevOffsetPtr;                                  // restore prev ptr
    localIndexStream << fingersTree-> newFinger.nextChunkOffset;        // new next ptr

    fingersTree-> currentFinger.itemsCount = egChunkVolume/2+1;

    localIndexStream << fingersTree-> currentFinger.itemsCount;

    localIndexStream.device()->seek(0);
    localIndexStream >> fingersTree-> currentFinger.minKey;

    localIndexStream.device()->seek(egChunkVolume/2 * oneIndexSize); // FIXME check pos
    localIndexStream >> fingersTree-> currentFinger.maxKey;

    StoreIndexChunk(); // old one

    memcpy(indexBA.data(), &new_chunk, indexChunkSize);

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize);
    localIndexStream << fingersTree-> currentFinger.nextChunkOffset;    // prev ptr
    localIndexStream << nextOffsetPtr;                                  // old next ptr

    if (nextOffsetPtr) // update next chunk in chain
    {
        indexStream.device()->seek(nextOffsetPtr + egChunkVolume * oneIndexSize);
        indexStream << fingersTree-> newFinger.nextChunkOffset;
    }

    fingersTree-> newFinger.itemsCount = egChunkVolume/2;

    localIndexStream << fingersTree-> newFinger.itemsCount;

    localIndexStream.device()->seek(0);
    localIndexStream >> fingersTree-> newFinger.minKey;

    localIndexStream.device()->seek((egChunkVolume/2 - 1) * oneIndexSize); // FIXME check pos
    localIndexStream >> fingersTree-> newFinger.maxKey;

    StoreIndexChunk(indexBA.data(), fingersTree-> newFinger.nextChunkOffset);

    // PrintIndexesChunk(indexBA.data(), "old chunk " + FNS);
    // PrintIndexesChunk(new_chunk, "new_chunk " + FNS);

        // update fingers tree
    fingersTree-> UpdateFingersChainAfterSplit(); // false);

    return 0;
}


template <typename KeyType> int EgIndexes<KeyType>::AppendIndexChunk(QDataStream& localIndexStream)
{

        // set new finger
    fingersTree-> newFinger.nextChunkOffset = indexStream.device()->size(); // append to the end of indexes file

    fingersTree-> newFinger.myLevel    = 0;
    fingersTree-> newFinger.itemsCount = 1;

    fingersTree-> newFinger.minKey     = theKey;
    fingersTree-> newFinger.maxKey     = theKey;

        // get old next ptr
    localIndexStream.device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    localIndexStream >> nextOffsetPtr;

    // qDebug() << "current next ptr =  " << hex << (int) fingersTree-> newFinger.nextChunkOffset << " ,prev ptr =  " << hex << (int) prevOffsetPtr << FN;

        // write new next ptr
    localIndexStream.device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    localIndexStream << fingersTree-> newFinger.nextChunkOffset;

        // save old chunk
    StoreIndexChunk();

        // fill new chunk data
    memset(indexBA.data(), 0, indexChunkSize);
    // memcpy(indexBA.data(), zero_chunk, indexChunkSize);

        // write index key and offset
    indexData.indexKey   = theKey;
    indexData.dataOffset = oldDataOffset;

    WriteIndexValues(localIndexStream, indexData, 0);

        // update pointers & count
    localIndexStream.device()->seek(egChunkVolume * oneIndexSize);
    localIndexStream << fingersTree-> currentFinger.nextChunkOffset;        // prev ptr
    localIndexStream << nextOffsetPtr;                                      // stored next ptr

    UpdateChunkCount(localIndexStream, 1);

    StoreIndexChunk(indexBA.data(), fingersTree-> newFinger.nextChunkOffset);

    if (nextOffsetPtr) // update next chunk in chain
    {
        indexStream.device()->seek(nextOffsetPtr + egChunkVolume * oneIndexSize);
        indexStream << fingersTree-> newFinger.nextChunkOffset;
    }

        // update fingers tree
    fingersTree-> UpdateFingersChainAfterSplit(); // true);

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::FindIndexPositionToInsert(QDataStream& localIndexesStream)
{
    KeyType currentIndex;

    indexPosition = fingersTree-> currentFinger.itemsCount - 1; // last one

        // find position to insert
    while (indexPosition >= 0)
    {
           // load index key from chunk
       ReadIndexOnly(localIndexesStream, currentIndex);

       if(currentIndex <= theKey)
       {
           indexPosition++; // insert here
           break;
       }

       indexPosition--;
    }

    if (indexPosition < 0)
        indexPosition = 0;

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::FindPosByKeyFirst(QDataStream& localIndexesStream, CompareFunctionType myCompareFunc)
{
    KeyType currentIndex;

        // min/max lookup
    if ((theKey >  fingersTree-> currentFinger.maxKey)|| (fingersTree-> currentFinger.itemsCount <= 0))
    {
        qDebug() << "ERROR bad finger of " << fingersTree-> IndexFileName << " for Key = " << theKey << FN;

        fingersTree-> PrintFingerInfo(fingersTree-> currentFinger, "currentFinger");
        return -1; // error
    }

    if ((theKey < fingersTree-> currentFinger.minKey) || (fingersTree-> currentFinger.itemsCount == 1))
    {
            // load key
        indexPosition = 0;
        ReadIndexOnly(localIndexesStream, currentIndex);

        if (myCompareFunc(currentIndex,theKey))
            return 0; // got it
        else
            return -1;
    }

        // proportional index lookup
    if (fingersTree-> currentFinger.maxKey > fingersTree-> currentFinger.minKey)
        indexPosition = (theKey - fingersTree-> currentFinger.minKey)*(fingersTree-> currentFinger.itemsCount - 1) /
                        (fingersTree-> currentFinger.maxKey - fingersTree-> currentFinger.minKey);
    else
    {
        if (myCompareFunc(fingersTree-> currentFinger.minKey, theKey))
            return 0; // got it
        else
            return -1;
    }

    // qDebug() << "indexPosition =  " << indexPosition << FN;

    ReadIndexOnly(localIndexesStream, currentIndex);

    // qDebug() << "indexPosition =  " << indexPosition << " ,currentIndex = " << hex << (int) currentIndex << FN;

        // compare
    if (myCompareFunc(currentIndex,theKey) && (indexPosition > 0)) // (currentIndex >= theKey)
    {
            // move down until currentIndex <
        do
        {
            indexPosition--;
            ReadIndexOnly(localIndexesStream, currentIndex);
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
            ReadIndexOnly(localIndexesStream, currentIndex);
        }
        while (! myCompareFunc(currentIndex,theKey) && (indexPosition < (fingersTree-> currentFinger.itemsCount-1))); // (currentIndex < theKey)
    }

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
            // load key
        indexPosition = fingersTree-> currentFinger.itemsCount - 1;
        ReadIndexOnly(localIndexesStream, currentIndex);

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

    ReadIndexOnly(localIndexesStream, currentIndex);

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
            ReadIndexOnly(localIndexesStream, currentIndex);

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
            ReadIndexOnly(localIndexesStream, currentIndex);
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
    indexStream >> fingersTree-> currentFinger.nextChunkOffset;

    LoadIndexChunk(); // fingersTree-> currentFinger.nextChunkOffset to indexBA.data()

    indexPosition = 0;

    LoadDataUp(index_offsets, *localStream);
}

template <typename KeyType> void EgIndexes<KeyType>::LoadDataByChunkUp(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc)
{
    // QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(); // indexBA.data()

    indexPosition = FindPosByKeyFirst(*localStream, myCompareFunc);

    // qDebug() << "pos =  " << indexPosition << FN;

    if (indexPosition >= 0)
        LoadDataUp(index_offsets, *localStream);
}

template <typename KeyType> void EgIndexes<KeyType>::LoadDataUp(QSet<quint64>& index_offsets, QDataStream& localIndexStream)
{
    quint64 dataOffset;

    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream >> chunkCount;

    // qDebug() << "indexPosition =  " << indexPosition  << " , chunkCount =  " << chunkCount << FN;

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
        LoadIndexChunk(indexBA.data(), nextOffsetPtr);

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
    quint64 dataOffset;

    LoadIndexChunk(); // indexBA.data()

    // qDebug() << "indexBA =  " << indexBA.toHex() << FN;

    indexPosition = FindPosByKeyLast(*localStream, myCompareFunc);

    // qDebug() << "pos =  " << indexPosition << FN;

    if (indexPosition < 0)
        return indexPosition; // not found

    while (indexPosition >= 0)
    {
        localStream->device()-> seek(indexPosition*oneIndexSize + sizeof(KeyType));
        // localIndexStream >> currentIndex;
        *localStream >> dataOffset;

        index_offsets.insert(dataOffset);

        indexPosition--;
    }

        // get next chunk
    localStream->device()-> seek(egChunkVolume * oneIndexSize);
    *localStream >> prevOffsetPtr;

    // qDebug() << "prevOffsetPtr =  " << hex << (int) prevOffsetPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

    while (prevOffsetPtr)
    {
        LoadIndexChunk(indexBA.data(), prevOffsetPtr);

            // get chunk items count
        localStream->device()-> seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
        *localStream >> chunkCount;

        indexPosition = chunkCount - 1;

        while (indexPosition >= 0)
        {
            localStream->device()-> seek(indexPosition*oneIndexSize + sizeof(KeyType));
            // localIndexStream >> currentIndex;
            *localStream >> dataOffset;

            index_offsets.insert(dataOffset);

            indexPosition--;
        }

            // get next chunk ptr
        localStream->device()-> seek(egChunkVolume * oneIndexSize);
        *localStream >> prevOffsetPtr;
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

    nextChunkPtr =  fingersTree-> currentFinger.nextChunkOffset;

    // qDebug() << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

    while (nextChunkPtr && (! keyOutOfRange))
    {
        LoadIndexChunk(indexBA.data(), nextChunkPtr);

        if (firstChunk)
        {
           indexPosition = FindPosByKeyFirst(*localStream, CompareGE);

           if (indexPosition < 0)
              indexPosition = 0;    // FIXME

           firstChunk = false;

           chunkCount = fingersTree-> currentFinger.itemsCount;
        }
        else
        {
            indexPosition = 0;

                // get chunk items count
            localStream->device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
            *localStream >> chunkCount;
        }

        // qDebug() << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

        // indexPosition = 0;

        do
        {
            localStream->device()-> seek(indexPosition*oneIndexSize);
            *localStream >> currentIndex;
            *localStream >> dataOffset;

            // qDebug() << "currentIndex =  " << hex << (int) currentIndex  << " , dataOffset =  " << hex << (int) dataOffset << FN;

            if (currentIndex == theKey)
                index_offsets.insert(dataOffset);
            else
                keyOutOfRange = true;

            indexPosition++;
        }
        while ((! keyOutOfRange) && (indexPosition < chunkCount));

            // get next chunk ptr
        localStream->device()-> seek((egChunkVolume * oneIndexSize) + (sizeof(quint64)));
        *localStream >> nextChunkPtr;
    }
}


template <typename KeyType> int EgIndexes<KeyType>::FindIndexByDataOffset(QDataStream &localIndexStream, bool isPrimary)
{
    KeyType currentIndex;
    quint64 dataOffset;

    // quint64 nextChunkPtr;

    bool keyOutOfRange = false;
    bool firstChunk = true;

    // fingersTree-> fingerIsMoved = false; // moved flag setup

    LoadIndexChunk(); // indexBA.data()

    indexesChunkOffset = fingersTree-> currentFinger.nextChunkOffset;

    while (indexesChunkOffset && (! keyOutOfRange))
    {
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

        do
        {
            localIndexStream.device()->seek(indexPosition*oneIndexSize);
            localIndexStream >> currentIndex;
            localIndexStream >> dataOffset;

            // qDebug() << "currentIndex =  " << hex << (int) currentIndex  << " , dataOffset =  " << hex << (int) dataOffset << FN;

            if (currentIndex == theKey)
            {
                if (! isPrimary)
                {
                    if (dataOffset == oldDataOffset)
                        return 0; // found indexPosition
                }
                else    // found ID, store offset
                {
                    oldDataOffset = dataOffset;
                    return 0;
                }
            }
            else
                keyOutOfRange = true;

            indexPosition++;
        }
        while ((! keyOutOfRange) && (indexPosition < chunkCount));

            // get next chunk ptr
        localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64)));
        localIndexStream >> indexesChunkOffset;

        if (indexesChunkOffset)
        {
            // fingersTree-> fingerIsMoved = true;
            LoadIndexChunk(); // to indexBA.data()
        }
    }

    return 1; // not found
}

template <typename KeyType> int EgIndexes<KeyType>::UpdateDataOffset()
{
    indexStream.device()->seek(fingersTree-> currentFinger.nextChunkOffset + indexPosition*oneIndexSize + sizeof(KeyType));
    indexStream << newDataOffset;

    return 0;
}


template <typename KeyType> int EgIndexes<KeyType>::DeleteDataOffset(QDataStream &localIndexStream)
{

        // get count
    localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    localIndexStream >> chunkCount;

    // qDebug() << "indexPosition =  " << hex << (int) indexPosition << " ,chunkCount =  " << hex << (int) chunkCount << FN;

        // if not last, move tail
    if (indexPosition < (chunkCount - 1)) // FIXME move tail
    {
        // qDebug() << "indexPosition = " << indexPosition << ", itemsCount = " << chunkCount << FN;
        // qDebug() << "chunk before memmove" << indexBA.toHex() << FN;
        memmove (indexBA.data() + indexPosition*oneIndexSize, indexBA.data() + (indexPosition+1)*oneIndexSize,  oneIndexSize*(chunkCount - indexPosition - 1));
    }

        // write zeroes
    memset(indexBA.data() + (chunkCount - 1)*oneIndexSize, 0, oneIndexSize);

    --chunkCount; // DECREASE

    fingersTree-> minValueChanged = false;
    fingersTree-> maxValueChanged = false;

        // get new min/max FIXME remove it
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
    StoreIndexChunk();

    return 0;
}

template <typename KeyType> void EgIndexes<KeyType>::UpdateIndex(bool isPrimary)
{
    if (FindIndexByDataOffset(*localStream, isPrimary) == 0) // index found
        UpdateDataOffset();
    else
        qDebug() << "Index to update not found, theKey = " << theKey << ", IndexFileName = " << fingersTree-> IndexFileName << FN;
}

template <typename KeyType> int EgIndexes<KeyType>::DeleteIndex(bool isPrimary)
{
    // qDebug() << "theKey = " << theKey << ", IndexFileName = " << fingersTree-> IndexFileName << FN;

    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger, "DeleteIndex");

    if (FindIndexByDataOffset(*localStream, isPrimary) == 0) // index found
    {
        // qDebug() << "chunkCount 1 =  " << hex << (int) chunkCount << FN;

            // get finger ptr
        localStream->device()-> seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2) + sizeof(keysCountType));
        *localStream >> fingersTree-> currentFingerOffset;

        // qDebug() << "chunkCount =  " << (int) chunkCount << "currentFingerOffset =  " << hex << (int) fingersTree-> currentFingerOffset << FN;

        if (chunkCount > 1)
        {
            DeleteDataOffset(*localStream); // count decrease

            // qDebug() << "chunkCount 2 =  " << hex << (int) chunkCount << FN;
            // qDebug() << "fingersTree-> currentFingerOffset " << hex << (int) fingersTree-> currentFingerOffset << FN;

            // fingersTree-> currentKeysCount = chunkCount;
            fingersTree-> UpdateFingerCountAfterDelete(chunkCount); // by fingersTree-> currentFingerOffset

            if (fingersTree-> minValueChanged)
                fingersTree-> UpdateMinValueUp();
            else if (fingersTree-> maxValueChanged)
                fingersTree-> UpdateMaxValueUp();
        }
        else if (chunkCount == 1)
        {
            if (fingersTree-> DeleteParentFinger() == 1) // last index
                return 1; // delete files

            RemoveChunkFromChain();
        }
        else
            qDebug() << "Bad indexes count at " << fingersTree-> IndexFileName << " Key = " << hex << (int) theKey << " Offset = " << hex << (int) oldDataOffset << FN;
    }
    else
        qDebug() << "Indexes chunk not found " << fingersTree-> IndexFileName << " Key = " << hex << (int) theKey << " Offset = " << hex << (int) oldDataOffset << FN;

    return 0;
}


template <typename KeyType> void EgIndexes<KeyType>::RemoveChunkFromChain()
{
    // quint64 prevChunkPtr, nextChunkPtr;

        // get pointers
    indexStream.device()->seek(indexesChunkOffset + egChunkVolume * oneIndexSize);
    indexStream >> prevOffsetPtr;
    indexStream >> nextOffsetPtr;
/*
    qDebug() << "indexesChunkOffset = " << hex << (int) indexesChunkOffset <<
                ", nextOffsetPtr = " << hex << (int) nextOffsetPtr <<
                ", prevOffsetPtr = " << hex << (int) prevOffsetPtr << FN;
*/
        // update backlinks
    if (prevOffsetPtr)
    {        
        indexStream.device()->seek(prevOffsetPtr + egChunkVolume * oneIndexSize  + sizeof(quint64));
        indexStream << nextOffsetPtr;
    }
    else // update header
    {
        indexStream.device()->seek(0);
        indexStream << nextOffsetPtr;
    }

    if (nextOffsetPtr)
    {
        indexStream.device()->seek(nextOffsetPtr + egChunkVolume * oneIndexSize );
        indexStream << prevOffsetPtr;
    }

    // StoreIndexChunk(zero_chunk, indexesChunkOffset);

    // TODO add to vacant chunks chain

}

/*
EgIndexes<qint32> EgIndexesqint32;
EgIndexes<quint32> EgIndexesquint32;
EgIndexes<qint64> EgIndexesqint64;
EgIndexes<quint64> EgIndexesquint64;
EgIndexes<float> EgIndexesfloat;
EgIndexes<double> EgIndexesdouble;
*/
