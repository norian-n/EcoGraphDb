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
    // localIndexStream << (quint64) fingersTree-> rootHeaderSize;

    memcpy(chunk, indexBA.constData(), indexChunkSize);
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

    indexStream << (quint64) indexHeaderSize;   // first chunk offset
    indexStream << (quint64) 0xABCDEF; // reserved for chunks recycling

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


template <typename KeyType> int EgIndexes<KeyType>::GetFingerOffset(quint64& fingerOffset)
{
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    localIndexStream.device()->seek(egChunkVolume * oneIndexSize + sizeof(quint64) * 2 + sizeof(keysCountType));
    localIndexStream >> fingerOffset;

    // qDebug() << "fingerOffset = " << hex << (int) fingerOffset << FN;

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::StoreIndexChunk(const char* chunkPtr, const quint64 chunkOffset)
{
    // PrintIndexesChunk(indexBA.data(), "chunk to save " + FNS);

    indexStream.device()-> seek(chunkOffset); // FIXME check result in paranoid mode

    // qDebug() << "seek result = " << seekRes << "fingersTree-> currentFinger.nextChunkOffset = " << hex << (int) fingersTree-> currentFinger.nextChunkOffset << FN;
    // qDebug() << "fingersTree-> currentFinger.nextChunkOffset = " << hex << (int) fingersTree-> currentFinger.nextChunkOffset << " , pos: " << hex << (int) indexStream.device()-> pos() << FN;

    // int res =
    return indexStream.device()-> write(chunkPtr, indexChunkSize);
/*
    if (res < 0)
    {
       qDebug() << "write() returns " << res << " , pos: " << indexStream.device()-> pos() << " " << indexStream.device()->errorString() << FN;

       return -1;
    }
*/
    // return 0;
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

    // if (! fingersTree-> currentFinger.nextChunkOffset)
    //    qDebug() << "ERROR: zero fingersTree-> currentFinger.nextChunkOffset" << FN;

    return indexStream.readRawData(indexBA.data(), indexChunkSize);
}

template <typename KeyType> inline int EgIndexes<KeyType>::LoadIndexChunk(char* chunkPtr, quint64 chunkOffset)
{
    indexStream.device()->seek(chunkOffset);

    // if (! indexStream.device()->seek(fingersTree-> currentFinger.nextChunkOffset)) // FIXME implement paranoid mode
    //    qDebug() << "ERROR: bad position of fingersTree-> currentFinger.nextChunkOffset = " << hex << fingersTree-> currentFinger.nextChunkOffset << FN;

    // if (! fingersTree-> currentFinger.nextChunkOffset)
    //    qDebug() << "ERROR: zero fingersTree-> currentFinger.nextChunkOffset" << FN;

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


template <typename KeyType> int EgIndexes<KeyType>::InsertInsideIndexChunk(QDataStream &localIndexesStream)
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

/*
if (theKey == 11)
{

    memcpy(chunk, zero_chunk, indexChunkSize);

    EgIndexStruct<KeyType> indexTest;

    indexTest.indexKey = theKey;
    indexTest.dataOffset = oldDataOffset;

    // memcpy(chunk,(const char *)&indexTest, sizeof(indexTest));

    indexPtr = (EgIndexStruct<KeyType>*) (chunk);

    *indexPtr = indexTest;

    PrintIndexesChunk(chunk, "chunk after test " + FNS);

    qDebug() <<"ptrIndex = " << (int) indexPtr-> indexKey
             << " , data offset = " << hex << (int) indexPtr-> dataOffset << FN;

    // qDebug() << QByteArray((char *) &indexTest, oneIndexSize).toHex() << FN;
}
*/

template <typename KeyType> int EgIndexes<KeyType>::InsertToIndexChunk()
{

    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(); // fingersTree-> currentFinger.nextChunkOffset to indexBA.data()

    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger);

    if (fingersTree-> currentFinger.itemsCount < egChunkVolume) // chunk has empty space
    {
        InsertInsideIndexChunk(localIndexStream);
        UpdateChunkCount(localIndexStream, fingersTree-> currentFinger.itemsCount + 1);

        StoreIndexChunk(); // indexBA.data() to fingersTree-> currentFinger.nextChunkOffset

            // update fingers
        fingersTree-> UpdateFingersChainAfterInsert();
    }
    else    // overflow - append or split chunk
    {
        if (theKey >= fingersTree-> currentFinger.maxKey)
            AppendIndexChunk(localIndexStream);
        else
            SplitIndexChunk(localIndexStream);
    }

    // PrintIndexesChunk(indexBA.data(), "chunk after insert " + FNS);

    return 0;
}

template <typename KeyType> int EgIndexes<KeyType>::SplitIndexChunk(QDataStream& localIndexStream)
{
    // KeyType minKey, maxKey;

    fingersTree-> newFinger.nextChunkOffset = indexStream.device()->size(); // append to the end of indexes file
    fingersTree-> newFinger.myLevel = 0;

    // indexPosition = FindIndexPosition(localIndexStream);

    indexPosition = FindPosByKeyLast(localIndexStream, EgIndexes<KeyType>::CompareLE) + 1;

    // qDebug() << "indexPosition = " << indexPosition << ", itemsCount = " << fingersTree-> currentFinger.itemsCount << FN;

    // PrintIndexesChunk(chunk, "old chunk loaded " + FNS);

        // init new chunk
    memmove (new_chunk, zero_chunk, indexChunkSize);

        // split chunks
    memcpy(chunk, indexBA.data(), indexChunkSize); // FIXME check

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
    StoreIndexChunk(chunk, fingersTree-> currentFinger.nextChunkOffset);

    StoreIndexChunk(new_chunk, fingersTree-> newFinger.nextChunkOffset);

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

        // update prev ptr of old chunk
    if (nextOffsetPtr)
    {
        localIndexStream.device()->seek(egChunkVolume * oneIndexSize);
        localIndexStream << fingersTree-> newFinger.nextChunkOffset;
    }

        // save old chunk
    StoreIndexChunk();

        // fill new chunk data
    memcpy(indexBA.data(), zero_chunk, indexChunkSize);

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
            // load proposed key
        // localIndexesStream.device()->seek(0);
        // localIndexesStream >> currentIndex;

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


        // load proposed key
    // localIndexesStream.device()->seek(indexPosition*oneIndexSize);
    // localIndexesStream >> currentIndex;

    ReadIndexOnly(localIndexesStream, currentIndex);

    // qDebug() << "indexPosition =  " << indexPosition << " ,currentIndex = " << hex << (int) currentIndex << FN;

        // compare
    if (myCompareFunc(currentIndex,theKey) && (indexPosition > 0)) // (currentIndex >= theKey)
    {
            // move down until currentIndex <
        do
        {
            indexPosition--;
            // localIndexesStream.device()->seek(indexPosition*oneIndexSize);
            // localIndexesStream >> currentIndex;
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
            // localIndexesStream.device()->seek(indexPosition*oneIndexSize);
            // localIndexesStream >> currentIndex;
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
        // localIndexesStream.device()->seek((fingersTree-> currentFinger.itemsCount - 1)*oneIndexSize);
        // localIndexesStream >> currentIndex;

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

        // load proposed key
    // localIndexesStream.device()->seek(indexPosition*oneIndexSize);
    // localIndexesStream >> currentIndex;

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
            // localIndexesStream.device()->seek(indexPosition*oneIndexSize);
            // localIndexesStream >> currentIndex;

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
            // localIndexesStream.device()->seek(indexPosition*oneIndexSize);
            // localIndexesStream >> currentIndex;

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

        // load chunk
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(); // fingersTree-> currentFinger.nextChunkOffset to indexBA.data()

    indexPosition = 0;

    // qDebug() << "pos =  " << indexPosition << FN;

    LoadDataUp(index_offsets, localIndexStream);

}

template <typename KeyType> void EgIndexes<KeyType>::LoadDataByChunkUp(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc)
{
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(); // indexBA.data()

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
    // KeyType currentIndex;
    quint64 dataOffset;

    // quint64 nextChunkPtr;

    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(); // indexBA.data()

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
        LoadIndexChunk(indexBA.data(), prevOffsetPtr);

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

    nextChunkPtr =  fingersTree-> currentFinger.nextChunkOffset;

    // qDebug() << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

    while (nextChunkPtr && (! keyOutOfRange))
    {
        LoadIndexChunk(indexBA.data(), nextChunkPtr);

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


template <typename KeyType> int EgIndexes<KeyType>::FindIndexByDataOffset(QDataStream &localIndexStream, bool isPrimary)
{
    KeyType currentIndex;
    quint64 dataOffset;

    // quint64 nextChunkPtr;

    bool keyOutOfRange = false;
    bool firstChunk = true;

    // QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    LoadIndexChunk(); // indexBA.data()

    // qDebug() << "theKey =  " << hex << (int) theKey  << " , oldDataOffset =  " << hex << (int) oldDataOffset << FN;

    // qDebug() << "indexBA =  " << indexBA.toHex() << FN;

    // nextChunkPtr =  fingersTree-> currentFinger.nextChunkOffset;

    // qDebug() << "nextChunkPtr =  " << hex << (int) nextChunkPtr  << " , chunkCount =  " << hex << (int) chunkCount << FN;

    while (fingersTree-> currentFinger.nextChunkOffset && (! keyOutOfRange))
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
        localIndexStream >> fingersTree-> currentFinger.nextChunkOffset;

        if (fingersTree-> currentFinger.nextChunkOffset)
            LoadIndexChunk(); // to indexBA.data()
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
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    // qDebug() << "theKey = " << theKey << ", IndexFileName = " << fingersTree-> IndexFileName << FN;

    if (FindIndexByDataOffset(localIndexStream, isPrimary) == 0) // index found
        UpdateDataOffset();
    else
        qDebug() << "Index to update not found, theKey = " << theKey << ", IndexFileName = " << fingersTree-> IndexFileName << FN;

}

template <typename KeyType> int EgIndexes<KeyType>::DeleteIndex(bool isPrimary)
{
    QDataStream localIndexStream(&indexBA, QIODevice::ReadWrite);

    // qDebug() << "theKey = " << theKey << ", IndexFileName = " << fingersTree-> IndexFileName << FN;

    // fingersTree-> PrintFingerInfo(fingersTree-> currentFinger, "DeleteIndex");

    if (FindIndexByDataOffset(localIndexStream, isPrimary) == 0) // index found
    {
        // qDebug() << "chunkCount 1 =  " << hex << (int) chunkCount << FN;

            // get finger ptr
        localIndexStream.device()->seek((egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2) + sizeof(keysCountType));
        localIndexStream >> fingersTree-> currentFingerOffset;

        if (chunkCount > 1)
        {
            DeleteDataOffset(localIndexStream); // count decrease

            // qDebug() << "chunkCount 2 =  " << hex << (int) chunkCount << FN;
            // qDebug() << "fingersTree-> currentFingerOffset " << hex << (int) fingersTree-> currentFingerOffset << FN;

            fingersTree-> currentKeysCount = chunkCount;
            fingersTree-> UpdateFingerCountAfterDelete(); // by fingersTree-> currentFingerOffset

            if (fingersTree-> minValueChanged)
                fingersTree-> UpdateMinValueUp();
            else if (fingersTree-> maxValueChanged)
                fingersTree-> UpdateMaxValueUp();
        }
        else if (chunkCount == 1)
        {

            // qDebug() << "fingersTree-> currentFingerOffset " << hex << (int) fingersTree-> currentFingerOffset << FN;

            if (fingersTree-> DeleteParentFinger() == 1) // last index
                return 1; // delete files

            // fingersTree-> PrintAllChunksInfo("After finger delete" + FNS);

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
    quint64 prevChunkPtr, nextChunkPtr;

        // get pointers
    indexStream.device()->seek(fingersTree-> currentFinger.nextChunkOffset + egChunkVolume * oneIndexSize);
    indexStream >> prevChunkPtr;
    indexStream >> nextChunkPtr;

    //qDebug() << "fingersTree-> currentFinger.nextChunkOffset = " << hex << (int) fingersTree-> currentFinger.nextChunkOffset <<
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

    StoreIndexChunk(zero_chunk, fingersTree-> currentFinger.nextChunkOffset);

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
