/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "../egDataNodesType.h"
#include "egFingers.h"
#include "egIndexes.h"

using namespace egIndexesNamespace;

template <typename KeyType> void EgFingers<KeyType>::PrintFingerInfo(egFinger<KeyType>& fingerInfo, const QString &theMessage)
{
    EG_LOG_STUB << "indexes: " << IndexFileName << theMessage;
    EG_LOG_STUB << "fingerInfo.minKey = " << hex << (int) fingerInfo.minKey;
    EG_LOG_STUB << "fingerInfo.maxKey = " << hex << (int) fingerInfo.maxKey;
    EG_LOG_STUB << "fingerInfo.myLevel = " << hex << (int) fingerInfo.myLevel;
    EG_LOG_STUB << "fingerInfo.itemsCount = " << hex << (int) fingerInfo.itemsCount;

    EG_LOG_STUB << "fingerInfo.nextChunkOffset = " << hex << (int) fingerInfo.nextChunkOffset;
    EG_LOG_STUB << "fingerInfo.myOffset = " << hex << (int) fingerInfo.myOffsetInChunk;
    EG_LOG_STUB << "fingerInfo.myChunkOffset = " << hex << (int) fingerInfo.myChunkOffset;

    // EG_LOG_STUB << "fingerInfo.fingersCount = " << hex << (int) fingerInfo.fingersCount;
    // EG_LOG_STUB << "fingerInfo.myLevel = " << hex << (int) fingerInfo.myLevel << endl;
}

template <typename KeyType> void EgFingers<KeyType>::PrintFingersChunk(char* theFingersChunk, const QString &theMessage)
{
    EG_LOG_STUB << QByteArray(theFingersChunk, fingersChunkSize).toHex() << theMessage;
}


template <typename KeyType> void EgFingers<KeyType>::PrintChunkInfo(quint64 fingersChunkOffset)
{
    KeyType minParent, maxParent; //, minReal, maxReal;
    keysCountType parentKeysCount;
    fingersLevelType myLevel;
    quint64 parentNextOffset;

        // get parent finger offset
    fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
    fingerStream >> parentFingerOffset;
    fingerStream >> myLevel;

    // get parent finger info : count, my offset, min/max

    if (parentFingerOffset)
    {

        fingerStream.device()->seek(parentFingerOffset);
        fingerStream >> minParent;
        fingerStream >> maxParent;
        fingerStream >> parentKeysCount;
        fingerStream >> parentNextOffset;
    }
    else
    {
        minParent = rootFinger.minKey;
        maxParent = rootFinger.maxKey;
        parentKeysCount = rootFinger.itemsCount;
        parentNextOffset = rootFinger.nextChunkOffset;
    }

    // TODO get real min/max keys by count

    EG_LOG_STUB << "myChunkOffset = " << hex << (int) fingersChunkOffset
             << ", parentNextOffset = " << hex << (int) parentNextOffset;

    EG_LOG_STUB << "Parent: minKey = " << hex << (int) minParent
             << ", maxKey = " << hex << (int) maxParent
             << ", itemsCount = " << (int) parentKeysCount;

    EG_LOG_STUB << "myLevel = " << (int) myLevel;

    EG_LOG_STUB << "parentFingerOffset = " << hex << (int) parentFingerOffset << endl;

}

template <typename KeyType> void EgFingers<KeyType>::PrintAllChunksInfo(const QString& theMessage)
{
    QList<quint64> chunksList;
    QList<quint64> branchesList;

    quint64 nextOffset, parentOffset, branchOffset;

    keysCountType chunkKeysCount;
    fingersLevelType myLevel;

        // start with top chunk offset
    nextOffset = rootFinger.nextChunkOffset;
    chunkKeysCount = rootFinger.itemsCount;

            // get top level
    fingerStream.device()->seek(nextOffset + egChunkVolume * oneFingerSize + sizeof(quint64));
    fingerStream >> myLevel;

    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader");

    // EG_LOG_STUB << "nextOffset = " << hex << (int) nextOffset << FN;

    chunksList.append(nextOffset);

        // add fingers if level > 0
    if (myLevel > 0)
        for (int i = 0; i < chunkKeysCount; i++)
        {
            fingerStream.device()->seek(nextOffset + i * oneFingerSize + sizeof(KeyType)*2 + sizeof(keysCountType));
            fingerStream >> branchOffset;

            // EG_LOG_STUB << "nextOffset = " << hex << (int) nextOffset << "branchOffset = " << hex << (int) branchOffset << FN;

            branchesList.append(branchOffset);
        }

        // process branches
    while (! branchesList.isEmpty())
    {
        nextOffset = branchesList.first();
        branchesList.pop_front();

        // EG_LOG_STUB << "nextOffset = " << hex << (int) nextOffset << FN;

        chunksList.append(nextOffset);

        fingerStream.device()->seek(nextOffset + egChunkVolume * oneFingerSize);
        fingerStream >> parentOffset;
        fingerStream >> myLevel;

        if (myLevel > 0)
        {
            if (parentOffset)
            {
                fingerStream.device()->seek(parentOffset + sizeof(KeyType)*2);
                fingerStream >> chunkKeysCount;
            }
            else
                chunkKeysCount = rootFinger.itemsCount;

            for (int i = 0; i < chunkKeysCount; i++)
            {
                fingerStream.device()->seek(nextOffset + i * oneFingerSize + sizeof(KeyType)*2 + sizeof(keysCountType));
                fingerStream >> branchOffset;

                // EG_LOG_STUB << "nextOffset = " << hex << (int) nextOffset << "branchOffset = " << hex << (int) branchOffset << FN;

                branchesList.append(branchOffset);
            }
        }
    }

        // print list
    EG_LOG_STUB << theMessage;

    for(QList<quint64>::iterator chunksIterator = chunksList.begin(); chunksIterator != chunksList.end(); ++chunksIterator)
        PrintChunkInfo(*chunksIterator);

}


template <typename KeyType> void EgFingers<KeyType>::InitRootFinger()
{
    rootFinger.minKey = indexChunks-> theKey;
    rootFinger.maxKey = indexChunks-> theKey;

    rootFinger.myLevel = 0;
    rootFinger.itemsCount = 1;

    rootFinger.nextChunkOffset = indexHeaderSize;
}


template <typename KeyType> void EgFingers<KeyType>::LoadRootFinger()
{
    // EG_LOG_STUB << "fingerFile.fileName() = " << fingerFile.fileName() << FN;
    // EG_LOG_STUB << "fingerFile size = " << fingerStream.device()->size() << FN;

    fingerStream.device()-> seek(0);

    fingerStream >> rootFinger.minKey;
    fingerStream >> rootFinger.maxKey;

    fingerStream >> rootFinger.itemsCount;
    fingerStream >> rootFinger.nextChunkOffset;

    fingerStream >> rootFinger.myLevel;

    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);
}

template <typename KeyType> void EgFingers<KeyType>::StoreRootFinger()
{
    fingerStream.device()->seek(0);

    fingerStream << rootFinger.minKey;
    fingerStream << rootFinger.maxKey;

    fingerStream << rootFinger.itemsCount;
    fingerStream << rootFinger.nextChunkOffset;

    fingerStream << rootFinger.myLevel;
}

template <typename KeyType> void EgFingers<KeyType>::StoreRootFingerMinMaxOnly()
{
    fingerStream.device()->seek(0);

    fingerStream << rootFinger.minKey;
    fingerStream << rootFinger.maxKey;
}


template <typename KeyType> inline void EgFingers<KeyType>::InitFingersChunk()
{   
    memset(fingersBA.data(), 0, fingersChunkSize);

        // write initial values
    localStream->device()-> seek(0);
    *localStream << (KeyType) indexChunks-> theKey;               // min
    *localStream << (KeyType) indexChunks-> theKey;               // max
    *localStream << (keysCountType) 1;    // count
    *localStream << (quint64) indexHeaderSize; // first indexes chunk offset // indexChunks-> indexesChunkOffset; // theFinger.chunkOffset;
}



template <typename KeyType> int EgFingers<KeyType>::StoreFingersChunk(quint64 fingersChunkOffset, char* chunkPtr)
{
    fingerStream.device()-> seek(fingersChunkOffset);
    // fingerStream.writeRawData(chunkPtr, fingersChunkSize);

        // int res =
    fingerStream.device()-> write(chunkPtr, fingersChunkSize);

    // EG_LOG_STUB << "fingersChunkOffset" << hex << (int) fingersChunkOffset << FN;
    return 0; // FIXME
}

template <typename KeyType> int EgFingers<KeyType>::StoreParentOffset(quint64 fingersChunkOffset, quint64 parentFingerOffset)
{
    fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
    fingerStream << parentFingerOffset;

    // EG_LOG_STUB << "fingersChunkOffset" << hex << (int) fingersChunkOffset << ", parentFingerOffset" << hex << (int) parentFingerOffset << FN;
    return 0;
}

/*
template <typename KeyType> int EgFingers<KeyType>::GetParentOffset(quint64& parentFingerOffset)
{
    localStream->device()-> seek(egChunkVolume * oneFingerSize);
    *localStream >> parentFingerOffset;

    // EG_LOG_STUB << "fingerOffset = " << hex << (int) fingerOffset << FN;
    return 0;
}
*/

template <typename KeyType> int EgFingers<KeyType>::LoadFingersChunk()
{
    fingerStream.device()->seek(parentFinger.nextChunkOffset);
    fingerStream.readRawData(fingersBA.data(), fingersChunkSize);

    return 0;
}

/*
template <typename KeyType> int EgFingers<KeyType>::LoadFingersChunkExplicit(char* chunkPtr, const quint64 fingersChunkOffset)
{
    fingerStream.device()->seek(fingersChunkOffset);
    fingerStream.readRawData(chunkPtr, fingersChunkSize);

    return 0;
}
*/

template <typename KeyType> void EgFingers<KeyType>::LoadFingerDirect(egFinger<KeyType>& theFinger, const quint64 fingerOffset)
{
    theFinger.myOffsetInChunk = (fingerOffset - rootHeaderSize) % fingersChunkSize;
    theFinger.myChunkOffset   =  fingerOffset - currentFinger.myOffsetInChunk;

    fingerStream.device()->seek(fingerOffset);

    fingerStream >> theFinger.minKey;
    fingerStream >> theFinger.maxKey;
    fingerStream >> theFinger.itemsCount;
    fingerStream >> theFinger.nextChunkOffset;
}

template <typename KeyType> void EgFingers<KeyType>::ReadFinger(egFinger<KeyType>& theFinger, const int fingerPosition)
{
    theFinger.myOffsetInChunk = fingerPosition * oneFingerSize;

    localStream->device()->seek(theFinger.myOffsetInChunk);

    *localStream >> theFinger.minKey;
    *localStream >> theFinger.maxKey;
    *localStream >> theFinger.itemsCount;
    *localStream >> theFinger.nextChunkOffset;
}

// *****************************************************************************************

template <typename KeyType> void EgFingers<KeyType>::FindFingerGE()
{
    int fingerPosition = 0;

    do
        ReadFinger(currentFinger, fingerPosition);
    while ((currentFinger.maxKey < indexChunks-> theKey) && (++fingerPosition < parentFinger.itemsCount));
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkGE()
{
    int result = 0;

    // EG_LOG_STUB << "theKey = " << hex << indexChunks-> theKey << FN;

    if (indexChunks-> theKey > rootFinger.maxKey) // key out of range
            return 1;

    if (rootFinger.myLevel == 0) // only root finger exists
    {
        currentFinger = rootFinger;
        return 0;
    }

    parentFinger = rootFinger;

        // go fingers chain
    do
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

        LoadFingersChunk(); // to fingersBA.data() from parentFinger.nextChunkOffset
        GetChunkLevel();    // to currentFinger.myLevel

        // EG_LOG_STUB << "myLevel = " << currentFinger.myLevel << FN;

            // if one finger just return it
        if (parentFinger.itemsCount == 1)
            ReadFinger(currentFinger, 0);
        else
            FindFingerGE();  // go get currentFinger

        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        if (currentFinger.myLevel > 0)
            parentFinger = currentFinger;
    }
    while ((currentFinger.myLevel > 0) && !result);

    currentFinger.myChunkOffset = parentFinger.nextChunkOffset;  // FIXME chck if required

    return result;
}

template <typename KeyType> void EgFingers<KeyType>::FindFingerGT()
{
    int fingerPosition = 0;

    do
        ReadFinger(currentFinger, fingerPosition);
    while ((currentFinger.maxKey <= indexChunks-> theKey) && (++fingerPosition < parentFinger.itemsCount));
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkGT()
{
    int result = 0;

    // EG_LOG_STUB << "theKey = " << hex << indexChunks-> theKey << FN;

    if (indexChunks-> theKey >= rootFinger.maxKey) // key out of range
            return 1;

    if (rootFinger.myLevel == 0) // only root finger exists
    {
        currentFinger = rootFinger;
        return 0;
    }

    parentFinger = rootFinger;

        // go fingers chain
    do
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

        LoadFingersChunk(); // to fingersBA.data() from parentFinger.nextChunkOffset
        GetChunkLevel(); // to currentFinger.myLevel

        // EG_LOG_STUB << "myLevel = " << currentFinger.myLevel << FN;

            // if one finger just return it
        if (parentFinger.itemsCount == 1)
            ReadFinger(currentFinger, 0);
        else
            FindFingerGT();  // go get currentFinger

        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        if (currentFinger.myLevel > 0)
            parentFinger = currentFinger;
    }
    while ((currentFinger.myLevel > 0) && !result);

    currentFinger.myChunkOffset = parentFinger.nextChunkOffset;  // FIXME chck if required

    return result;
}

template <typename KeyType> void EgFingers<KeyType>::FindFingerLT()
{   
    int fingerPosition = parentFinger.itemsCount-1;

    do
        ReadFinger(currentFinger, fingerPosition);
    while ((currentFinger.minKey >= indexChunks-> theKey) && (--fingerPosition >= 0));
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkLT()
{
    int result = 0;

    // EG_LOG_STUB << "theKey = " << hex << indexChunks-> theKey << FN;

    if (indexChunks-> theKey <= rootFinger.minKey) // key out of range
            return 1;

    if (rootFinger.myLevel == 0) // only root finger exists
    {
        currentFinger = rootFinger;
        return 0;
    }

    parentFinger = rootFinger;

        // go fingers chain
    do
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

        LoadFingersChunk(); // to fingersBA.data() from parentFinger.nextChunkOffset
        GetChunkLevel();    // to currentFinger.myLevel

        // EG_LOG_STUB << "myLevel = " << currentFinger.myLevel << FN;

            // if one finger just return it
        if (parentFinger.itemsCount == 1)
            ReadFinger(currentFinger, 0);
        else
            FindFingerLT();  // go get currentFinger

        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        if (currentFinger.myLevel > 0)
            parentFinger = currentFinger;
    }
    while ((currentFinger.myLevel > 0) && !result);

    currentFinger.myChunkOffset = parentFinger.nextChunkOffset;  // FIXME chck if required

    return result;
}


template <typename KeyType> void EgFingers<KeyType>::FindFingerLE()
{
    int fingerPosition = parentFinger.itemsCount-1;

    do
        ReadFinger(currentFinger, fingerPosition);
    while ((currentFinger.minKey > indexChunks-> theKey) && (--fingerPosition >= 0));
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkLE()
{
    int result = 0;

    // EG_LOG_STUB << "theKey = " << hex << indexChunks-> theKey << FN;

    if (indexChunks-> theKey < rootFinger.minKey) // key out of range
            return 1;

    if (rootFinger.myLevel == 0) // only root finger exists
    {
        currentFinger = rootFinger;
        return 0;
    }

    parentFinger = rootFinger;

        // go fingers chain
    do
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

        LoadFingersChunk(); // to fingersBA.data() from parentFinger.nextChunkOffset
        GetChunkLevel();    // to currentFinger.myLevel

        // EG_LOG_STUB << "myLevel = " << currentFinger.myLevel << FN;

            // if one finger just return it
        if (parentFinger.itemsCount == 1)
            ReadFinger(currentFinger, 0);
        else
            FindFingerLE();  // go get currentFinger

        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        if (currentFinger.myLevel > 0)
            parentFinger = currentFinger;
    }
    while ((currentFinger.myLevel > 0) && !result);

    currentFinger.myChunkOffset = parentFinger.nextChunkOffset;  // FIXME chck if required

    return result;
}

template <typename KeyType> int EgFingers<KeyType>::FindFingerEQ()
{
    int fingerPosition = 0;

    do
        ReadFinger(currentFinger, fingerPosition);
    while ((currentFinger.maxKey < indexChunks-> theKey) && (++fingerPosition < parentFinger.itemsCount));

        // check if key between fingers
    if (indexChunks-> theKey < currentFinger.minKey) // ||  (fingerPosition >= parentFinger.itemsCount)
        return 1;

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkEQ()
{
    int result = 0;

    // EG_LOG_STUB << "theKey = " << hex << indexChunks-> theKey << FN;

    if ((indexChunks-> theKey < rootFinger.minKey) || (indexChunks-> theKey > rootFinger.maxKey)) // key out of range
            return 1;

    if (rootFinger.myLevel == 0) // only root finger exists
    {
        currentFinger = rootFinger;
        return 0;
    }

    parentFinger = rootFinger;

        // go fingers chain
    do
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

        LoadFingersChunk(); // to fingersBA.data() from parentFinger.nextChunkOffset
        GetChunkLevel();    // to currentFinger.myLevel

        // EG_LOG_STUB << "myLevel = " << currentFinger.myLevel << FN;

            // if one finger just return it
        if (parentFinger.itemsCount == 1)
            ReadFinger(currentFinger, 0);
        else
            result = FindFingerEQ();  // go get currentFinger, 1 - between fingers, key not found, exit

        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        if (currentFinger.myLevel > 0)
            parentFinger = currentFinger;
    }
    while ((currentFinger.myLevel > 0) && !result);

    currentFinger.myChunkOffset = parentFinger.nextChunkOffset;  // FIXME chck if required

    return result;
}

template <typename KeyType> inline void EgFingers<KeyType>::GetChunkLevel()
{
    localStream->device()-> seek(egChunkVolume * oneFingerSize + sizeof(quint64));
    *localStream >> currentFinger.myLevel;
}

template <typename KeyType> inline void EgFingers<KeyType>::UpdateChunkLevel(const fingersLevelType& theLevel)
{
    localStream->device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
    *localStream << theLevel;
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkToInsert()
{
    LoadRootFinger();

    if (rootFinger.myLevel == 0) // root only
    {
        currentFinger = rootFinger;

        return 0;
    }

    // FIXME TODO check global min/max and use first/last index chunk directly

    // int res = 0;

    parentFinger = rootFinger;
    fingersChain.clear();

    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);

        // fill fingers chain
    do
    {
        LoadFingersChunk(); // to fingersBA.data() from parentFinger.nextChunkOffset

        GetChunkLevel();
        FindFingerLE();

        currentFinger.myChunkOffset = parentFinger.nextChunkOffset;
        fingersChain.append(parentFinger);

        if ( currentFinger.myLevel > 0 )
            parentFinger = currentFinger;
    }
    while ((currentFinger.myLevel > 0) /*&& (res >= 0)*/);

    // EG_LOG_STUB << "theKey = " << hex << (int) indexChunks-> theKey << FN;

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    return 0; // res;
}

template <typename KeyType> inline void EgFingers<KeyType>::GetFingerByOffset(quint64 updatedFingerOffset)
{
    currentFinger.myOffsetInChunk = (updatedFingerOffset - rootHeaderSize) % fingersChunkSize;
    currentFinger.myChunkOffset   =  updatedFingerOffset - currentFinger.myOffsetInChunk;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateCurrentFingerAfterInsert()
{
    // PrintFingerInfo(currentFinger, "currentFinger before " + FNS );

    UpdateTheFingerMinMax(currentFinger);
    currentFinger.itemsCount++;

    // PrintFingerInfo(currentFinger, "currentFinger after " + FNS );

    fingerStream.device()->seek(currentFinger.myChunkOffset + currentFinger.myOffsetInChunk);

        // write one finger
    fingerStream << currentFinger.minKey;
    fingerStream << currentFinger.maxKey;
    fingerStream << currentFinger.itemsCount;

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateFingersChainAfterInsert()
{
    if (rootFinger.myLevel == 0) // root only
    {
        UpdateTheFingerMinMax(rootFinger);
        rootFinger.itemsCount++;

        StoreRootFinger();

        return 0;
    }

    UpdateCurrentFingerAfterInsert();

    bool indexOutOfRange = true;

            // all chain up
    while ((! fingersChain.isEmpty()) && indexOutOfRange)
    {
        indexOutOfRange = (indexChunks-> theKey < fingersChain.last().minKey) || (indexChunks-> theKey > fingersChain.last().maxKey);

        if (indexOutOfRange)
        {
            UpdateTheFingerMinMax(fingersChain.last()); // do not increment count

            // PrintFingerInfo(fingersChain.last(), "parentFinger " + FNS);

            fingerStream.device()->seek(fingersChain.last().myChunkOffset + fingersChain.last().myOffsetInChunk);

                // write one finger only
            fingerStream << fingersChain.last().minKey;    // min
            fingerStream << fingersChain.last().maxKey;    // max
        }

        fingersChain.removeLast();
    }

        // update root header
    if (indexOutOfRange)
    {
        UpdateTheFingerMinMax(rootFinger);   // do not increment count

        // indexChunks-> StoreRootHeader(true); // true means store min max only
        StoreRootFingerMinMaxOnly();
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateFingerCountAfterDelete(keysCountType newKeysCount)
{
    if (rootFinger.myLevel == 0) // only root
    {
        fingerStream.device()->seek(sizeof(KeyType) * 2 + sizeof(fingersLevelType));
        fingerStream << newKeysCount;

        return 0;
    }

        // update keys count, decremented by indexes
    fingerStream.device()->seek(currentFingerOffset + sizeof(KeyType) * 2);
    fingerStream << newKeysCount;

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateMinValueUp()
{
    if (rootFinger.myLevel == 0) // root only
    {
        if (newMinValue != rootFinger.minKey)
        {
            rootFinger.minKey = newMinValue;

            fingerStream.device()->seek(0); // root
            fingerStream << newMinValue;
        }
        return 0;
    }

    KeyType oldValue;

        // iterate tree
    while (currentFingerOffset)
    {
            // get old value
        fingerStream.device()->seek(currentFingerOffset);
        fingerStream >> oldValue;

        if (newMinValue != oldValue) // changed
        {
                // update finger
            fingerStream.device()->seek(currentFingerOffset);
            fingerStream << newMinValue;

                // check finger offset in chunk
            fingersChunkOffset = currentFingerOffset - ( (currentFingerOffset-rootHeaderSize) % fingersChunkSize );

            if (currentFingerOffset == fingersChunkOffset) // first finger, go up tree
            {
                fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
                fingerStream >> currentFingerOffset; // parentFingerOffset
            }
            else
                return 0; // not first finger
        }
        else
            return 0; // min not changed FIXME move to indexes
    }

    if (newMinValue != rootFinger.minKey)
    {
        rootFinger.minKey = newMinValue;

        fingerStream.device()->seek(0); // root
        fingerStream << newMinValue;
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateMaxValueUp() // recursive
{
    if (rootFinger.myLevel == 0) // root only
    {
        if (newMaxValue != rootFinger.maxKey)
        {
            rootFinger.maxKey = newMaxValue;

            fingerStream.device()->seek(sizeof(KeyType)); // root
            fingerStream << newMaxValue;
        }
        return 0;
    }


    KeyType oldValue;

        // iterate tree
    while (currentFingerOffset)
    {
        // get old value
        fingerStream.device()->seek(currentFingerOffset + sizeof(KeyType));
        fingerStream >> oldValue;

        if (newMaxValue != oldValue) // changed
        {
                // update finger
            fingerStream.device()->seek(currentFingerOffset + sizeof(KeyType));
            fingerStream << newMaxValue;

                // check finger offset in chunk
            fingersChunkOffset = currentFingerOffset - ( (currentFingerOffset-rootHeaderSize) % fingersChunkSize );

            keysCountType fingerNum = (currentFingerOffset - fingersChunkOffset) / oneFingerSize;

                // get fingers count
            fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
            fingerStream >> parentFingerOffset;

            if (parentFingerOffset)
            {
                fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType)*2);
                fingerStream >> currentKeysCount;
            }
            else
                currentKeysCount = rootFinger.itemsCount;

            if (fingerNum == (currentKeysCount - 1)) // last one
                currentFingerOffset = parentFingerOffset;
            else
                return 0; // not first finger
        }
        else
            return 0; // max not changed FIXME move to indexes
    }

    if (newMaxValue != rootFinger.maxKey)
    {
        rootFinger.maxKey = newMaxValue;

        fingerStream.device()->seek(sizeof(KeyType)); // root
        fingerStream << newMaxValue;
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateFingersChainAfterDelete()
{
    KeyType oldValue;

    keysCountType fingersCount;

    bool isFirstFinger;
    bool isLastFinger;

        // check / update root header
    if(! parentFingerOffset)
    {
        if (maxValueChanged || minValueChanged)
        {
            if (maxValueChanged)
                rootFinger.maxKey =  newMaxValue;
            else if (minValueChanged)
                rootFinger.minKey =  newMinValue;

            StoreRootFingerMinMaxOnly();
        }

        return 0;
    }

    while (parentFingerOffset && (maxValueChanged || minValueChanged))
    {
        currentFingerOffset = parentFingerOffset; // up

            // calc this chunk offset
        quint64 fingersChunkOffset = currentFingerOffset - ( currentFingerOffset % fingersChunkSize );

            // get next parent finger offset
        fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
        fingerStream >> parentFingerOffset;

            // check first/last finger for min/max update of root finger
        keysCountType fingerNum = (currentFingerOffset - fingersChunkOffset) / oneFingerSize;

        if (parentFingerOffset)
        {
            fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType)*2);
            fingerStream >> fingersCount;
        }
        else
            fingersCount = rootFinger.itemsCount;

        isFirstFinger = (fingerNum == 0);
        isLastFinger  = (fingerNum == fingersCount-1);

        if (maxValueChanged)
        {
            fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType));
            fingerStream >> oldValue;

            if (newMaxValue < oldValue)
            {
                fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType));
                fingerStream << newMaxValue;
            }

            if ((newMaxValue >= oldValue) || (! isLastFinger))
            {
                maxValueChanged = false;
                break;
            }

            // EG_LOG_STUB << "newMaxValue = " << newMaxValue << FN;
        }
        else if (minValueChanged)
        {
            fingerStream.device()->seek(parentFingerOffset);
            fingerStream >> oldValue;

            if (newMinValue > oldValue)
            {
                fingerStream.device()->seek(parentFingerOffset);
                fingerStream << newMinValue;
            }

            if ((newMinValue <= oldValue) || (! isFirstFinger))
            {
                minValueChanged = false;
                break;
            }

            // EG_LOG_STUB << "newMinValue = " << newMinValue << FN;
        }
        else
        {
            maxValueChanged = false;
            minValueChanged = false;
            break;
        }
    }   // while

        // EG_LOG_STUB << "parentFingerOffset = " << parentFingerOffset << FN;

        // check / update root header
    if(! parentFingerOffset)
    {
        if (maxValueChanged || minValueChanged)
        {
            if (maxValueChanged)
                rootFinger.maxKey =  newMaxValue;
            else if (minValueChanged)
                rootFinger.minKey =  newMinValue;

            StoreRootFingerMinMaxOnly();
        }
    }

        // reset
    maxValueChanged = false;
    minValueChanged = false;

    return 0;
}

template <typename KeyType> inline void EgFingers<KeyType>::DeleteSpecificFinger(keysCountType keysCount)
{
    parentFinger.nextChunkOffset = currentFinger.myChunkOffset;
    LoadFingersChunk();     // to fingersBA.data() by currentFinger.myChunkOffset);

    keysCountType fingerPosition = currentFinger.myOffsetInChunk / oneFingerSize;

    GetChunkLevel();        // to currentFinger.myLevel

    // EG_LOG_STUB << "keysCount = " << keysCount << ", fingerPosition = " << fingerPosition << ", myLocalLevel = " << myLocalLevel << FN;
    // PrintFingersChunk(fingersBA.data(), "fingers chunk before delete " + FNS);

        // move tail if required
    if (fingerPosition < keysCount-1)
    {
        memmove (fingersBA.data() + fingerPosition*oneFingerSize, fingersBA.data() + (fingerPosition+1)*oneFingerSize,  oneFingerSize*(keysCount - 1 - fingerPosition));
        UpdateBackptrOffsets(currentFinger.myChunkOffset,  fingerPosition, keysCount-1, currentFinger.myLevel);
    }

    memset(fingersBA.data()+ oneFingerSize*(keysCount-1), 0, oneFingerSize);

    StoreFingersChunk(currentFinger.myChunkOffset, fingersBA.data());

    // PrintFingersChunk(fingersBA.data(), "fingers chunk after delete " + FNS);

    localStream->device()-> seek(0);
    *localStream >> currentFinger.minKey; // level up

    localStream->device()-> seek(oneFingerSize*(keysCount-2) + sizeof(KeyType));
    *localStream >> currentFinger.maxKey; // level up
}

template <typename KeyType> void EgFingers<KeyType>::DeleteFingersChunk(quint64 fingersChunkOffset)
{
        // init zero chunk
    memset(fingersBA.data(), 0, fingersChunkSize);
    StoreFingersChunk(fingersChunkOffset, fingersBA.data());
}

template <typename KeyType> int EgFingers<KeyType>::DeleteParentFinger() // recursive
{
    if (rootFinger.myLevel == 0) // root only
    {
        return 1; // delete files
    }

    while (currentFingerOffset)
    {
        GetFingerByOffset(currentFingerOffset); // set current finger offsets

            // get parent
        fingerStream.device()->seek(currentFinger.myChunkOffset + egChunkVolume * oneFingerSize);
        fingerStream >> parentFingerOffset;

            // check fingers count
        if (parentFingerOffset) // parent not root
        {
            fingerStream.device()->seek(parentFingerOffset);
            fingerStream >> parentFinger.minKey;
            fingerStream >> parentFinger.maxKey;
            fingerStream >> currentKeysCount;
        }
        else
        {
            parentFinger = rootFinger;
            currentKeysCount = rootFinger.itemsCount;
        }

/*
        PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        EG_LOG_STUB << "currentFingerOffset " << hex << (int) currentFingerOffset
                 << " , parentFingerOffset " << hex << (int) parentFingerOffset
                 << " , currentKeysCount = " << currentKeysCount << FN;
*/

        if (currentKeysCount > 1) // not last finger
        {
            DeleteSpecificFinger(currentKeysCount); // currentFinger

                // update count
            fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType)*2);
            fingerStream << (keysCountType) (currentKeysCount - 1);

            if (parentFinger.minKey < currentFinger.minKey)
            {
                newMinValue = currentFinger.minKey;
                currentFingerOffset = parentFingerOffset;
                UpdateMinValueUp();
            }
            else if (parentFinger.maxKey > currentFinger.maxKey)
            {
                newMaxValue = currentFinger.maxKey;
                currentFingerOffset = parentFingerOffset;
                UpdateMaxValueUp();
            }

            return 0;
        }
        else // last finger
        {
            DeleteFingersChunk(currentFinger.myChunkOffset); // just write zeroes

            currentFingerOffset = parentFingerOffset; // go up, delete parent
        }
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateFingersChainAfterSplit()
{
    bool splitParentChunk = false;
    bool appendMode = true;

    if (rootFinger.myLevel == 0) // no fingers chunks yet
    {
            // add new chunk under root with currentFinger and newFinger
        AddNewSubRootChunk();

        return 0;
    }

    while (! fingersChain.isEmpty())
    {
            // get parent from chain
        parentFinger = fingersChain.last();
        fingersChain.removeLast();

        appendMode = (indexChunks-> theKey >= parentFinger.maxKey); // FIXME check

        LoadFingersChunk(); // to fingersBA.data() from parentFinger.nextChunkOffset

        if (parentFinger.itemsCount < egChunkVolume)
        {
            InsertSplittedFinger();     // add new item  to localStream and recalc min/max

            splitParentChunk = false;

            break;  // get out
        }
        else
        {
            if (appendMode)
                AppendFingersChunk();
            else
                SplitFingersChunk();

            splitParentChunk = true;
        }
    }

    if (fingersChain.isEmpty() && splitParentChunk) // new root chunk is required
    {
        if (appendMode)
            currentFinger = rootFinger; // level up

            // add new chunk under root with currentFinger and newFinger
        AddNewSubRootChunk();
    }
    else if (fingersChain.isEmpty())
    {
        UpdateTheFingerMinMax(rootFinger);
        StoreRootFinger();
    }

    /*
    PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    PrintFingerInfo(newFinger, "newFinger " + FNS);
    */

    return 0;
}

template <typename KeyType> inline void EgFingers<KeyType>::UpdateTheFingerMinMax(egFinger<KeyType>& theFinger)
{
    if (indexChunks-> theKey > theFinger.maxKey)
        theFinger.maxKey =  indexChunks-> theKey;
    else if (indexChunks-> theKey < theFinger.minKey)
        theFinger.minKey =  indexChunks-> theKey;
}

template <typename KeyType> inline void EgFingers<KeyType>::AddNewSubRootChunk()
{
    rootFinger.nextChunkOffset = fingerStream.device()->size();

        // update root finger
    rootFinger.minKey = currentFinger.minKey;
    rootFinger.maxKey = newFinger.maxKey;

    rootFinger.myLevel++;
    rootFinger.itemsCount = 2;

    StoreRootFinger();

    memset(fingersBA.data(), 0, fingersChunkSize);

    localStream->device()-> seek(0);

    WriteFinger(*localStream, currentFinger);     // update current finger
    WriteFinger(*localStream, newFinger);         // add new finger

        // set level
    localStream->device()-> seek(egChunkVolume * oneFingerSize + sizeof(quint64));
    *localStream << (keysCountType) (rootFinger.myLevel - 1);

    StoreFingersChunk(rootFinger.nextChunkOffset, fingersBA.data());

            // backptrs to fingers in chunk
    if (rootFinger.myLevel == 1)
    {
        indexChunks-> StoreFingerOffset(currentFinger.nextChunkOffset, rootFinger.nextChunkOffset);
        indexChunks-> StoreFingerOffset(newFinger.nextChunkOffset, rootFinger.nextChunkOffset + oneFingerSize);
    }
    else
    {
        StoreParentOffset(currentFinger.nextChunkOffset, rootFinger.nextChunkOffset);
        StoreParentOffset(newFinger.nextChunkOffset, rootFinger.nextChunkOffset + oneFingerSize);
    }
}

template <typename KeyType> inline void EgFingers<KeyType>::WriteFinger(QDataStream &localFingersStream, egFinger<KeyType>& theFinger)
{
    localFingersStream << theFinger.minKey;
    localFingersStream << theFinger.maxKey;
    localFingersStream << theFinger.itemsCount;
    localFingersStream << theFinger.nextChunkOffset; // theFinger.chunkOffset;
}

template <typename KeyType> inline void EgFingers<KeyType>::MoveTailToInsert(char* chunkPtr, int fingerPosition, int fingersToMove)
{
    // EG_LOG_STUB << "fingerPosition = " << fingerPosition << ", fingersToMove = " << fingersToMove  << FN;
    memmove (chunkPtr + (fingerPosition+1)*oneFingerSize, chunkPtr + fingerPosition*oneFingerSize, fingersToMove*oneFingerSize);
    // PrintFingersChunk(chunkPtr, "chunk after tail move " + FNS);
}


template <typename KeyType> int EgFingers<KeyType>::InsertSplittedFinger()
{
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    // PrintFingerInfo(newFinger, "newFinger " + FNS);

        // update current finger
    localStream->device()-> seek(currentFinger.myOffsetInChunk);
    WriteFinger(*localStream, currentFinger);

    posToInsert = currentFinger.myOffsetInChunk/oneFingerSize + 1;

    InsertNewFinger(posToInsert, parentFinger.itemsCount); // to localStream

    // EG_LOG_STUB << "backlink = " << hex << currentFinger.myChunkOffset + posToInsert*oneFingerSize << ", newFinger.nextChunkOffset = " << hex << newFinger.nextChunkOffset << FN;

        // update backlinks
    // if (currentFinger.myLevel == 0)
    //    indexChunks-> StoreFingerOffset(newFinger.nextChunkOffset, currentFinger.myChunkOffset + posToInsert*oneFingerSize);

    // memcpy(fingersChunk, fingersBA.constData(), fingersChunkSize);

    UpdateBackptrOffsets(parentFinger.nextChunkOffset, posToInsert, parentFinger.itemsCount+1, currentFinger.myLevel);

    StoreFingersChunk(parentFinger.nextChunkOffset, fingersBA.data());

    // PrintFingersChunk(fingersBA.data(), "fingers chunk after insert " + FNS);

    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

    if (parentFinger.myLevel != rootFinger.myLevel) // not a root
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        UpdateTheFingerMinMax(parentFinger);
        parentFinger.itemsCount++;

        fingerStream.device()->seek(parentFinger.myChunkOffset + parentFinger.myOffsetInChunk);
        WriteFinger(fingerStream, parentFinger);
    }
    else
    {
        // UpdateTheFingerMinMax(rootFinger);
        UpdateTheFingerMinMax(rootFinger);
        rootFinger.itemsCount++;

        StoreRootFinger();
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::InsertNewFinger(int posToInsert, int itemsCount)
{
    if (posToInsert < itemsCount) // not last
        MoveTailToInsert(fingersBA.data(), posToInsert, itemsCount - posToInsert);

        // write new finger to the chunk at position
    localStream->device()-> seek(posToInsert*oneFingerSize);
    WriteFinger(*localStream, newFinger);

    // PrintFingerInfo(newFinger, "newFinger " + FNS);
    // PrintFingersChunk(fingersBA.data(), "chunk after insert " + FNS);

    return 0;
}

template <typename KeyType> void EgFingers<KeyType>::UpdateMinMax(egFinger<KeyType>& theFinger)
{
    localStream->device()->seek(0);
    *localStream >> theFinger.minKey;

    localStream->device()->seek((theFinger.itemsCount - 1) * oneFingerSize + sizeof(KeyType));
    *localStream >> theFinger.maxKey;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateBackptrOffsets( quint64 myChunkOffset, int posToInsert, int itemsCount, fingersLevelType myLocalLevel)
{
    quint64 nextLevelOffset;

    // EG_LOG_STUB << "posToInsert = " << posToInsert << ", myLocalLevel = " << myLocalLevel << ", itemsCount = " << itemsCount << FN;

        // update parent backlinks for shifted fingers
    for (int i = posToInsert; i < itemsCount; i++)
    {
            // get next level offset
        localStream->device()-> seek(i * oneFingerSize + sizeof(KeyType)*2 + sizeof(keysCountType));
        *localStream >> nextLevelOffset;

            // EG_LOG_STUB << "i = " << i << ", nextLevelOffset = " << hex << (int) nextLevelOffset
            //         << ", myChunkOffset = " << hex << (int) myChunkOffset << FN;

            // write actual backlink
        if (myLocalLevel > 0)
            StoreParentOffset(nextLevelOffset, myChunkOffset + i * oneFingerSize);
        else
            indexChunks-> StoreFingerOffset(nextLevelOffset, myChunkOffset + i * oneFingerSize);
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::SplitFingersChunk()
{
    quint64 parentOffset;
/*
    PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    PrintFingerInfo(newFinger, "newFinger " + FNS);
*/
        // init new chunk
    char new_chunk[fingersChunkSize];
    memset(&new_chunk, 0, fingersChunkSize);

        // get new chunk offset
    quint64 newChunkOffset = fingerStream.device()->size();

        // store current chunk backptr
    localStream->device()-> seek(egChunkVolume * oneFingerSize);
    *localStream >> parentOffset;

        // insert new finger next to current
    posToInsert = currentFinger.myOffsetInChunk/oneFingerSize + 1;

    // EG_LOG_STUB << "posToInsert = " << posToInsert << ", fingersCount = " << parentFinger.itemsCount << FN;

    if (posToInsert < parentFinger.itemsCount) // not last
        MoveTailToInsert(fingersBA.data(), posToInsert, parentFinger.itemsCount-posToInsert);

        // write updated current and new finger
    localStream->device()-> seek(currentFinger.myOffsetInChunk);
    WriteFinger(*localStream, currentFinger);
    WriteFinger(*localStream, newFinger);

        // move second part to new chunk
    memmove(&new_chunk, fingersBA.data() + (egChunkVolume/2+1)*oneFingerSize, egChunkVolume/2*oneFingerSize);
    memset(fingersBA.data() + (egChunkVolume/2+1)*oneFingerSize, 0, egChunkVolume/2*oneFingerSize);

        // restore parent back ptr & chunk level
    localStream->device()-> seek(egChunkVolume * oneFingerSize);
    *localStream << parentOffset;
    *localStream << currentFinger.myLevel;

    StoreFingersChunk(parentFinger.nextChunkOffset, fingersBA.data());

        // update parent backlinks for shifted fingers
    if (posToInsert < egChunkVolume/2+1)
        UpdateBackptrOffsets(parentFinger.nextChunkOffset, posToInsert, egChunkVolume/2+1, currentFinger.myLevel); // FIXME check

    // PrintFingersChunk(fingersBA.data(), "old chunk after split up " + FNS);

    currentFinger.itemsCount = egChunkVolume/2+1; // level up
    UpdateMinMax(currentFinger);

    memcpy(fingersBA.data(), &new_chunk, fingersChunkSize);

        // store chunk level
    UpdateChunkLevel(currentFinger.myLevel);

    StoreFingersChunk(newChunkOffset, fingersBA.data());

    // PrintFingersChunk(fingersBA.data(), "new chunk after split up " + FNS);

        // update parent backlinks for shifted fingers
    UpdateBackptrOffsets(newChunkOffset, 0, egChunkVolume/2, currentFinger.myLevel);

        // update parent fingers, go level up
    newFinger.itemsCount = egChunkVolume/2;
    UpdateMinMax(newFinger);

    currentFinger.nextChunkOffset = parentFinger.nextChunkOffset;
    currentFinger.myOffsetInChunk = parentFinger.myOffsetInChunk;
    currentFinger.myChunkOffset = parentFinger.myChunkOffset;

    currentFinger.myLevel = parentFinger.myLevel;

    fingerStream.device()->seek(parentFinger.myChunkOffset + parentFinger.myOffsetInChunk);
    WriteFinger(fingerStream, currentFinger);

    newFinger.myLevel = parentFinger.myLevel;
    newFinger.nextChunkOffset = newChunkOffset;
/*
    PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    PrintFingerInfo(newFinger, "newFinger " + FNS);
*/
    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::AppendFingersChunk()
{
        // init new chunk
    memset(fingersBA.data(), 0, fingersChunkSize);

        // write new finger and level
    localStream->device()-> seek(0);
    WriteFinger(*localStream, newFinger);

    UpdateChunkLevel(currentFinger.myLevel);

        // set parent back link to new finger at fingerStream.device()->size()
    if (currentFinger.myLevel > 0)
        StoreParentOffset(newFinger.nextChunkOffset, fingerStream.device()->size()); // FIXME check
    else // update index chunk backlink
        indexChunks-> StoreFingerOffset(newFinger.nextChunkOffset, fingerStream.device()->size());

        // level up
    newFinger.nextChunkOffset = fingerStream.device()->size();
    newFinger.itemsCount = 1;
    newFinger.myLevel = parentFinger.myLevel;
    // newFingerChunkOffset = fingerStream.device()->size();            // current level

    currentFinger = parentFinger;   // level up

        // save new chunk, fingers already updated to upper level
    StoreFingersChunk(newFinger.nextChunkOffset, fingersBA.data());

    return 0;
}

template <typename KeyType> bool EgFingers<KeyType>::checkFingersTreeIntegrity()
{   
    LoadRootFinger();

        // check root finger, it has to have at least one index
    if (! rootFinger.itemsCount || ! rootFinger.nextChunkOffset)
    {
        EG_LOG_STUB << "Possible fingers structure corrupted: zero count or data offset " << FN;
        return false;
    }

    if (rootFinger.myLevel == 0) // root only
        return true;

    parentFinger = rootFinger;
    fingersChain.clear();

        // use queue (QList) to walk tree and fingers counter to check loops (?)
    do
    {
        LoadFingersChunk(); // to fingersBA.data() from parentFinger.nextChunkOffset
        GetChunkLevel();    // to currentFinger.myLevel

            // check parent offset
        localStream-> device()->seek(egChunkVolume * oneFingerSize);
        *localStream >> parentFingerOffset;

        egFinger<KeyType> testFinger;
        LoadFingerDirect(testFinger, parentFingerOffset);

        if (testFinger.nextChunkOffset != parentFinger.nextChunkOffset)
        {
            EG_LOG_STUB << "Possible fingers structure corrupted: bad parent finger backlink " << FN;
            return false;
        }

            // check chunk content
        for (int i=0; i < parentFinger.itemsCount; i++)
        {
                // check next offset
            ReadFinger(currentFinger, i);

            if (! currentFinger.itemsCount || ! currentFinger.nextChunkOffset)
            {
                EG_LOG_STUB << "Possible fingers structure corrupted: zero count or data offset " << FN;
                return false;
            }

            if ( currentFinger.myLevel > 0 )
                fingersChain.append(currentFinger);
        }

        if ( ! fingersChain.isEmpty() )
        {
            parentFinger = fingersChain.first();
            fingersChain.removeFirst();
        }

        // EG_LOG_STUB << "fingersChain.count() = " << fingersChain.count() << FN;
    }
    while (! fingersChain.isEmpty()); // ! fingersChain.isEmpty()

    return true; // ok
}

/*
EgFingers<qint32> EgFingersqint32;
EgFingers<quint32> EgFingersquint32;
EgFingers<qint64>  EgFingersqint64;
EgFingers<quint64> EgFingersquint64;
EgFingers<float> EgFingersfloat;
EgFingers<double> EgFingersdouble;
*/
