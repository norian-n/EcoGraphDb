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
    qDebug() << "indexes: " << IndexFileName << theMessage;
    qDebug() << "fingerInfo.minKey = " << hex << (int) fingerInfo.minKey;
    qDebug() << "fingerInfo.maxKey = " << hex << (int) fingerInfo.maxKey;
    qDebug() << "fingerInfo.myLevel = " << hex << (int) fingerInfo.myLevel;
    qDebug() << "fingerInfo.itemsCount = " << hex << (int) fingerInfo.itemsCount;

    qDebug() << "fingerInfo.nextChunkOffset = " << hex << (int) fingerInfo.nextChunkOffset;
    qDebug() << "fingerInfo.myOffset = " << hex << (int) fingerInfo.myOffsetInChunk;
    qDebug() << "fingerInfo.myChunkOffset = " << hex << (int) fingerInfo.myChunkOffset;

    // qDebug() << "fingerInfo.fingersCount = " << hex << (int) fingerInfo.fingersCount;
    // qDebug() << "fingerInfo.myLevel = " << hex << (int) fingerInfo.myLevel << endl;
}

template <typename KeyType> void EgFingers<KeyType>::PrintFingersChunk(char* theFingersChunk, const QString &theMessage)
{
    qDebug() << QByteArray(theFingersChunk, fingersChunkSize).toHex() << theMessage;
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

    qDebug() << "myChunkOffset = " << hex << (int) fingersChunkOffset
             << ", parentNextOffset = " << hex << (int) parentNextOffset;

    qDebug() << "Parent: minKey = " << hex << (int) minParent
             << ", maxKey = " << hex << (int) maxParent
             << ", itemsCount = " << (int) parentKeysCount;

    qDebug() << "myLevel = " << (int) myLevel;

    qDebug() << "parentFingerOffset = " << hex << (int) parentFingerOffset << endl;

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

    // qDebug() << "nextOffset = " << hex << (int) nextOffset << FN;

    chunksList.append(nextOffset);

        // add fingers if level > 0
    if (myLevel > 0)
        for (int i = 0; i < chunkKeysCount; i++)
        {
            fingerStream.device()->seek(nextOffset + i * oneFingerSize + sizeof(KeyType)*2 + sizeof(keysCountType));
            fingerStream >> branchOffset;

            // qDebug() << "nextOffset = " << hex << (int) nextOffset << "branchOffset = " << hex << (int) branchOffset << FN;

            branchesList.append(branchOffset);
        }

        // process branches
    while (! branchesList.isEmpty())
    {
        nextOffset = branchesList.first();
        branchesList.pop_front();

        // qDebug() << "nextOffset = " << hex << (int) nextOffset << FN;

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

                // qDebug() << "nextOffset = " << hex << (int) nextOffset << "branchOffset = " << hex << (int) branchOffset << FN;

                branchesList.append(branchOffset);
            }
        }
    }

        // print list
    qDebug() << theMessage;

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

    // rootFinger.myChunkOffset = 0;
    // rootFinger.myOffset = 0;
}


template <typename KeyType> void EgFingers<KeyType>::LoadRootFinger()
{
    // qDebug() << "fingerFile.fileName() = " << fingerFile.fileName() << FN;
    // qDebug() << "fingerFile size = " << fingerStream.device()->size() << FN;

    fingerStream.device()-> seek(0);

    fingerStream >> rootFinger.minKey;
    fingerStream >> rootFinger.maxKey;

    fingerStream >> rootFinger.itemsCount;
    fingerStream >> rootFinger.nextChunkOffset;

    fingerStream >> rootFinger.myLevel;

    rootFinger.myChunkOffset = 0;
    rootFinger.myOffsetInChunk = 0;

    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);
}

template <typename KeyType> void EgFingers<KeyType>::StoreRootFingerMinMax()
{
    fingerStream.device()->seek(0);

    fingerStream << rootFinger.minKey;
    fingerStream << rootFinger.maxKey;
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
    fingerStream.device()->seek(fingersChunkOffset);
    // fingerStream.writeRawData(chunkPtr, fingersChunkSize);

        // int res =
    fingerStream.device()->write(chunkPtr, fingersChunkSize);

    // qDebug() << "fingersChunkOffset" << hex << (int) fingersChunkOffset << FN;

    return 0; // FIXME
}

template <typename KeyType> int EgFingers<KeyType>::StoreParentOffset(quint64 fingersChunkOffset, quint64 parentFingerOffset)
{
    fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
    fingerStream << parentFingerOffset;

    // qDebug() << "fingersChunkOffset" << hex << (int) fingersChunkOffset << ", parentFingerOffset" << hex << (int) parentFingerOffset << FN;

    return 0; // FIXME
}

template <typename KeyType> int EgFingers<KeyType>::GetParentOffset(QDataStream &localFingersStream, quint64& parentFingerOffset)
{
    localFingersStream.device()->seek(egChunkVolume * oneFingerSize);
    localFingersStream >> parentFingerOffset;

    // qDebug() << "fingerOffset = " << hex << (int) fingerOffset << FN;

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::LoadFingersChunk(quint64 fingersChunkOffset)
{
    fingerStream.device()->seek(fingersChunkOffset);
    fingerStream.readRawData(fingersChunk, fingersChunkSize);

    return 0; // FIXME process load returns
}

template <typename KeyType> int EgFingers<KeyType>::LoadFingersChunk(char* chunkPtr, const quint64 fingersChunkOffset)
{
    fingerStream.device()->seek(fingersChunkOffset);
    fingerStream.readRawData(chunkPtr, fingersChunkSize);

    return 0; // FIXME process load returns
}

template <typename KeyType> inline void EgFingers<KeyType>::ReadFinger(QDataStream &localFingersStream, egFinger<KeyType>& theFinger)
{
    localFingersStream.device()->seek(theFinger.myOffsetInChunk);

    localFingersStream >> theFinger.minKey;
    localFingersStream >> theFinger.maxKey;
    localFingersStream >> theFinger.itemsCount;
    localFingersStream >> theFinger.nextChunkOffset;
}


template <typename KeyType> int EgFingers<KeyType>::FindNextLevelOffsetFirst(QDataStream &localFingersStream, bool isExactEqual)
{
    int fingerPosition;
    egFinger<KeyType> secondFinger;

    if (parentFinger.itemsCount == 0)  // no fingers
        return -2;

    if ((indexChunks-> theKey < parentFinger.minKey) || (parentFinger.itemsCount == 1)) // out of range or only one
    {
        currentFinger.myOffsetInChunk = 0;
        ReadFinger(localFingersStream, currentFinger);

        if (indexChunks-> theKey < parentFinger.minKey) // out of range
            return 1;

        return 0;
    }

    if (indexChunks-> theKey > parentFinger.maxKey) // out of range or only one
    {
        currentFinger.myOffsetInChunk = (parentFinger.itemsCount - 1);
        ReadFinger(localFingersStream, currentFinger);

        return 1;
    }

    // FIXME - write tests

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        // proportional finger lookup

    if (parentFinger.maxKey > parentFinger.minKey)
        fingerPosition = (indexChunks-> theKey - parentFinger.minKey)*(parentFinger.itemsCount - 1)/(parentFinger.maxKey - parentFinger.minKey);
    else // parentFinger.maxKey == parentFinger.minKey
    {
        currentFinger.myOffsetInChunk = 0;
        ReadFinger(localFingersStream, currentFinger);

        return 0;
    }

    // qDebug() << "indexChunks-> theKey = " << indexChunks-> theKey << "fingerPosition = " << fingerPosition << FN;

    currentFinger.myOffsetInChunk = fingerPosition*oneFingerSize;
    ReadFinger(localFingersStream, currentFinger);

    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    secondFinger.myLevel = currentFinger.myLevel;

    if (isExactEqual)
    do
    {
        if (indexChunks-> theKey > currentFinger.maxKey)
        {
            if (fingerPosition < (parentFinger.itemsCount - 1))
            {
                fingerPosition ++;

                // qDebug() << "fingerPosition = " << fingerPosition << FN;

                currentFinger.myOffsetInChunk = fingerPosition*oneFingerSize;
                ReadFinger(localFingersStream, currentFinger);
            }
        }
        else
        {
            if (indexChunks-> theKey > currentFinger.minKey)
            {
                // PrintFingerInfo(currentFinger, "currentFinger (equal) found " + FNS);
                return 0;
            }
            else
            {
                if (fingerPosition > 0)
                {
                    secondFinger.myOffsetInChunk = (fingerPosition-1)*oneFingerSize;
                    ReadFinger(localFingersStream, secondFinger);

                    if (indexChunks-> theKey > secondFinger.maxKey) // finger found
                    {
                        if (indexChunks-> theKey == currentFinger.minKey)
                            return 0;

                        return 1;   // between fingers
                    }
                    else
                    {
                        fingerPosition --;

                        currentFinger = secondFinger;
                    }
                }
            }
        }
    }
    while ((fingerPosition > 0) && (fingerPosition < (parentFinger.itemsCount - 1)) /*&& (emergencyCounter < (parentFinger.itemsCount + 1))*/);

    else    // ! isExactEqual

    do
    {
        if (indexChunks-> theKey >= currentFinger.maxKey)
        {
            if (fingerPosition < (parentFinger.itemsCount - 1))
            {
                fingerPosition ++;

                currentFinger.myOffsetInChunk = fingerPosition*oneFingerSize;
                ReadFinger(localFingersStream, currentFinger);
            }
        }
        else
        {
            if (indexChunks-> theKey >= currentFinger.minKey)
            {
                // PrintFingerInfo(currentFinger, "currentFinger (not equal) found " + FNS);
                return 0;
            }
            else
            {
                if (fingerPosition > 0)
                {
                    secondFinger.myOffsetInChunk = (fingerPosition-1)*oneFingerSize;
                    ReadFinger(localFingersStream, secondFinger);

                    if (indexChunks-> theKey >= secondFinger.maxKey) // finger found
                    {
                        return 0;
                    }
                    else
                    {
                        fingerPosition --;
                        currentFinger = secondFinger;
                    }
                }
            }
        }
    }
    while ((fingerPosition > 0) && (fingerPosition < (parentFinger.itemsCount - 1)) /*&& (emergencyCounter < (parentFinger.itemsCount + 1))*/);

    currentFinger.myOffsetInChunk = fingerPosition*oneFingerSize;
    ReadFinger(localFingersStream, currentFinger);

    return 0;
}


template <typename KeyType> inline int EgFingers<KeyType>::FindFingerInChunkToInsert(QDataStream &localFingersStream)
{
    if (parentFinger.itemsCount == 0)  // no fingers, corrupted structure FIXME process
    {
        PrintFingerInfo(parentFinger, "Error: corrupted parentFinger, 0 items " + FNS);
        return -1;
    }

    int fingerPosition = parentFinger.itemsCount - 1; // last finger first

        // read last finger
    currentFinger.myOffsetInChunk = fingerPosition*oneFingerSize;
    ReadFinger(localFingersStream, currentFinger);

    fingerPosition--;

        // go left until appropriate finger
    while ((fingerPosition >= 0) && (indexChunks-> theKey < currentFinger.minKey))
    {
        currentFinger.myOffsetInChunk = fingerPosition*oneFingerSize;
        ReadFinger(localFingersStream, currentFinger);

        fingerPosition--;
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::FindNextLevelOffsetLast(QDataStream &localFingersStream, bool isExactEqual)
{
    int fingerPosition;
    egFinger<KeyType> secondFinger;

    if (parentFinger.itemsCount == 0)  // no fingers FIXME STUB
        return -2;

    if ((indexChunks-> theKey > parentFinger.maxKey) || (parentFinger.itemsCount == 1)) // out of range or only one
    {
        currentFinger.myOffsetInChunk = (parentFinger.itemsCount - 1)*oneFingerSize;
        ReadFinger(localFingersStream, currentFinger);

        if (indexChunks-> theKey > parentFinger.maxKey) // out of range
            return 1;

        return 0;
    }

    if (indexChunks-> theKey < parentFinger.minKey) // out of range or only one
    {
        currentFinger.myOffsetInChunk = 0;
        ReadFinger(localFingersStream, currentFinger);

        return 1;
    }

    // FIXME - write tests

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);


        // proportional finger lookup

    if (parentFinger.maxKey > parentFinger.minKey)
        fingerPosition = (indexChunks-> theKey - parentFinger.minKey)*(parentFinger.itemsCount - 1)/(parentFinger.maxKey - parentFinger.minKey);
    else // parentFinger.maxKey == parentFinger.minKey
    {
        currentFinger.myOffsetInChunk = (parentFinger.itemsCount - 1)*oneFingerSize;
        ReadFinger(localFingersStream, currentFinger);

        return 0;
    }

    // qDebug() << "fingerPosition = " << fingerPosition << FN;

    currentFinger.myOffsetInChunk = fingerPosition*oneFingerSize;
    ReadFinger(localFingersStream, currentFinger);

    secondFinger.myLevel = currentFinger.myLevel;

    if (isExactEqual)
    do
    {
        if (indexChunks-> theKey < currentFinger.minKey)
        {
            if (fingerPosition > 0)
            {
                fingerPosition --;

                currentFinger.myOffsetInChunk = fingerPosition*oneFingerSize;
                ReadFinger(localFingersStream, currentFinger);
            }
        }
        else
        {
            if (indexChunks-> theKey < currentFinger.maxKey)
            {
                // PrintFingerInfo(currentFinger, "currentFinger (equal) found " + FNS);
                return 0;
            }
            else
            {
                if (fingerPosition < (parentFinger.itemsCount - 1))
                {
                    secondFinger.myOffsetInChunk = (fingerPosition+1)*oneFingerSize;
                    ReadFinger(localFingersStream, secondFinger);

                    if (indexChunks-> theKey < secondFinger.minKey) // finger found
                    {
                        if (indexChunks-> theKey == currentFinger.maxKey)
                            return 0;

                        return 1;   // between fingers
                    }
                    else
                    {
                        fingerPosition ++;

                        currentFinger = secondFinger;
                    }
                }
            }
        }
    }
    while ((fingerPosition > 0) && (fingerPosition < (parentFinger.itemsCount - 1)) /*&& (emergencyCounter < (parentFinger.itemsCount + 1))*/);

    else    // ! isExactEqual

    do
    {
        if (indexChunks-> theKey <= currentFinger.minKey)
        {
            if (fingerPosition > 0)
            {
                fingerPosition --;

                currentFinger.myOffsetInChunk = fingerPosition*oneFingerSize;
                ReadFinger(localFingersStream, currentFinger);
            }
        }
        else
        {
            if (indexChunks-> theKey <= currentFinger.maxKey)
            {
                // PrintFingerInfo(currentFinger, "currentFinger (not equal) found " + FNS);
                return 0;
            }
            else
            {
                if (fingerPosition < (parentFinger.itemsCount - 1))
                {
                    secondFinger.myOffsetInChunk = (fingerPosition+1)*oneFingerSize;
                    ReadFinger(localFingersStream, secondFinger);

                    if (indexChunks-> theKey <= secondFinger.minKey) // finger found
                    {
                        return 0;
                    }
                    else
                    {
                        fingerPosition ++;
                        currentFinger = secondFinger;
                    }
                }
            }
        }
    }
    while ((fingerPosition > 0) && (fingerPosition < (parentFinger.itemsCount - 1)) /*&& (emergencyCounter < (parentFinger.itemsCount + 1))*/);

    currentFinger.myOffsetInChunk = fingerPosition*oneFingerSize;
    ReadFinger(localFingersStream, currentFinger);

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkToInsert()
{
    int res = 0;

    fingersChain.clear();

    LoadRootFinger();

    if (rootFinger.myLevel == 0) // root only
    {
        currentFinger = rootFinger;

        return 0;
    }

    // FIXME TODO check global min/max and use first/last index chunk directly

    parentFinger = rootFinger;

    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);

        // fill fingers chain
    do
    {
        LoadFingersChunk(fingersBA.data(), parentFinger.nextChunkOffset);

            // get level
        localStream->device()-> seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        *localStream >> currentFinger.myLevel;

        // qDebug() << "myLevel = " << currentFinger.myLevel << FN;

        res = FindFingerInChunkToInsert(*localStream);

        currentFinger.myChunkOffset = parentFinger.nextChunkOffset; // shortcut (???)

        fingersChain.append(parentFinger);

        if ( currentFinger.myLevel > 0 )
            parentFinger = currentFinger;
    }
    while ((currentFinger.myLevel > 0) && (res >= 0));

    // qDebug() << "theKey = " << hex << (int) indexChunks-> theKey << FN;

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    return res;
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkFirst(bool isExactEqual)
{
    // PrintFingerInfo(rootFinger, "rootFinger " + FNS);

    LoadRootFinger();

    if (rootFinger.myLevel == 0) // only root finger exists
    {
        if (isExactEqual && (indexChunks-> theKey >= rootFinger.minKey) && (indexChunks-> theKey <= rootFinger.maxKey))
        {
            currentFinger = rootFinger;

            return 0;
        }

        if (!isExactEqual && (indexChunks-> theKey > rootFinger.minKey) && (indexChunks-> theKey < rootFinger.maxKey))
        {
            currentFinger = rootFinger;

            return 0;
        }

        return 1; // out of range
    }

    int res = 0;

        // check root borders
    // if (indexChunks-> theKey > fingersRootHeader.maxKey) // no first chunk found
    //    return -1;

    // int myLevel  = fingersRootHeader.myLevel;
    parentFinger = rootFinger;

        // go fingers chain
    do
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

        LoadFingersChunk(fingersBA.data(), parentFinger.nextChunkOffset);

            // get level
        localStream->device()-> seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        *localStream >> currentFinger.myLevel;

        // qDebug() << "myLevel = " << currentFinger.myLevel << FN;

        res = FindNextLevelOffsetFirst(*localStream, isExactEqual);  // get currentFinger

        currentFinger.myChunkOffset = parentFinger.nextChunkOffset; // shortcut (???)

        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        if (currentFinger.myLevel > 0)
            parentFinger = currentFinger;
        // myLevel--;
    }
    while ((currentFinger.myLevel > 0) && (res >= 0)); // FIXME

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    return res;
}


template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkLast(bool isExactEqual)
{
    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);

    LoadRootFinger();

    if (rootFinger.myLevel == 0) // only root finger exists
    {
        if (isExactEqual && (indexChunks-> theKey >= rootFinger.minKey) && (indexChunks-> theKey <= rootFinger.maxKey))
        {
            currentFinger = rootFinger;

            return 0;
        }

        if (!isExactEqual && (indexChunks-> theKey > rootFinger.minKey) && (indexChunks-> theKey < rootFinger.maxKey))
        {
            currentFinger = rootFinger;

            return 0;
        }

        return 1; // out of range
    }

    int res = 0;

        // check root borders
    // if (indexChunks-> theKey < fingersRootHeader.minKey) // no first chunk found
    //    return -1;

    // int myLevel  = fingersRootHeader.myLevel;
    parentFinger = rootFinger;

        // fill fingers chain
    do
    {
        LoadFingersChunk(fingersBA.data(), parentFinger.nextChunkOffset);        

            // get level
        localStream->device()-> seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        *localStream >> currentFinger.myLevel;

        res = FindNextLevelOffsetLast(*localStream, isExactEqual);  // get currentFinger

        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        currentFinger.myChunkOffset = parentFinger.nextChunkOffset; // shortcut (???)


        if (currentFinger.myLevel > 0)
            parentFinger = currentFinger;
        // myLevel--;
    }
    while ((currentFinger.myLevel > 0) && (res >= 0)); // FIXME

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    return res;
}


template <typename KeyType> int EgFingers<KeyType>::GetFingerByOffset()
{
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS );

    currentFinger.myOffsetInChunk = (updatedFingerOffset-rootHeaderSize) % fingersChunkSize;
    currentFinger.myChunkOffset = updatedFingerOffset - currentFinger.myOffsetInChunk;

    return 0;
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

    if (rootFinger.myLevel == 0)
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
        StoreRootFingerMinMax();
    }

    return 0;
}

/*
template <typename KeyType> int EgFingers<KeyType>::UpdateFingerAfterDelete()
{

    // prerequisites:

    KeyType oldValue;
    keysCountType fingersCount;

    // qDebug() << "Finger count offset = " << hex << (int) (currentFingerOffset + sizeof(KeyType) * 2)
    //         << "newKeysCount = " << currentKeysCount << FN;

        // update keys count, decremented by indexes
    fingerStream.device()->seek(currentFingerOffset + sizeof(KeyType) * 2);
    fingerStream << currentKeysCount;

    // get chunk offset
// quint64 fingersChunkOffset = currentFingerOffset - ( (currentFingerOffset-rootHeaderSize) % fingersChunkSize );

    if (minValueChanged)
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

            if (currentFingerOffset == fingersChunkOffset) // first finger
                UpdateMinValueUp();
        }
    }
    else if (maxValueChanged)
    {
        // get old value
        fingerStream.device()->seek(currentFingerOffset + sizeof(KeyType));
        fingerStream >> oldValue;

        if (newMinValue != oldValue) // changed
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
                fingerStream >> fingersCount;
            }
            else
                fingersCount = fingersRootHeader.itemsCount;

            if (fingerNum == (fingersCount - 1)) // last one
                UpdateMaxValueUp();
        }
    }

        // check
    // fingerStream.device()->waitForBytesWritten(-1);

    // fingerStream.device()->seek(currentFingerOffset + sizeof(KeyType) * 2);
    // fingerStream >> myFingersCount;

    // qDebug() << "fingersCount 2 = " << myFingersCount << FN;
    */
/*
    if (maxValueChanged)
    {
        fingerStream.device()->seek(currentFingerOffset + sizeof(KeyType));
        fingerStream << newMaxValue;

        // qDebug() << "newMaxValue = " << (int) newMaxValue << FN;
    }
    else if (minValueChanged)
    {
        fingerStream.device()->seek(currentFingerOffset);
        fingerStream << newMinValue;

        // qDebug() << "newMinValue = " << (int) newMinValue << FN;
    }

    if (maxValueChanged || minValueChanged)
    {
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
            fingersCount = fingersRootHeader.itemsCount;

        isFirstFinger = (fingerNum == 0);
        isLastFinger  = (fingerNum == fingersCount-1);

        if ((isFirstFinger && minValueChanged) || (isLastFinger && maxValueChanged))
            UpdateFingersChainAfterDelete();
    }
*/
        // reset
    // maxValueChanged = false;
    // minValueChanged = false;

        // check
/*
    fingerStream.device()->seek(currentFingerOffset + sizeof(KeyType) * 2 );
    fingerStream >> fingersCount;

    qDebug() << "newKeysCount 2 = " << (int) fingersCount << FN;
    */

/*
    return 0;
}
*/


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

            StoreRootFingerMinMax();
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

            // qDebug() << "newMaxValue = " << newMaxValue << FN;
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

            // qDebug() << "newMinValue = " << newMinValue << FN;
        }
        else
        {
            maxValueChanged = false;
            minValueChanged = false;
            break;
        }
    }   // while

        // qDebug() << "parentFingerOffset = " << parentFingerOffset << FN;

        // check / update root header
    if(! parentFingerOffset)
    {
        if (maxValueChanged || minValueChanged)
        {
            if (maxValueChanged)
                rootFinger.maxKey =  newMaxValue;
            else if (minValueChanged)
                rootFinger.minKey =  newMinValue;

            StoreRootFingerMinMax();
        }
    }

        // reset
    maxValueChanged = false;
    minValueChanged = false;

    return 0;
}

template <typename KeyType> void EgFingers<KeyType>::DeleteSpecificFinger()
{
    bool isFirstFinger;
    bool isLastFinger;

    KeyType oldValue;

    fingersLevelType myLocalLevel;

        // calc this chunk offset and finger position
    quint64 fingersChunkOffset = currentFingerOffset - ((currentFingerOffset-rootHeaderSize) % fingersChunkSize);
    keysCountType fingerNum = (currentFingerOffset - fingersChunkOffset) / oneFingerSize;

    isFirstFinger = (fingerNum == 0);
    isLastFinger  = (fingerNum == currentKeysCount-1);

    // qDebug() << "currentKeysCount = " << currentKeysCount << ", fingerNum = " << fingerNum << FN;

    LoadFingersChunk(fingersChunk, fingersChunkOffset);

        // update min/max
    if (isLastFinger)
    {
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        localStream->device()-> seek(fingerNum * oneFingerSize + sizeof(KeyType));
        *localStream >> oldValue;

        localStream->device()-> seek((fingerNum - 1) * oneFingerSize + sizeof(KeyType));
        *localStream >> newMaxValue;

        if (newMaxValue < oldValue)
        {
            maxValueChanged = true;
        }
    }
    else if (isFirstFinger)
    {
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        localStream->device()-> seek(0);
        *localStream >> oldValue;

        localStream->device()-> seek(oneFingerSize);
        *localStream >> newMinValue;

        if (newMinValue > oldValue)
        {
            minValueChanged = true;
        }
    }

    // PrintFingersChunk(fingersChunk, "fingers chunk before delete " + FNS);

        // move tail if required
    if (! isLastFinger)
    {
        memmove (fingersChunk + fingerNum*oneFingerSize, fingersChunk + (fingerNum+1)*oneFingerSize,  oneFingerSize*(currentKeysCount - fingerNum));

        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        localStream->device()-> seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        *localStream >> myLocalLevel;

        // qDebug() << "myLocalLevel = " << myLocalLevel << FN;

            // update parent links
        UpdateParentsOffsets(*localStream, fingersChunk, fingersChunkOffset,  fingerNum, currentKeysCount-1, myLocalLevel);
    }
        // write zeroes to last and decrease count
    memmove (fingersChunk + oneFingerSize*(currentKeysCount - 1), zeroFingersChunk,  oneFingerSize);

    // PrintFingersChunk(fingersChunk, "fingers chunk after delete " + FNS);

    // PrintAllChunksInfo("After finger delete" + FNS);

    StoreFingersChunk(fingersChunkOffset, fingersChunk);    
}

template <typename KeyType> void EgFingers<KeyType>::DeleteFingersChunk(quint64 fingersChunkOffset)
{
    StoreFingersChunk(fingersChunkOffset, zeroFingersChunk);
}


template <typename KeyType> int EgFingers<KeyType>::DeleteParentFinger() // recursive
{
    if (rootFinger.myLevel == 0) // root only
    {
        return 1; // delete files
    }

        // iterate tree
    while (currentFingerOffset)
    {
            // calc this chunk offset
        fingersChunkOffset = currentFingerOffset - ((currentFingerOffset-rootHeaderSize) % fingersChunkSize);

            // get parent
        fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
        fingerStream >> parentFingerOffset;

            // check fingers count
        if (parentFingerOffset) // parent not root
        {
            fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType)*2);
            fingerStream >> currentKeysCount;
        }
        else
            currentKeysCount = rootFinger.itemsCount;

        /*
        qDebug() << "currentFingerOffset " << hex << (int) currentFingerOffset
                 << " , parentFingerOffset " << hex << (int) parentFingerOffset
                 << " , currentKeysCount = " << currentKeysCount << FN;
        */

        if (currentKeysCount > 1) // not Only One Finger
        {
            minValueChanged = false;
            maxValueChanged = false;

            DeleteSpecificFinger(); // only one by currentFingerOffset and currentKeysCount

                // decrease parent items count
            if (parentFingerOffset) // parent not root
            {
                currentKeysCount--;

                /*
                qDebug() << "currentFingerOffset " << hex << (int) currentFingerOffset
                         << " , parentFingerOffset " << hex << (int) parentFingerOffset
                         << " , currentKeysCount = " << currentKeysCount << FN;
                */

                fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType) * 2 );
                fingerStream << currentKeysCount;

                currentFingerOffset = parentFingerOffset;

                if (minValueChanged)
                    UpdateMinValueUp();
                else if (maxValueChanged)
                    UpdateMaxValueUp();
            }
            else
            {
                rootFinger.itemsCount--;

                if (minValueChanged)
                    rootFinger.minKey = newMinValue;
                else if (maxValueChanged)
                    rootFinger.maxKey = newMaxValue;

                StoreRootFinger();
            }

            return 0;
        }
        else if (currentKeysCount == 1)   // last finger
        {
            DeleteFingersChunk(fingersChunkOffset); // just write zeroes
        }

        currentFingerOffset = parentFingerOffset;   // go up
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateFingersChainAfterSplit()
{
    bool splitParentChunk = false;
    bool appendMode = true;

    if (rootFinger.myLevel == 0) // no fingers chunks yet
    {
            // init chunk
        memset(fingersBA.data(), 0, fingersChunkSize);

            // put old and new fingers to the chunk
        WriteFinger(*localStream, currentFinger);     // update current finger
        WriteFinger(*localStream, newFinger);         // add new finger

        StoreFingersChunk(fingerStream.device()->size(), fingersBA.data());

            // update root finger
        rootFinger.minKey = currentFinger.minKey;
        rootFinger.maxKey = newFinger.maxKey;

        rootFinger.myLevel = 1;
        rootFinger.itemsCount = 2;

        rootFinger.nextChunkOffset = rootHeaderSize;

        StoreRootFinger();

            // backptrs to first 2 fingers in chunk
        indexChunks-> StoreFingerOffset(currentFinger.nextChunkOffset, rootHeaderSize);             // FIXME check
        indexChunks-> StoreFingerOffset(newFinger.nextChunkOffset, rootHeaderSize + oneFingerSize); // FIXME check

        return 0;
    }

    while (! fingersChain.isEmpty())
    {

        parentFinger = fingersChain.last();
        fingersChain.removeLast();

        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
        // PrintFingerInfo(newFinger, "newFinger " + FNS);

        LoadFingersChunk(fingersChunk, parentFinger.nextChunkOffset);
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        if (parentFinger.itemsCount < egChunkVolume)
        {
            InsertSplittedFinger(*localStream);     // if have space just add new item for new chunk and recalc min/max
            splitParentChunk = false;

            break;
        }
        else
        {
            appendMode = (indexChunks-> theKey >= parentFinger.maxKey); // FIXME check

            if (appendMode)
                AppendFingersChunk(*localStream);
            else
                SplitFingersChunk2(*localStream);

            splitParentChunk = true;
        }
    }

    if (fingersChain.isEmpty() && splitParentChunk) // new root chunk is required
    {
        // qDebug() << "fingersChain.count() = " << fingersChain.count() << FN;
/*
        PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        PrintFingerInfo(currentFinger, "currentFinger " + FNS);
        PrintFingerInfo(newFinger, "newFinger " + FNS);
        */

        if (appendMode)
            currentFinger = rootFinger;
            // shift up
        rootFinger.nextChunkOffset = fingerStream.device()->size(); // up
        AddNewRootChunk(*localStream);

        StoreParentOffset(currentFinger.nextChunkOffset, rootFinger.nextChunkOffset); // FIXME CHECK
        StoreParentOffset(newFinger.nextChunkOffset, rootFinger.nextChunkOffset + oneFingerSize);

        UpdateTheFingerMinMax(rootFinger);

        rootFinger.myLevel++;
        rootFinger.itemsCount = 2; // new top chunk

        StoreRootFinger();

        // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);
    }
    else if (fingersChain.isEmpty())
    {
        UpdateTheFingerMinMax(rootFinger);
        StoreRootFinger();
    }

    return 0;
}

template <typename KeyType> inline void EgFingers<KeyType>::UpdateTheFingerMinMax(egFinger<KeyType>& theFinger)
{
    if (indexChunks-> theKey > theFinger.maxKey)
        theFinger.maxKey =  indexChunks-> theKey;
    else if (indexChunks-> theKey < theFinger.minKey)
        theFinger.minKey =  indexChunks-> theKey;
}

template <typename KeyType> void EgFingers<KeyType>::AddNewRootChunk(QDataStream &localFingersStream)
{
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    // PrintFingerInfo(newFinger, "newFinger " + FNS);

    memcpy(fingersBA.data(), zeroFingersChunk, fingersChunkSize);

    localFingersStream.device()->seek(0);

    WriteFinger(localFingersStream, currentFinger);     // update current finger
    WriteFinger(localFingersStream, newFinger);         // add new finger

    localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
    localFingersStream << (fingersLevelType) (rootFinger.myLevel);

    // qDebug() << "currentFinger.myLevel = " << currentFinger.myLevel << FN;
    // qDebug() << "fingersChunkOffset = " << hex << (int) fingerStream.device()->size() << FN;
    // fingersChunkOffset = fingerStream.device()->size();

    StoreFingersChunk(fingerStream.device()->size(), fingersBA.data());
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
    memmove (chunkPtr + (fingerPosition+1)*oneFingerSize, chunkPtr + fingerPosition*oneFingerSize, fingersToMove*oneFingerSize);
    // PrintFingersChunk(chunk, "chunk after tail move " + FNS);
}


template <typename KeyType> int EgFingers<KeyType>::InsertSplittedFinger(QDataStream &localFingersStream)
{
    // PrintFingerInfo(newFinger, "newFinger " + FNS);

    posToInsert = currentFinger.myOffsetInChunk/oneFingerSize + 1;

    localFingersStream.device()->seek(currentFinger.myOffsetInChunk);

    WriteFinger(localFingersStream, currentFinger);         // update current

    memcpy(fingersChunk, fingersBA.constData(), fingersChunkSize);

    InsertNewFinger(localFingersStream, fingersChunk, posToInsert, parentFinger.itemsCount);

    // qDebug() << "backlink = " << hex << currentFinger.myChunkOffset + posToInsert*oneFingerSize << ", newFinger.nextChunkOffset = " << hex << newFinger.nextChunkOffset << FN;

        // update backlink
    if (currentFinger.myLevel == 0)
        indexChunks-> StoreFingerOffset(newFinger.nextChunkOffset, currentFinger.myChunkOffset + posToInsert*oneFingerSize);

    UpdateParentsOffsets(localFingersStream, fingersChunk, parentFinger.nextChunkOffset, posToInsert, parentFinger.itemsCount+1, currentFinger.myLevel);

    StoreFingersChunk(parentFinger.nextChunkOffset, fingersChunk);

    // PrintFingersChunk(fingersBA.data(), "fingers chunk after split/append " + FNS);

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

template <typename KeyType> int EgFingers<KeyType>::InsertNewFinger(QDataStream &localFingersStream, char* theChunk, int posToInsert, int itemsCount)
{
        // writes new finger to the chunk at position, old items count - before insert

        // check if not last position, move tail
    if (posToInsert < itemsCount)
    {
        // qDebug() << "posToInsert = " << posToInsert << ", fingersCount = " << itemsCount << FN;
        memmove (theChunk + (posToInsert+1)*oneFingerSize, theChunk + posToInsert*oneFingerSize,  oneFingerSize*(itemsCount - posToInsert));
        // PrintFingersChunk(theChunk, "fingers chunk after memmove " + FNS);
    }

    memcpy(fingersBA.data(), theChunk, fingersChunkSize);

    localFingersStream.device()->seek(posToInsert*oneFingerSize);

    WriteFinger(localFingersStream, newFinger);

    memcpy(theChunk, fingersBA.constData(), fingersChunkSize);

    return 0;
}

template <typename KeyType> void EgFingers<KeyType>::UpdateMinMax(QDataStream& localFingersStream, egFinger<KeyType>& theFinger, char* theChunk)
{
    memcpy(fingersBA.data(), theChunk, fingersChunkSize);

    localFingersStream.device()->seek(0);
    localFingersStream >> theFinger.minKey;

    localFingersStream.device()->seek((theFinger.itemsCount - 1) * oneFingerSize + sizeof(KeyType));
    localFingersStream >> theFinger.maxKey;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateParentsOffsets(QDataStream &localFingersStream, char* theChunk,
                                    quint64 myChunkOffset, int posToInsert, int itemsCount, fingersLevelType myLocalLevel)
{
    // set parent backlinks, real items count !! currentFinger.myLevel is required FIXME

    quint64 nextLevelOffset;

    // fingersLevelType myLocalLevel;

    memcpy(fingersBA.data(), theChunk, fingersChunkSize);

        // update parent backlinks - shifted fingers
    for (int i = posToInsert; i < itemsCount; i++)
    {
            // get next level offset
        localFingersStream.device()->seek(i * oneFingerSize + sizeof(KeyType)*2 + sizeof(keysCountType));
        localFingersStream >> nextLevelOffset;

            // qDebug() << "i = " << i << "myLocalLevel = " << myLocalLevel << ", nextLevelOffset = " << hex << (int) nextLevelOffset
            //          << ", myChunkOffset = " << hex << (int) myChunkOffset << FN;

            // write actual backlink
        if (myLocalLevel > 0)
            StoreParentOffset(nextLevelOffset, myChunkOffset + i * oneFingerSize);
        else
            indexChunks-> StoreFingerOffset(nextLevelOffset, myChunkOffset + i * oneFingerSize);
    }

    return 0;
}


template <typename KeyType> int EgFingers<KeyType>::SplitFingersChunk2(QDataStream &localFingersStream)
{
    // KeyType minKey, maxKey;

    // qDebug() << "initialPosToInsert = " << posToInsert << ", fingersCount = " << parentFinger.itemsCount << FN;

        // update current finger
    memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

    localFingersStream.device()->seek(currentFinger.myOffsetInChunk);
    WriteFinger(localFingersStream, currentFinger);

    memcpy(fingersChunk, fingersBA.constData(), fingersChunkSize);

        // insert new finger next to current
    posToInsert = currentFinger.myOffsetInChunk/oneFingerSize + 1;

        // init new chunk
    memmove (newFingersChunk, zeroFingersChunk,  fingersChunkSize);

        // split chunks 50:50
    memmove (newFingersChunk, fingersChunk + egChunkVolume*oneFingerSize/2,  egChunkVolume*oneFingerSize/2);
    // PrintFingersChunk(newFingersChunk, "new chunk after split up " + FNS);

        // add zeroes to first chunk tail
    memmove (fingersChunk + egChunkVolume*oneFingerSize/2, zeroFingersChunk,  egChunkVolume*oneFingerSize/2);
    // PrintFingersChunk(fingersChunk, "old chunk after split up " + FNS);

    // check what chunk to insert
    if ( posToInsert < (egChunkVolume/2+1)) // insert to first part
    {
        InsertNewFinger(localFingersStream, fingersChunk, posToInsert, egChunkVolume/2);
        UpdateParentsOffsets(localFingersStream, fingersChunk, parentFinger.nextChunkOffset, posToInsert, egChunkVolume/2+1, currentFinger.myLevel);
        UpdateParentsOffsets(localFingersStream, newFingersChunk, fingerStream.device()->size(), 0, egChunkVolume/2, currentFinger.myLevel);

            // update parent fingers
        currentFinger.itemsCount = egChunkVolume/2+1;   // level up
        newFinger.itemsCount = egChunkVolume/2;         // level up
    }
    else
    {
        posToInsert -= egChunkVolume/2;
        // qDebug() << "posToInsert = " << posToInsert << ", egChunkVolume/2 - posToInsert = " << egChunkVolume/2 - posToInsert << FN;

        InsertNewFinger(localFingersStream, newFingersChunk, posToInsert, egChunkVolume/2);
        UpdateParentsOffsets(localFingersStream, newFingersChunk, fingerStream.device()->size(), 0, egChunkVolume/2+1, currentFinger.myLevel);

            // update parent fingers
        currentFinger.itemsCount = egChunkVolume/2;   // level up
        newFinger.itemsCount = egChunkVolume/2+1;         // level up
    }

        // update backlink
    if (currentFinger.myLevel == 0)
        indexChunks-> StoreFingerOffset(newFinger.nextChunkOffset, fingerStream.device()->size() + posToInsert*oneFingerSize);

        // write level
    if (currentFinger.myLevel > 0)
    {
        memcpy(fingersBA.data(), newFingersChunk, fingersChunkSize);

        localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        localFingersStream << currentFinger.myLevel;

        memcpy(newFingersChunk, fingersBA.constData(), fingersChunkSize);
    }

    UpdateMinMax(localFingersStream, currentFinger, fingersChunk);
    UpdateMinMax(localFingersStream, newFinger, newFingersChunk);

    currentFinger.nextChunkOffset = parentFinger.nextChunkOffset; // level up
    currentFinger.myOffsetInChunk = parentFinger.myOffsetInChunk;
    currentFinger.myChunkOffset = parentFinger.myChunkOffset;

    currentFinger.myLevel = parentFinger.myLevel;

    newFinger.myLevel = parentFinger.myLevel;
    newFinger.nextChunkOffset = fingerStream.device()->size(); // level up

    /*
    PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    PrintFingerInfo(newFinger, "newFinger " + FNS);

    PrintFingersChunk(fingersChunk, "old chunk after split up " + FNS);
    PrintFingersChunk(newFingersChunk, "new chunk after split up " + FNS);
    */

    // qDebug() << "parentFinger.myOffset = " << parentFinger.myOffset << ", parentFinger.myChunkOffset = " << parentFinger.myChunkOffset << FN;

        // save both chunks, fingers already updated to upper level
    StoreFingersChunk(parentFinger.nextChunkOffset, fingersChunk);
    StoreFingersChunk(fingerStream.device()->size(), newFingersChunk);

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::AppendFingersChunk(QDataStream &localFingersStream)
{
    /*
    PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    PrintFingerInfo(newFinger, "newFinger " + FNS);
    */

        // init new chunk
    memmove (newFingersChunk, zeroFingersChunk,  fingersChunkSize);

    memcpy(fingersBA.data(), newFingersChunk, fingersChunkSize);

        // write new finger
    localFingersStream.device()->seek(0);
    WriteFinger(localFingersStream, newFinger);

        // prepare parent back link
    // indexChunks-> indexesChunkOffset = newFinger.nextChunkOffset; // already set in indexes
    if (currentFinger.myLevel > 0)
        StoreParentOffset(newFinger.nextChunkOffset, fingerStream.device()->size()); // FIXME check
    else // update backlink
        indexChunks-> StoreFingerOffset(newFinger.nextChunkOffset, fingerStream.device()->size());

    newFinger.nextChunkOffset = fingerStream.device()->size();  // level up
    // newFingerChunkOffset = fingerStream.device()->size();            // current level

    localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
    localFingersStream << (fingersLevelType) (currentFinger.myLevel);

    currentFinger = parentFinger;   // level up
    newFinger.myLevel = parentFinger.myLevel;

        // save new chunk, fingers already updated to upper level
    StoreFingersChunk(fingerStream.device()->size(), fingersBA.data());

    return 0;
}

/*
EgFingers<qint32> EgFingersqint32;
EgFingers<quint32> EgFingersquint32;
EgFingers<qint64>  EgFingersqint64;
EgFingers<quint64> EgFingersquint64;
EgFingers<float> EgFingersfloat;
EgFingers<double> EgFingersdouble;
*/
