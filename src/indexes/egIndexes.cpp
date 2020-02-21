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
    EG_LOG_STUB << QByteArray(theChunk, indexChunkSize).toHex() << theMessage;
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

    // EG_LOG_STUB << "seek result = " << seekRes << "fingersTree-> currentFinger.nextChunkOffset = " << hex << (int) fingersTree-> currentFinger.nextChunkOffset << FN;

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
    //    EG_LOG_STUB << "ERROR: bad position of fingersTree-> currentFinger.nextChunkOffset = " << hex << fingersTree-> currentFinger.nextChunkOffset << FN;

    return indexStream.readRawData(indexBA.data(), indexChunkSize);
}

template <typename KeyType> inline int EgIndexes<KeyType>::LoadIndexChunk(char* chunkPtr, quint64 chunkOffset)
{
    indexStream.device()->seek(chunkOffset);

    // if (! indexStream.device()->seek(fingersTree-> currentFinger.nextChunkOffset)) // FIXME implement paranoid mode
    //    EG_LOG_STUB << "ERROR: bad position of fingersTree-> currentFinger.nextChunkOffset = " << hex << fingersTree-> currentFinger.nextChunkOffset << FN;

    return indexStream.readRawData(chunkPtr, indexChunkSize);
}

template <typename KeyType> inline void EgIndexes<KeyType>::MoveTailToInsert(char* chunkPtr, int indexPosition)
{
    // EG_LOG_STUB << "chunk before memmove" << QByteArray(chunk, egChunkVolume*oneIndexSize).toHex() << FN;
    memmove (chunkPtr + (indexPosition+1)*oneIndexSize, chunkPtr + indexPosition*oneIndexSize,
             oneIndexSize*(fingersTree-> currentFinger.itemsCount - indexPosition));
    // PrintIndexesChunk(chunk, "chunk after memmove " + FNS);
}

template <typename KeyType> inline void EgIndexes<KeyType>::ReadIndexValues(EgIndexStruct<KeyType>& indexStruct)
{
    localStream->device()->seek(indexPosition*oneIndexSize);
    *localStream >> indexStruct.indexKey;
    *localStream >> indexStruct.dataOffset;
}

template <typename KeyType> inline void EgIndexes<KeyType>::ReadIndexOnly(KeyType& currentIndex)
{
    localStream->device()->seek(indexPosition*oneIndexSize);
    *localStream >> currentIndex;
}

template <typename KeyType> inline void EgIndexes<KeyType>::WriteIndexValues(EgIndexStruct<KeyType>& indexStruct, int position)
{
    localStream->device()->seek(position*oneIndexSize);
    *localStream << indexStruct.indexKey;
    *localStream << indexStruct.dataOffset;
}


template <typename KeyType> inline void EgIndexes<KeyType>::UpdateChunkCount(keysCountType newCount)
{
    localStream->device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    *localStream << (keysCountType) newCount;
}


template <typename KeyType> inline int EgIndexes<KeyType>::InsertInsideIndexChunk()
{
    FindIndexPositionToInsert(); // sets indexPosition

    if (indexPosition < fingersTree-> currentFinger.itemsCount) // not last
        MoveTailToInsert(indexBA.data(), indexPosition);

            // write index key and offset
    indexData.indexKey = theKey;
    indexData.dataOffset = oldDataOffset;

    WriteIndexValues(indexData, indexPosition);

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::InsertToIndexChunk()
{
    LoadIndexChunk(); // fingersTree-> currentFinger.nextChunkOffset to indexBA.data()

    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger);

    if (fingersTree-> currentFinger.itemsCount < egChunkVolume) // chunk has empty space
    {
        InsertInsideIndexChunk();
        UpdateChunkCount(fingersTree-> currentFinger.itemsCount + 1);

        StoreIndexChunk(); // indexBA.data() to fingersTree-> currentFinger.nextChunkOffset

        // PrintIndexesChunk(indexBA.data(), "chunk after insert " + FNS);

            // update fingers
        fingersTree-> UpdateFingersChainAfterInsert();
    }
    else    // overflow - append or split chunk
    {
        if (theKey >= fingersTree-> currentFinger.maxKey)
            AppendIndexChunk();
        else
            SplitIndexChunk();

            // update fingers tree
        fingersTree-> UpdateFingersChainAfterSplit();
    }

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::SplitIndexChunk()
{
    char new_chunk[indexChunkSize];
    memset(&new_chunk, 0, indexChunkSize);

    localStream->device()-> seek(egChunkVolume * oneIndexSize);
    *localStream >> prevOffsetPtr; // store for chain update
    *localStream >> nextOffsetPtr;

    fingersTree-> newFinger.nextChunkOffset = indexStream.device()->size(); // append to the end of indexes file
    fingersTree-> newFinger.myLevel = 0;

    // indexPosition = FindIndexPosition(*localStream);
    // indexPosition = FindPosByKeyLast(EgIndexes<KeyType>::CompareLE) + 1;

    FindIndexPositionToInsert(); // sets indexPosition

    // EG_LOG_STUB << "indexPosition = " << indexPosition << FN;

    // PrintIndexesChunk(indexBA.data(), "before move" + FNS);

    if (indexPosition < fingersTree-> currentFinger.itemsCount ) // not last
        // memmove (indexBA.data() + (indexPosition+1)*oneIndexSize, indexBA.data() + indexPosition*oneIndexSize,
        //         oneIndexSize*(egChunkVolume - indexPosition + 1)); // MoveTail

        MoveTailToInsert(indexBA.data(), indexPosition);

            // write index key and offset
    indexData.indexKey = theKey;
    indexData.dataOffset = oldDataOffset;

    WriteIndexValues(indexData, indexPosition);

    // PrintIndexesChunk(indexBA.data(), "after insert" + FNS);

        // move second part to new chunk
    memmove(&new_chunk, indexBA.data() + (egChunkVolume/2+1)*oneIndexSize, egChunkVolume/2*oneIndexSize);
    memset(indexBA.data() + (egChunkVolume/2+1)*oneIndexSize, 0, (egChunkVolume/2-1)*oneIndexSize);

    localStream->device()->seek(egChunkVolume * oneIndexSize);
    *localStream << prevOffsetPtr;                                  // restore prev ptr
    *localStream << fingersTree-> newFinger.nextChunkOffset;        // new next ptr

    fingersTree-> currentFinger.itemsCount = egChunkVolume/2+1;

    *localStream << fingersTree-> currentFinger.itemsCount;

    localStream->device()->seek(0);
    *localStream >> fingersTree-> currentFinger.minKey;

    localStream->device()->seek(egChunkVolume/2 * oneIndexSize); // FIXME check pos
    *localStream >> fingersTree-> currentFinger.maxKey;

    StoreIndexChunk(); // old one

    // PrintIndexesChunk(indexBA.data(), "old chunk updated" + FNS);

    memcpy(indexBA.data(), &new_chunk, indexChunkSize);

    localStream->device()->seek(egChunkVolume * oneIndexSize);
    *localStream << fingersTree-> currentFinger.nextChunkOffset;    // prev ptr
    *localStream << nextOffsetPtr;                                  // old next ptr

    if (nextOffsetPtr) // update next chunk in chain
    {
        indexStream.device()->seek(nextOffsetPtr + egChunkVolume * oneIndexSize);
        indexStream << fingersTree-> newFinger.nextChunkOffset;
    }

    fingersTree-> newFinger.itemsCount = egChunkVolume/2;

    *localStream << fingersTree-> newFinger.itemsCount;

    localStream->device()-> seek(0);
    *localStream >> fingersTree-> newFinger.minKey;

    localStream->device()-> seek((egChunkVolume/2 - 1) * oneIndexSize); // FIXME check pos
    *localStream >> fingersTree-> newFinger.maxKey;

    StoreIndexChunk(indexBA.data(), fingersTree-> newFinger.nextChunkOffset);

    // PrintIndexesChunk(indexBA.data(), "new_chunk " + FNS);

    return 0;
}


template <typename KeyType> int EgIndexes<KeyType>::AppendIndexChunk()
{
        // setup new finger
    fingersTree-> newFinger.nextChunkOffset = indexStream.device()->size(); // append to the end of indexes file

    fingersTree-> newFinger.myLevel    = 0;
    fingersTree-> newFinger.itemsCount = 1;

    fingersTree-> newFinger.minKey     = theKey;
    fingersTree-> newFinger.maxKey     = theKey;

        // get old next ptr
    localStream->device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    *localStream >> nextOffsetPtr;

    // EG_LOG_STUB << "current next ptr =  " << hex << (int) fingersTree-> newFinger.nextChunkOffset << " ,prev ptr =  " << hex << (int) prevOffsetPtr << FN;

        // write new next ptr
    localStream->device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64));
    *localStream << fingersTree-> newFinger.nextChunkOffset;

    StoreIndexChunk(); // save old chunk by currentFinger

    memset(indexBA.data(), 0, indexChunkSize); // empty new chunk

        // write index key and offset
    indexData.indexKey   = theKey;
    indexData.dataOffset = oldDataOffset;

    WriteIndexValues(indexData, 0);

        // update chain pointers & count
    localStream->device()->seek(egChunkVolume * oneIndexSize);
    *localStream << fingersTree-> currentFinger.nextChunkOffset;        // prev ptr
    *localStream << nextOffsetPtr;                                      // stored next ptr

    UpdateChunkCount(1);

    StoreIndexChunk(indexBA.data(), fingersTree-> newFinger.nextChunkOffset); // new chunk

    if (nextOffsetPtr) // update next chunk in chain
    {
        indexStream.device()->seek(nextOffsetPtr + egChunkVolume * oneIndexSize);
        indexStream << fingersTree-> newFinger.nextChunkOffset;
    }

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::FindIndexPositionToInsert()
{
    KeyType currentIndex;

    indexPosition = fingersTree-> currentFinger.itemsCount - 1; // last one

        // find position to insert
    while (indexPosition >= 0)
    {
           // load index key from chunk
       ReadIndexOnly(currentIndex);

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

template <typename KeyType> int EgIndexes<KeyType>::FindPosByKeyFirst(CompareFunctionType myCompareFunc)
{
    KeyType currentIndex;

        // min/max lookup
    if ((theKey >  fingersTree-> currentFinger.maxKey)|| (fingersTree-> currentFinger.itemsCount <= 0))
    {
        EG_LOG_STUB << "ERROR bad finger of " << fingersTree-> IndexFileName << " for Key = " << theKey << FN;

        fingersTree-> PrintFingerInfo(fingersTree-> currentFinger, "currentFinger");
        return -1; // error
    }

    if ((theKey < fingersTree-> currentFinger.minKey) || (fingersTree-> currentFinger.itemsCount == 1))
    {
            // load key
        indexPosition = 0;
        ReadIndexOnly(currentIndex);

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

    // EG_LOG_STUB << "indexPosition =  " << indexPosition << FN;

    ReadIndexOnly(currentIndex);

    // EG_LOG_STUB << "indexPosition =  " << indexPosition << " ,currentIndex = " << hex << (int) currentIndex << FN;

        // compare
    if (myCompareFunc(currentIndex,theKey) && (indexPosition > 0)) // (currentIndex >= theKey)
    {
            // move down until currentIndex <
        do
        {
            indexPosition--;
            ReadIndexOnly(currentIndex);
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
            ReadIndexOnly(currentIndex);
        }
        while (! myCompareFunc(currentIndex,theKey) && (indexPosition < (fingersTree-> currentFinger.itemsCount-1))); // (currentIndex < theKey)
    }

    return indexPosition;
}

template <typename KeyType> int EgIndexes<KeyType>::FindPosByKeyLast(CompareFunctionType myCompareFunc)
{
    KeyType currentIndex;

        // min/max check
    if ((theKey < fingersTree-> currentFinger.minKey) || (fingersTree-> currentFinger.itemsCount <= 0))
    {
        // EG_LOG_STUB << "ERROR bad finger for Key = " << theKey << FN;

        // fingersTree->PrintFingerInfo(fingersTree-> currentFinger, "currentFinger");
        return -1; // error or between fingers
    }

    if ((theKey >  fingersTree-> currentFinger.maxKey) || (fingersTree-> currentFinger.itemsCount == 1))
    {
            // load key
        indexPosition = fingersTree-> currentFinger.itemsCount - 1;
        ReadIndexOnly(currentIndex);

        /*
        EG_LOG_STUB << "indexPosition 0 =  " << indexPosition
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

    ReadIndexOnly(currentIndex);

    /*
    EG_LOG_STUB << "indexPosition 1 =  " << indexPosition
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
            ReadIndexOnly(currentIndex);

            // EG_LOG_STUB << "indexPosition 2 =  " << indexPosition << " ,currentIndex = " << hex << (int) currentIndex << FN;
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
            ReadIndexOnly(currentIndex);
        }
        while (! myCompareFunc(currentIndex,theKey)  && (indexPosition > 0)); // (currentIndex > theKey)
    }

    /*
    EG_LOG_STUB << "indexPosition 3 =  " << indexPosition
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

    LoadDataUp(index_offsets);
}

template <typename KeyType> int EgIndexes<KeyType>::LoadDataByChunkUp(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc)
{
    LoadIndexChunk(); // indexBA.data()

    indexPosition = FindPosByKeyFirst(myCompareFunc);

    // EG_LOG_STUB << "pos =  " << indexPosition << FN;

    if (indexPosition >= 0)
        LoadDataUp(index_offsets);

    return 0;
}

template <typename KeyType> void EgIndexes<KeyType>::LoadDataUp(QSet<quint64>& index_offsets)
{
    quint64 dataOffset;

    localStream->device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    *localStream >> chunkCount;

    // EG_LOG_STUB << "indexPosition =  " << indexPosition  << " , chunkCount =  " << chunkCount << FN;

    while (indexPosition < chunkCount)
    {
        localStream->device()->seek(indexPosition*oneIndexSize + sizeof(KeyType));
        // *localStream >> currentIndex;
        *localStream >> dataOffset;

        index_offsets.insert(dataOffset);

        indexPosition++;
    }

        // get next chunk
    localStream->device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64)));
    *localStream >> nextOffsetPtr;

    while (nextOffsetPtr)
    {
        LoadIndexChunk(indexBA.data(), nextOffsetPtr);

            // get chunk items count
        localStream->device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
        *localStream >> chunkCount;

        // EG_LOG_STUB << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

        indexPosition = 0;

        while (indexPosition < chunkCount)
        {
            localStream->device()->seek(indexPosition*oneIndexSize + sizeof(KeyType));
            // *localStream >> currentIndex;
            *localStream >> dataOffset;

            index_offsets.insert(dataOffset);

            indexPosition++;
        }

            // get next chunk ptr
        localStream->device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64)));
        *localStream >> nextOffsetPtr;
    }
}


template <typename KeyType> int EgIndexes<KeyType>::LoadDataByChunkDown(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc)
{
    quint64 dataOffset;

    LoadIndexChunk(); // indexBA.data()

    // EG_LOG_STUB << "indexBA =  " << indexBA.toHex() << FN;

    indexPosition = FindPosByKeyLast(myCompareFunc);

    // EG_LOG_STUB << "pos =  " << indexPosition << FN;

    if (indexPosition < 0)
        return indexPosition; // not found

    while (indexPosition >= 0)
    {
        localStream->device()-> seek(indexPosition*oneIndexSize + sizeof(KeyType));
        // *localStream >> currentIndex;
        *localStream >> dataOffset;

        index_offsets.insert(dataOffset);

        indexPosition--;
    }

        // get next chunk
    localStream->device()-> seek(egChunkVolume * oneIndexSize);
    *localStream >> prevOffsetPtr;

    // EG_LOG_STUB << "prevOffsetPtr =  " << hex << (int) prevOffsetPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

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
            // *localStream >> currentIndex;
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

template <typename KeyType> int EgIndexes<KeyType>::LoadDataByChunkEqual(QSet<quint64>& index_offsets)
{
    KeyType currentIndex;
    quint64 dataOffset;

    quint64 nextChunkPtr;

    bool keyOutOfRange = false;
    bool firstChunk = true;

    nextChunkPtr =  fingersTree-> currentFinger.nextChunkOffset;

    // EG_LOG_STUB << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

    while (nextChunkPtr && (! keyOutOfRange))
    {
        LoadIndexChunk(indexBA.data(), nextChunkPtr);

        if (firstChunk)
        {
           indexPosition = FindPosByKeyFirst(CompareGE);  // Index >= Key

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

        // EG_LOG_STUB << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

        // indexPosition = 0;

        do
        {
            localStream->device()-> seek(indexPosition*oneIndexSize);
            *localStream >> currentIndex;
            *localStream >> dataOffset;

            // EG_LOG_STUB << "currentIndex =  " << hex << (int) currentIndex  << " , dataOffset =  " << hex << (int) dataOffset << FN;

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

    return 0;
}


template <typename KeyType> int EgIndexes<KeyType>::FindIndexByDataOffset(bool isPrimary)
{
    KeyType currentIndex;
    quint64 dataOffset;

    // quint64 nextChunkPtr;

    bool keyOutOfRange = false;
    bool firstChunk = true;

    // fingersTree-> fingerIsMoved = false; // moved flag setup

    LoadIndexChunk(); // indexBA.data() by finger

    indexesChunkOffset = fingersTree-> currentFinger.nextChunkOffset;

    while (indexesChunkOffset && (! keyOutOfRange))
    {
        if (firstChunk)
        {
           indexPosition = FindPosByKeyFirst(CompareGE);
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

        // EG_LOG_STUB << "indexPosition =  " << indexPosition;

        do
        {
            localStream->device()->seek(indexPosition*oneIndexSize);
            *localStream >> currentIndex;
            *localStream >> dataOffset;

            // EG_LOG_STUB << "currentIndex =  " << hex << (int) currentIndex  << " , dataOffset =  " << hex << (int) dataOffset << FN;

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
        localStream->device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64)));
        *localStream >> indexesChunkOffset;

        if (indexesChunkOffset)
        {
            // fingersTree-> fingerIsMoved = true;
            LoadIndexChunk(indexBA.data(), indexesChunkOffset);
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


template <typename KeyType> int EgIndexes<KeyType>::DeleteDataOffset()
{

        // get count
    localStream->device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    *localStream >> chunkCount;

    // EG_LOG_STUB << "indexPosition =  " << hex << (int) indexPosition << " ,chunkCount =  " << hex << (int) chunkCount << FN;

        // if not last, move tail
    if (indexPosition < (chunkCount - 1)) // FIXME move tail
    {
        // EG_LOG_STUB << "indexPosition = " << indexPosition << ", itemsCount = " << chunkCount << FN;
        // EG_LOG_STUB << "chunk before memmove" << indexBA.toHex() << FN;
        memmove (indexBA.data() + indexPosition*oneIndexSize, indexBA.data() + (indexPosition+1)*oneIndexSize,  oneIndexSize*(chunkCount - indexPosition - 1));
    }

        // write zeroes
    memset(indexBA.data() + (chunkCount - 1)*oneIndexSize, 0, oneIndexSize);

        // reset flags
    fingersTree-> minValueChanged = false;
    fingersTree-> maxValueChanged = false;

        // get new min/max FIXME check
    if (indexPosition == 0)
    {
        localStream->device()->seek(0);
        *localStream >> fingersTree-> newMinValue;

        if (theKey < fingersTree-> newMinValue)
            fingersTree-> minValueChanged = true;
    }
    else if ((indexPosition == chunkCount-1) && (chunkCount > 1))
    {
        localStream->device()->seek((chunkCount-2)*oneIndexSize);
        *localStream >> fingersTree->newMaxValue;

        if (theKey > fingersTree-> newMaxValue)
            fingersTree-> maxValueChanged = true;
    }

    --chunkCount; // chunkCount DECREMENT here

        // write decreased count
    localStream->device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2));
    *localStream << chunkCount;

    // EG_LOG_STUB << "chunk after memmove " << indexBA.toHex() << FN;

        // save
    StoreIndexChunk();

    return 0;
}

template <typename KeyType> void EgIndexes<KeyType>::UpdateIndex(bool isPrimary)
{
    if (FindIndexByDataOffset(isPrimary) == 0) // index found
        UpdateDataOffset();
    else
        EG_LOG_STUB << "Index to update not found, theKey = " << theKey << ", IndexFileName = " << fingersTree-> IndexFileName << FN;
}

template <typename KeyType> int EgIndexes<KeyType>::DeleteIndex(bool isPrimary)
{
    // EG_LOG_STUB << "theKey = " << theKey << ", oldDataOffset = " << hex << (int) oldDataOffset << ", IndexFileName = " << fingersTree-> IndexFileName << FN;

    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger, "DeleteIndex");

    if (FindIndexByDataOffset(isPrimary) == 0) // index found
    {
        // EG_LOG_STUB << "chunkCount 1 =  " << (int) chunkCount << FN;

            // get finger ptr
        localStream->device()-> seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2) + sizeof(keysCountType));
        *localStream >> fingersTree-> currentFingerOffset;

        // EG_LOG_STUB << "chunkCount =  " << (int) chunkCount << "currentFingerOffset =  " << hex << (int) fingersTree-> currentFingerOffset << FN;

        if (chunkCount > 1) // not last index
        {
            DeleteDataOffset(); // chunkCount decrement here

            // EG_LOG_STUB << "chunkCount 2 =  " << hex << (int) chunkCount << FN;
            // EG_LOG_STUB << "fingersTree-> currentFingerOffset " << hex << (int) fingersTree-> currentFingerOffset << FN;

            // fingersTree-> currentKeysCount = chunkCount;
            fingersTree-> UpdateFingerCountAfterDelete(chunkCount); // by fingersTree-> currentFingerOffset

            if (fingersTree-> minValueChanged)
                fingersTree-> UpdateMinValueUp();
            else if (fingersTree-> maxValueChanged)
                fingersTree-> UpdateMaxValueUp();
        }
        else if (chunkCount == 1) // last index, delete finger by fingersTree-> currentFingerOffset
        {
            if (fingersTree-> DeleteParentFinger() == 1) // last index
                return 1; // delete files

            RemoveChunkFromChain();
        }
        else
            EG_LOG_STUB << "Bad indexes count at " << fingersTree-> IndexFileName << " Key = " << hex << (int) theKey << " Offset = " << hex << (int) oldDataOffset << FN;
    }
    else
        EG_LOG_STUB << "Indexes chunk not found " << fingersTree-> IndexFileName << " Key = " << hex << (int) theKey << " Offset = " << hex << (int) oldDataOffset << FN;

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
    EG_LOG_STUB << "indexesChunkOffset = " << hex << (int) indexesChunkOffset <<
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

template <typename KeyType> bool EgIndexes<KeyType>::checkIndexesIntegrity()
{


   return true; // ok
}

/*
EgIndexes<qint32> EgIndexesqint32;
EgIndexes<quint32> EgIndexesquint32;
EgIndexes<qint64> EgIndexesqint64;
EgIndexes<quint64> EgIndexesquint64;
EgIndexes<float> EgIndexesfloat;
EgIndexes<double> EgIndexesdouble;
*/
