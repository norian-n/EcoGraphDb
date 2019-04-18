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
    qDebug() << theMessage;
    qDebug() << "fingerInfo.minKey = " << hex << (int) fingerInfo.minKey;
    qDebug() << "fingerInfo.maxKey = " << hex << (int) fingerInfo.maxKey;
    qDebug() << "fingerInfo.myLevel = " << hex << (int) fingerInfo.myLevel;
    qDebug() << "fingerInfo.itemsCount = " << hex << (int) fingerInfo.itemsCount;

    qDebug() << "fingerInfo.nextChunkOffset = " << hex << (int) fingerInfo.nextChunkOffset;
    qDebug() << "fingerInfo.myOffset = " << hex << (int) fingerInfo.myOffset;
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


template <typename KeyType> void EgFingers<KeyType>::RemoveFingerFiles(const QString& IndexFileName)
{
    if (fingerFile.isOpen())
        fingerFile.close();

    fingerFile.setFileName("egdb/" + IndexFileName + ".odf");
    fingerFile.remove();
}

template <typename KeyType> int EgFingers<KeyType>::OpenFingerFileToUpdate(const QString& IndexFileName)
{
    fingerFile.close();

    fingerFile.setFileName("egdb/" + IndexFileName + ".odf");
    fingerStream.setDevice(&fingerFile);

    if (!fingerFile.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open finger file " << fingerFile.fileName();
        return -2;
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::OpenFingerFileToRead(const QString& IndexFileName)
{
    fingerFile.close();

    fingerFile.setFileName("egdb/" + IndexFileName + ".odf");
    fingerStream.setDevice(&fingerFile);

    if (!fingerFile.exists())
    {
        qDebug() << "file doesn't exist " << IndexFileName + ".odf" << FN;
        return -1;
    }

    if (!fingerFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "can't open finger file " << fingerFile.fileName() << FN;
        return -2;
    }

    return 0;
}


template <typename KeyType> int EgFingers<KeyType>::OpenFingerFileToCheck(const QString& IndexFilePath)
{
    fingerFile.close();

    fingerFile.setFileName(IndexFilePath);
    fingerStream.setDevice(&fingerFile);

    if (!fingerFile.exists())
    {
        qDebug() << "file doesn't exist " << IndexFileName << FN;
        return -1;
    }

    if (!fingerFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open finger file " << fingerFile.fileName() << FN;
        return -2;
    }

    return 0;
}


template <typename KeyType> void EgFingers<KeyType>::CloseFingerFiles()
{
    fingerFile.close();
}


template <typename KeyType> void EgFingers<KeyType>::InitRootFinger()
{
    rootFinger.minKey = indexChunks-> theKey;
    rootFinger.maxKey = indexChunks-> theKey;

    rootFinger.myLevel = 1;
    rootFinger.itemsCount = 1;

    rootFinger.nextChunkOffset = rootHeaderSize; // FIXME check
    rootFinger.myChunkOffset = 0;
    rootFinger.myOffset = 0;
}


template <typename KeyType> void EgFingers<KeyType>::LoadRootFinger()
{

    // InitRootHeader();

    // qDebug() << "fingerFile.fileName() = " << fingerFile.fileName() << FN;
    // qDebug() << "fingerFile size = " << fingerStream.device()->size() << FN;

    fingerStream.device()-> seek(0);

    fingerStream >> rootFinger.minKey;
    fingerStream >> rootFinger.maxKey;

    fingerStream >> rootFinger.myLevel;

    fingerStream >> rootFinger.itemsCount;
    fingerStream >> rootFinger.nextChunkOffset;

    rootFinger.myChunkOffset = 0;
    rootFinger.myOffset = 0;

    /*
    qDebug() << "fingersRootHeader.minKey = " << fingersRootHeader.minKey
             << ", fingersRootHeader.maxKey = " << fingersRootHeader.maxKey
             << ", indexChunks-> theKey = " << indexChunks-> theKey
             << ", IndexFileName = " << IndexFileName
             << FN;

     */

    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);
}

template <typename KeyType> void EgFingers<KeyType>::StoreRootFinger(bool minMaxOnly)
{
    fingerStream.device()->seek(0);

    fingerStream << rootFinger.minKey;
    fingerStream << rootFinger.maxKey;

    if (! minMaxOnly)
    {
        fingerStream << rootFinger.myLevel;
        fingerStream << rootFinger.itemsCount;
        fingerStream << rootFinger.nextChunkOffset;
    }

    // FIXME debug assert

    if ((rootFinger.minKey > rootFinger.maxKey)) // || (indexChunks-> theKey > fingersRootHeader.maxKey) || (indexChunks-> theKey < fingersRootHeader.minKey))
            qDebug() << "fingersRootHeader.minKey = " << rootFinger.minKey
                     << ", fingersRootHeader.maxKey = " << rootFinger.maxKey
                     << ", indexChunks-> theKey = " << indexChunks-> theKey
                     << ", IndexFileName = " << IndexFileName
                     << FN;
}

template <typename KeyType> inline void EgFingers<KeyType>::InitFingersChunk()
{
    QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);

    memcpy(fingersBA.data(), zeroFingersChunk, fingersChunkSize);

        // write one finger
    localFingersStream << (KeyType) indexChunks-> theKey;               // min
    localFingersStream << (KeyType) indexChunks-> theKey;               // max
    localFingersStream << (keysCountType) 1;    // count
    localFingersStream << (quint64) indexHeaderSize; // first indexes chunk offset // indexChunks-> indexesChunkOffset; // theFinger.chunkOffset;

    // qDebug() << fingersBA.toHex() << FN;

    memcpy(fingersChunk, fingersBA.constData(), fingersChunkSize);

    // fingersChunkOffset = 0;
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

template <typename KeyType> inline void EgFingers<KeyType>::LoadFingersChunk(quint64 fingersChunkOffset)
{
    fingerStream.device()->seek(fingersChunkOffset);
    fingerStream.readRawData(fingersChunk, fingersChunkSize);

    // currentFinger.myChunkOffset = fingersChunkOffset;

/*
    QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);
    memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

    KeyType min_index, max_index;
    keysCountType count;
    quint64 chunkOffset;

        // read one finger
    localFingersStream >> min_index;    // min
    localFingersStream >> max_index;    // max
    localFingersStream >> count;  // vacant keys
    localFingersStream >> chunkOffset;

    qDebug() << hex << (int) min_index << FN;
    qDebug() << hex << (int) max_index << FN;
    qDebug() << hex << (int) count << FN;
    qDebug() << hex << (int) chunkOffset << FN;
    */
}

template <typename KeyType> inline void EgFingers<KeyType>::ReadFinger(QDataStream &localFingersStream, egFinger<KeyType>& theFinger)
{
    // qDebug() << fingersBA.toHex() << FN;

    localFingersStream.device()->seek(theFinger.myOffset);

    localFingersStream >> theFinger.minKey;
    localFingersStream >> theFinger.maxKey;
    localFingersStream >> theFinger.itemsCount;
    localFingersStream >> theFinger.nextChunkOffset;
}

/*
template <typename KeyType> int EgFingers<KeyType>::FindNextLevelOffsetToInsert()
{
    int chunkDirection;  // shift fingers until match

    fingersLevelType myLevel;

    QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);
    memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

    // qDebug() << fingersBA.toHex() << FN;

        // get level
    localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
    localFingersStream >> myLevel;

    // if (currentFinger.myLevel != 0)
    //    qDebug() << fingersBA.toHex() << FN;

    switch (parentFinger.itemsCount)  // currentFingersChunkHeader.fingersCount
    {
        case 0: return -1;              // no chunks to insert

        case 1: // get first
            currentFinger.myOffset = 0;
            ReadFinger(localFingersStream, currentFinger);

            currentFinger.myLevel = myLevel;

            return 0;
        case 2: // select closest by key
            localFingersStream.device()->seek(0); // fingersHeaderSize
            SelectClosestFingerToInsert(localFingersStream);

            currentFinger.myLevel = myLevel;

            return 0;
    }

    // FIXME - write tests

        // proportional finger lookup
    int posToInsert = (indexChunks-> theKey - parentFinger.minKey)*(parentFinger.itemsCount-1)/(parentFinger.maxKey - parentFinger.minKey);

    if ((indexChunks-> theKey < parentFinger.minKey) || (posToInsert <= 0)) // take first one
    {
        currentFinger.myOffset = 0;
        ReadFinger(localFingersStream, currentFinger);

        currentFinger.myLevel = myLevel;

        return 0;
    }

    if ((indexChunks-> theKey >= parentFinger.maxKey) || (posToInsert >= (parentFinger.itemsCount-1))) // take last one
    {
        currentFinger.myOffset = (parentFinger.itemsCount-1)*oneFingerSize;
        ReadFinger(localFingersStream, currentFinger);

        currentFinger.myLevel = myLevel;

        return 0;
    }

    do
    {
        // qDebug() << "posToInsert = " << posToInsert << FN;
        // PrintFingerInfo(currentFinger);

        localFingersStream.device()->seek(posToInsert*oneFingerSize);
        chunkDirection = SelectClosestFingerToInsert(localFingersStream);

        if (chunkDirection > 0)
            posToInsert++;
        else if ((chunkDirection < 0 ) && (posToInsert > 0))
            posToInsert--;

        if (posToInsert <= 0) // take first one
        {
            currentFinger.myOffset = 0;
            ReadFinger(localFingersStream, currentFinger);

            currentFinger.myLevel = myLevel;

            return 0;
        }
    }
    while (chunkDirection);

    currentFinger.myLevel = myLevel;

    return 0;
}
*/

template <typename KeyType> int EgFingers<KeyType>::FindNextLevelOffsetFirst(QDataStream &localFingersStream, bool isExactEqual)
{
    int fingerPosition;
    egFinger<KeyType> secondFinger;

    if (parentFinger.itemsCount == 0)  // no fingers
        return -2;

    if ((indexChunks-> theKey < parentFinger.minKey) || (parentFinger.itemsCount == 1)) // out of range or only one
    {
        currentFinger.myOffset = 0;
        ReadFinger(localFingersStream, currentFinger);

        if (indexChunks-> theKey < parentFinger.minKey) // out of range
            return 1;

        return 0;
    }

    if (indexChunks-> theKey > parentFinger.maxKey) // out of range or only one
    {
        currentFinger.myOffset = (parentFinger.itemsCount - 1);
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
        currentFinger.myOffset = 0;
        ReadFinger(localFingersStream, currentFinger);

        return 0;
    }

    // qDebug() << "indexChunks-> theKey = " << indexChunks-> theKey << "fingerPosition = " << fingerPosition << FN;

    currentFinger.myOffset = fingerPosition*oneFingerSize;
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

                currentFinger.myOffset = fingerPosition*oneFingerSize;
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
                    secondFinger.myOffset = (fingerPosition-1)*oneFingerSize;
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

                currentFinger.myOffset = fingerPosition*oneFingerSize;
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
                    secondFinger.myOffset = (fingerPosition-1)*oneFingerSize;
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

    currentFinger.myOffset = fingerPosition*oneFingerSize;
    ReadFinger(localFingersStream, currentFinger);

    return 0;
}


template <typename KeyType> int EgFingers<KeyType>::FindFingerInChunkToInsert(QDataStream &localFingersStream)
{
    int fingerPosition = parentFinger.itemsCount - 1; // last finger first

    if (parentFinger.itemsCount == 0)  // no fingers, corrupted structure FIXME process
        return -1;

        // read last finger
    currentFinger.myOffset = fingerPosition*oneFingerSize;
    ReadFinger(localFingersStream, currentFinger);

    fingerPosition--;

        // go left until appropriate finger
    while ((fingerPosition >= 0) && (indexChunks-> theKey < currentFinger.minKey))
    {
        currentFinger.myOffset = fingerPosition*oneFingerSize;
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
        currentFinger.myOffset = (parentFinger.itemsCount - 1)*oneFingerSize;
        ReadFinger(localFingersStream, currentFinger);

        if (indexChunks-> theKey > parentFinger.maxKey) // out of range
            return 1;

        return 0;
    }

    if (indexChunks-> theKey < parentFinger.minKey) // out of range or only one
    {
        currentFinger.myOffset = 0;
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
        currentFinger.myOffset = (parentFinger.itemsCount - 1)*oneFingerSize;
        ReadFinger(localFingersStream, currentFinger);

        return 0;
    }

    // qDebug() << "fingerPosition = " << fingerPosition << FN;

    currentFinger.myOffset = fingerPosition*oneFingerSize;
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

                currentFinger.myOffset = fingerPosition*oneFingerSize;
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
                    secondFinger.myOffset = (fingerPosition+1)*oneFingerSize;
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

                currentFinger.myOffset = fingerPosition*oneFingerSize;
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
                    secondFinger.myOffset = (fingerPosition+1)*oneFingerSize;
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

    currentFinger.myOffset = fingerPosition*oneFingerSize;
    ReadFinger(localFingersStream, currentFinger);

    return 0;
}

/*
template <typename KeyType> int EgFingers<KeyType>::FindNextLevelOffsetLast(bool isExact)
{
    egFinger<KeyType> localFingers[2];

    int activeFinger = 0;
    int direction = 0;

    int emergencyCounter = 0;

    if (parentFinger.itemsCount == 0)  // no fingers
        return -2;

    QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);
    memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

    if (indexChunks-> theKey > parentFinger.maxKey) // out of range
    {
        currentFinger.myOffset = (parentFinger.itemsCount-1)*oneFingerSize;
        ReadFinger(localFingersStream, currentFinger);

        return -1;
    }
    else if ((indexChunks-> theKey < parentFinger.minKey) || (parentFinger.itemsCount == 1)) // out of range or the one
    {
        currentFinger.myOffset = 0;
        ReadFinger(localFingersStream, currentFinger);

        return -1;
    }

    // FIXME - write tests

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        // proportional finger lookup
    int posToInsert = (indexChunks-> theKey - parentFinger.minKey)*(parentFinger.itemsCount)/(parentFinger.maxKey - parentFinger.minKey);

    // PrintFingersChunk(fingersChunk, "fingers chunk" + FNS);

    do
    {
        localFingers[activeFinger].myOffset = posToInsert*oneFingerSize;
        ReadFinger(localFingersStream, localFingers[activeFinger]);

        // PrintFingerInfo(localFingers[activeFinger], "localFingers[activeFinger] " + FNS);
        // qDebug() << "posToInsert = " << posToInsert << "theKey = " << hex << indexChunks-> theKey << FN; //  << "localFingers[activeFinger].minKey = " << hex << localFingers[activeFinger].minKey << FN;
        // qDebug() << "condition = "  << (indexChunks-> theKey <= localFingers[activeFinger].maxKey) <<  " " << (indexChunks-> theKey > localFingers[activeFinger].minKey) << FN;

        if ((indexChunks-> theKey < localFingers[activeFinger].maxKey) && (indexChunks-> theKey > localFingers[activeFinger].minKey)) // finger found
        {
            currentFinger = localFingers[activeFinger];
            currentFinger.myOffset = posToInsert*oneFingerSize;

            // PrintFingerInfo(currentFinger, "currentFinger 2 " + FNS);

            return 0;
        }

        if (isExact && (indexChunks-> theKey == localFingers[activeFinger].minKey) && (indexChunks-> theKey < localFingers[activeFinger].maxKey)) // exact match
        {
            currentFinger = localFingers[activeFinger];
            currentFinger.myOffset = posToInsert*oneFingerSize;

            // PrintFingerInfo(currentFinger, "currentFinger 2 " + FNS);

            return 0;
        }

        if ((direction > 0) && (indexChunks-> theKey < localFingers[activeFinger].minKey) && (indexChunks-> theKey > localFingers[!activeFinger].maxKey))
        {
            currentFinger = localFingers[!activeFinger];
            currentFinger.myOffset = (posToInsert+1)*oneFingerSize;
            return 1; // key between fingers
        }

        if ((direction < 0) && (indexChunks-> theKey < localFingers[!activeFinger].minKey) && (indexChunks-> theKey > localFingers[activeFinger].maxKey))
        {
            currentFinger = localFingers[activeFinger];
            currentFinger.myOffset = posToInsert*oneFingerSize;
            return 1; // key between fingers
        }
            // ???
        if ((direction > 0) && (indexChunks-> theKey == localFingers[!activeFinger].maxKey) && (indexChunks-> theKey < localFingers[activeFinger].minKey))
        {
            currentFinger = localFingers[!activeFinger];
            currentFinger.myOffset = (posToInsert-1)*oneFingerSize;

            // PrintFingerInfo(currentFinger, "currentFinger 2 " + FNS);

            return 0;
        }

        if ((indexChunks-> theKey > localFingers[activeFinger].maxKey) && (posToInsert < (parentFinger.itemsCount-1)))
        {
            posToInsert++;
            direction = 1;
        }
        else
        {
            posToInsert--;
            direction = -1;
        }

        // qDebug() << "posToInsert = " << posToInsert << "direction = " << direction << FN;

        if (posToInsert >= (parentFinger.itemsCount-1)) // take last one
        {
            currentFinger.myOffset = (parentFinger.itemsCount-1)*oneFingerSize;
            ReadFinger(localFingersStream, currentFinger);

            return 0;
        }

        if (posToInsert < 0) // take first one
        {
            currentFinger.myOffset = 0;
            ReadFinger(localFingersStream, currentFinger);

            return 0;
        }

            // swap
        if (activeFinger == 0)
        {
            localFingers[1] = localFingers[0];
            activeFinger = 1;
        }
        else
        {
            localFingers[0] = localFingers[1];
            activeFinger = 0;
        }

        emergencyCounter++;
    }
    while (emergencyCounter < (parentFinger.itemsCount + 1));

    return -3; // emergency counter overflow
}
*/
/*
template <typename KeyType> int EgFingers<KeyType>::SelectClosestFingerToInsert(QDataStream& localFingersStream)
{

    egFinger<KeyType> localFinger1, localFinger2;

    // PrintFingersChunk(fingersBA.data(), "fingers chunk " + FNS);

        // get first finger
    localFinger1.myOffset = localFingersStream.device()->pos();
    ReadFinger(localFingersStream, localFinger1);

    // PrintFingerInfo(localFinger1, "localFinger1 " + FNS);

    if (indexChunks-> theKey < localFinger1.minKey) // min1)
    {
        currentFinger = localFinger1;
        return -1; // chunk is below
    }

        // get second finger
    localFinger2.myOffset = localFingersStream.device()->pos();
    ReadFinger(localFingersStream, localFinger2);

    // PrintFingerInfo(localFinger2, "localFinger2 " + FNS);

    if (indexChunks-> theKey > localFinger2.maxKey) // max2)
    {
        currentFinger = localFinger2;
        return 1; // chunk is above
    }

    if (indexChunks-> theKey <= localFinger1.maxKey) // max1)
    {
        currentFinger = localFinger1;
        return 0;   // finger found
    }

    if (indexChunks-> theKey >= localFinger2.minKey) // min2)
    {
        currentFinger = localFinger2;
        return 0;   // finger found
    }

        // index between chunks
    if ((localFinger1.itemsCount < egChunkVolume) || (localFinger2.itemsCount == egChunkVolume))
    {
        currentFinger = localFinger1;
        return 0;   // finger found
    }

    currentFinger = localFinger2;

    return 0;   // finger found
}
*/
/*
template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkToInsert()
{
    parentFinger = fingersRootHeader;

    fingersChain.clear();

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

        // fill fingers chain
    do
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

        LoadFingersChunk(parentFinger.nextChunkOffset);
        FindNextLevelOffsetToInsert();  // get currentFinger

        currentFinger.myChunkOffset = parentFinger.nextChunkOffset; // store chunk offset to update parent finger instantly

        if ( currentFinger.myLevel > 0 )
        {
            fingersChain.append(currentFinger);
            parentFinger = currentFinger;
        }
    }
    while (currentFinger.myLevel > 0);

    // qDebug() << "theKey = " << hex << (int) indexChunks-> theKey << FN;

    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    indexChunks-> indexesChunkOffset = currentFinger.nextChunkOffset;

    return 0;
}
*/

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkToInsert()
{
    int res = 0;

    fingersChain.clear();

    parentFinger = rootFinger;

    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);

        // fill fingers chain
    do
    {
        LoadFingersChunk(parentFinger.nextChunkOffset);
        // res = FindNextLevelOffsetLast(isExact);  // get currentFinger

        QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

            // get level
        localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        localFingersStream >> currentFinger.myLevel;

        // qDebug() << "myLevel = " << currentFinger.myLevel << FN;

        // res = FindNextLevelOffsetLast(localFingersStream, true);  // get currentFinger

        res = FindFingerInChunkToInsert(localFingersStream);

        currentFinger.myChunkOffset = parentFinger.nextChunkOffset; // shortcut (???)

        if ( currentFinger.myLevel > 0 )
        {
            fingersChain.append(currentFinger);
            parentFinger = currentFinger;
        }
    }
    while ((currentFinger.myLevel > 0) && (res >= 0));

    // qDebug() << "theKey = " << hex << (int) indexChunks-> theKey << FN;

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    indexChunks-> indexesChunkOffset = currentFinger.nextChunkOffset;


    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkFirst(bool isExactEqual)
{
    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);

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

        LoadFingersChunk(parentFinger.nextChunkOffset);

        QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

            // get level
        localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        localFingersStream >> currentFinger.myLevel;

        // qDebug() << "myLevel = " << currentFinger.myLevel << FN;

        res = FindNextLevelOffsetFirst(localFingersStream, isExactEqual);  // get currentFinger

            // FIXME debug
/*
        localFingersStream.device()->seek(egChunkVolume * oneFingerSize);
        localFingersStream >> parentFingerOffset;

        qDebug() << "parentFingerOffset 1 = " << parentFingerOffset << "parentFingerOffset 2 = " << parentFinger.myChunkOffset + parentFinger.myOffset << FN;
        */

        currentFinger.myChunkOffset = parentFinger.nextChunkOffset; // shortcut (???)

        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        if (currentFinger.myLevel > 0)
            parentFinger = currentFinger;
        // myLevel--;
    }
    while ((currentFinger.myLevel > 0) && (res >= 0)); // FIXME

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    indexChunks-> indexesChunkOffset = currentFinger.nextChunkOffset;

    return res;
}


template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkLast(bool isExactEqual)
{
    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);

    int res = 0;

        // check root borders
    // if (indexChunks-> theKey < fingersRootHeader.minKey) // no first chunk found
    //    return -1;

    // int myLevel  = fingersRootHeader.myLevel;
    parentFinger = rootFinger;

        // fill fingers chain
    do
    {
        // qDebug() << "myLevel = " << myLevel << FN;

        LoadFingersChunk(parentFinger.nextChunkOffset);
        // res = FindNextLevelOffsetLast(isExact);  // get currentFinger

        QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

            // get level
        localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        localFingersStream >> currentFinger.myLevel;

        // PrintFingersChunk(fingersBA.data(), "fingers chunk " + FNS);

        // qDebug() << "myLevel = " << currentFinger.myLevel << FN;

        res = FindNextLevelOffsetLast(localFingersStream, isExactEqual);  // get currentFinger

        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

                    // FIXME debug
/*
        localFingersStream.device()->seek(egChunkVolume * oneFingerSize);
        localFingersStream >> parentFingerOffset;

        qDebug() << "parentFingerOffset 1 = " << parentFingerOffset << "parentFingerOffset 2 = " << parentFinger.myChunkOffset + parentFinger.myOffset << FN;
        */

        currentFinger.myChunkOffset = parentFinger.nextChunkOffset; // shortcut (???)


        if (currentFinger.myLevel > 0)
            parentFinger = currentFinger;
        // myLevel--;
    }
    while ((currentFinger.myLevel > 0) && (res >= 0)); // FIXME

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    indexChunks-> indexesChunkOffset = currentFinger.nextChunkOffset;

    return res;
}


template <typename KeyType> int EgFingers<KeyType>::UpdateFingerAfterInsert()
{

    UpdateTheFinger(currentFinger);

    // PrintFingerInfo(currentFinger, "currentFinger " + FNS );
    // PrintFingerInfo(parentFinger, "parentFinger " + FNS );

    fingerStream.device()->seek(currentFinger.myChunkOffset + currentFinger.myOffset);

        // write one finger
    fingerStream << currentFinger.minKey;
    fingerStream << currentFinger.maxKey;
    fingerStream << currentFinger.itemsCount;

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateFingersChainAfterInsert()
{

    bool indexOutOfRange = true;

            // all chain up
    while ((! fingersChain.isEmpty()) && indexOutOfRange)
    {
        indexOutOfRange = (indexChunks-> theKey < fingersChain.last().minKey) || (indexChunks-> theKey > fingersChain.last().maxKey);

        if (indexOutOfRange)
        {
            UpdateTheFinger(fingersChain.last(), false); // false means do not increment count

            // PrintFingerInfo(fingersChain.last(), "parentFinger " + FNS);

            fingerStream.device()->seek(fingersChain.last().myChunkOffset + fingersChain.last().myOffset);

                // write one finger only
            fingerStream << fingersChain.last().minKey;    // min
            fingerStream << fingersChain.last().maxKey;    // max
        }

        fingersChain.removeLast();
    }

        // update root header
    if (indexOutOfRange)
    {
        UpdateTheFinger(rootFinger, false);   // false means do not increment count

        // indexChunks-> StoreRootHeader(true); // true means store min max only
        StoreRootFinger(true);
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


template <typename KeyType> int EgFingers<KeyType>::UpdateFingerCountAfterDelete()
{
        // update keys count, decremented by indexes
    fingerStream.device()->seek(currentFingerOffset + sizeof(KeyType) * 2);
    fingerStream << currentKeysCount;

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateMinValueUp()
{
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

            StoreRootFinger(true);
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

            StoreRootFinger(true);
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

    QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);

        // calc this chunk offset and finger position
    quint64 fingersChunkOffset = currentFingerOffset - ((currentFingerOffset-rootHeaderSize) % fingersChunkSize);
    keysCountType fingerNum = (currentFingerOffset - fingersChunkOffset) / oneFingerSize;

    isFirstFinger = (fingerNum == 0);
    isLastFinger  = (fingerNum == currentKeysCount-1);

    // qDebug() << "currentKeysCount = " << currentKeysCount << ", fingerNum = " << fingerNum << FN;

    LoadFingersChunk(fingersChunkOffset);

        // update min/max
    if (isLastFinger)
    {
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        localFingersStream.device()->seek(fingerNum * oneFingerSize + sizeof(KeyType));
        localFingersStream >> oldValue;

        localFingersStream.device()->seek((fingerNum - 1) * oneFingerSize + sizeof(KeyType));
        localFingersStream >> newMaxValue;

        if (newMaxValue < oldValue)
        {
            maxValueChanged = true;
        }
    }
    else if (isFirstFinger)
    {
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        localFingersStream.device()->seek(0);
        localFingersStream >> oldValue;

        localFingersStream.device()->seek(oneFingerSize);
        localFingersStream >> newMinValue;

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

        localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        localFingersStream >> myLocalLevel;

        // qDebug() << "myLocalLevel = " << myLocalLevel << FN;

            // update parent links
        UpdateParentsOffsets2(localFingersStream, fingersChunk, fingersChunkOffset,  fingerNum, currentKeysCount-1, myLocalLevel);
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


template <typename KeyType> void EgFingers<KeyType>::DeleteParentFinger() // recursive
{
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

                StoreRootFinger(false);  // store min/max and count
            }

            return;
        }
        else if (currentKeysCount == 1)   // last finger
        {
            DeleteFingersChunk(fingersChunkOffset); // just write zeroes
        }

        currentFingerOffset = parentFingerOffset;   // go up
    }
}
    // bool isOnlyFinger = true; // stub

    // keysCountType fingersCount;

    // quint64 currentFingerOffset = parentFingerOffset;
/*
    if (currentFingerOffset)
    {
            // calc this chunk offset
        quint64 fingersChunkOffset = currentFingerOffset - ((currentFingerOffset-rootHeaderSize) % fingersChunkSize);

            // get parent
        fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
        fingerStream >> parentFingerOffset;
        fingerStream >> currentFinger.myLevel;

        // quint64 parentChunkOffset = parentFingerOffset - ( parentFingerOffset % fingersChunkSize );

        if (parentFingerOffset) // parent not root
        {
                // check count
            fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType)*2);
            fingerStream >> currentKeysCount;
        }
        else
            currentKeysCount = fingersRootHeader.itemsCount;

        qDebug() << "parentFingerOffset " << hex << (int) parentFingerOffset << " , currentKeysCount = " << currentKeysCount << FN;

        if (currentKeysCount > 1) // not Only One Finger
        {
            minValueChanged = false;
            maxValueChanged = false;

            DeleteSpecificFinger(); // only one by currentFingerOffset and currentKeysCount

                // decrease parent items count
            if (parentFingerOffset) // parent not root
            {
                currentKeysCount--;

                fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType) * 2 );
                fingerStream << currentKeysCount;

                    // update min/max - current and up
                currentFingerOffset = parentFingerOffset;

                if (minValueChanged)
                    UpdateMinValueUp();
                else if (maxValueChanged)
                    UpdateMaxValueUp();
            }
            else
            {
                fingersRootHeader.itemsCount--;

                if (minValueChanged)
                    fingersRootHeader.minKey = newMinValue;
                else if (maxValueChanged)
                    fingersRootHeader.maxKey = newMaxValue;

                StoreRootHeader(false);  // store min/max and count
            }
        }
        else // == 1
        {
            DeleteFingersChunk(fingersChunkOffset);

            currentFingerOffset = parentFingerOffset;

            DeleteParentFinger(); // recursion
        }
    }
    else
    {
            // remove all if no more fingers
        RemoveIndexFiles(IndexFileName);

        indexChunks-> RemoveIndexFiles(IndexFileName);
    }
    */


template <typename KeyType> int EgFingers<KeyType>::UpdateFingersChainAfterSplit()
{
    bool splitParentChunk;
    bool appendMode;

    QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);

   /* if (! fingersChain.isEmpty())
        parentFinger = fingersChain.last();
    else
        parentFinger = fingersRootHeader;

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

    if (parentFinger.itemsCount < egChunkVolume)
    {
        InsertSplittedFinger(localFingersStream);     // if have space just add new item for new chunk and recalc min/max        
        splitParentChunk = false;
    }
    else
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

        if (appendMode)
            AppendFingersChunk(localFingersStream);
        else
            SplitFingersChunk(localFingersStream);

        splitParentChunk = true;        
    }

        // store parent backlink
    indexChunks-> StoreFingerOffset(newFingerOffset);

    // qDebug() << "fingersChain.count = " << fingersChain.count() << FN;

            // FIXME stub
            */
    do
    {
        if (! fingersChain.isEmpty())
        {
            parentFinger = fingersChain.last();
            fingersChain.removeLast();
        }
        else
            parentFinger = rootFinger;
    /*
        PrintFingerInfo(parentFinger, "parentFinger " + FNS);
        PrintFingerInfo(currentFinger, "currentFinger " + FNS);
        PrintFingerInfo(newFinger, "newFinger " + FNS);
    */

        LoadFingersChunk(parentFinger.nextChunkOffset);
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        if (parentFinger.itemsCount < egChunkVolume)
        {
            InsertSplittedFinger2(localFingersStream);     // if have space just add new item for new chunk and recalc min/max
            splitParentChunk = false; 
        }
        else
        {
            appendMode = (indexChunks-> theKey >= parentFinger.maxKey);

            if (appendMode)
                AppendFingersChunk(localFingersStream);
            else
                SplitFingersChunk2(localFingersStream);

            splitParentChunk = true;


            //if (parentFinger.myLevel < fingersRootHeader.myLevel)
            //{
                /*
                PrintFingerInfo(parentFinger, "parentFinger " + FNS);
                PrintFingerInfo(newFinger, "newFinger " + FNS);

                qDebug() << "newFingerOffset = " << hex << (int) newFingerOffset << FN;
                qDebug() << "parentFinger.newOffset = " << hex << (int) (parentFinger.myChunkOffset + parentFinger.myOffset + oneFingerSize) << FN;
                */

                // StoreParentOffset(newFinger.nextChunkOffset, parentFinger.myChunkOffset + parentFinger.myOffset + oneFingerSize); // wrong !!

                    // shift parent up
                // fingersChain.removeLast();
            //}
        }
    }
    while ((parentFinger.myLevel < rootFinger.myLevel) && splitParentChunk);

    if (splitParentChunk && (parentFinger.myLevel == rootFinger.myLevel)) // new root chunk is required
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
        AddNewRootChunk(localFingersStream);

        StoreParentOffset(currentFinger.nextChunkOffset, rootFinger.nextChunkOffset); // FIXME CHECK
        StoreParentOffset(newFinger.nextChunkOffset, rootFinger.nextChunkOffset + oneFingerSize);

        UpdateTheFinger(rootFinger, false);   // false means do not increment count

        rootFinger.myLevel++;
        rootFinger.itemsCount = 2; // new top chunk

        // indexChunks-> StoreRootHeader();

        StoreRootFinger();

        // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);
    }
    else
    {
        if (parentFinger.myLevel == rootFinger.myLevel) // update root only
        {
            UpdateTheFinger(rootFinger);

            // indexChunks-> StoreRootHeader();

            StoreRootFinger();
        }
        else
        {
            UpdateFingersChainAfterInsert();
        }
    }

    return 0;
}

template <typename KeyType> inline void EgFingers<KeyType>::UpdateTheFinger(egFinger<KeyType>& theFinger, bool addOne)
{
    if (indexChunks-> theKey > theFinger.maxKey)
        theFinger.maxKey =  indexChunks-> theKey;
    else if (indexChunks-> theKey < theFinger.minKey)
        theFinger.minKey =  indexChunks-> theKey;

    if (addOne)
        theFinger.itemsCount++;
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

template <typename KeyType> int EgFingers<KeyType>::InsertSplittedFinger2(QDataStream &localFingersStream)
{
    posToInsert = currentFinger.myOffset/oneFingerSize + 1;

    localFingersStream.device()->seek(currentFinger.myOffset);

    WriteFinger(localFingersStream, currentFinger);         // update current

    memcpy(fingersChunk, fingersBA.constData(), fingersChunkSize);

    InsertNewFinger2(localFingersStream, fingersChunk, posToInsert, parentFinger.itemsCount);
    UpdateParentsOffsets2(localFingersStream, fingersChunk, parentFinger.nextChunkOffset, posToInsert, parentFinger.itemsCount+1, currentFinger.myLevel);

    StoreFingersChunk(parentFinger.nextChunkOffset, fingersChunk);

    // PrintFingersChunk(fingersBA.data(), "fingers chunk after split/append " + FNS);

    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

    if (parentFinger.myLevel < rootFinger.myLevel)
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);


        UpdateTheFinger(parentFinger);

        fingerStream.device()->seek(parentFinger.myChunkOffset + parentFinger.myOffset);
        WriteFinger(fingerStream, parentFinger);
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::InsertSplittedFinger(QDataStream &localFingersStream)
{
    posToInsert = currentFinger.myOffset/oneFingerSize + 1; // FIXME - insert to left

    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    // PrintFingerInfo(newFinger, "newFinger " + FNS);

    // qDebug() << "posToInsert = " << posToInsert << ", fingersCount = " << parentFinger.itemsCount << FN;

        // check if not last position, move tail
    if (posToInsert < parentFinger.itemsCount)
    {
        // qDebug() << "posToInsert = " << posToInsert << ", fingersCount = " << parentFinger.itemsCount << FN;
        memmove (fingersChunk + (posToInsert+1)*oneFingerSize, fingersChunk + posToInsert*oneFingerSize,  oneFingerSize*(parentFinger.itemsCount - posToInsert));
        // PrintFingersChunk(fingersChunk, "fingers chunk after memmove " + FNS);
    }

    memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

    localFingersStream.device()->seek(currentFinger.myOffset);

    WriteFinger(localFingersStream, currentFinger);     // update current finger
    WriteFinger(localFingersStream, newFinger);         // add new finger

    //if (posToInsert < parentFinger.itemsCount) // last one
    //{
    // update parent backlinks - shifted fingers
    for (int i=posToInsert; i < parentFinger.itemsCount+1; i++)
    {
        quint64 nextLevelOffset;

        // get next level offset
        localFingersStream.device()->seek(i * oneFingerSize + sizeof(KeyType)*2 + sizeof(keysCountType));
        localFingersStream >> nextLevelOffset;

        // qDebug() << "i = " << i << ", nextLevelOffset = " << hex << (int) nextLevelOffset << FN;

        // write actual backlink
        if (currentFinger.myLevel == 0)
            indexChunks-> StoreFingerOffset(nextLevelOffset, parentFinger.nextChunkOffset + i * oneFingerSize);
        else
            StoreParentOffset(nextLevelOffset, parentFinger.nextChunkOffset + i * oneFingerSize);
    }
    //}



    // PrintFingersChunk(fingersBA.data(), "fingers chunk after split/append " + FNS);

    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

    StoreFingersChunk(parentFinger.nextChunkOffset, fingersBA.data());

/*
    if (currentFinger.myLevel == 0)
        indexChunks-> StoreFingerOffset(parentFinger.nextChunkOffset + currentFinger.myOffset + oneFingerSize);
    else
        StoreParentOffset(newFinger.nextChunkOffset, parentFinger.nextChunkOffset + currentFinger.myOffset + oneFingerSize);
        */

    // newFingerOffset = parentFinger.nextChunkOffset + posToInsert * oneFingerSize; // current level for back link

        // update parent finger
    // if (! fingersChain.isEmpty()) // parent is not a root
    if (parentFinger.myLevel < rootFinger.myLevel)
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);


        UpdateTheFinger(parentFinger);

        fingerStream.device()->seek(parentFinger.myChunkOffset + parentFinger.myOffset);
        WriteFinger(fingerStream, parentFinger);

/*

        LoadFingersChunk(parentFinger.myChunkOffset);
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        // qDebug() << "fingersChunk" << fingersBA.toHex() << FN;

        localFingersStream.device()->seek(parentFinger.myOffset);
        WriteFinger(localFingersStream, parentFinger);

        StoreFingersChunk(parentFinger.myChunkOffset, fingersBA.data());
        */
    }

    return 0;
}


template <typename KeyType> int EgFingers<KeyType>::InsertNewFinger2(QDataStream &localFingersStream, char* theChunk, int posToInsert, int itemsCount)
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

template <typename KeyType> int EgFingers<KeyType>::UpdateParentsOffsets2(QDataStream &localFingersStream, char* theChunk,
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
        if (myLocalLevel == 0)
            indexChunks-> StoreFingerOffset(nextLevelOffset, myChunkOffset + i * oneFingerSize);
        else
            StoreParentOffset(nextLevelOffset, myChunkOffset + i * oneFingerSize);
    }

    return 0;
}


template <typename KeyType> int EgFingers<KeyType>::SplitFingersChunk2(QDataStream &localFingersStream)
{
    // KeyType minKey, maxKey;

    // qDebug() << "initialPosToInsert = " << posToInsert << ", fingersCount = " << parentFinger.itemsCount << FN;

        // update current finger
    memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

    localFingersStream.device()->seek(currentFinger.myOffset);
    WriteFinger(localFingersStream, currentFinger);

    memcpy(fingersChunk, fingersBA.constData(), fingersChunkSize);

        // insert new finger next to current
    posToInsert = currentFinger.myOffset/oneFingerSize + 1;

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
        InsertNewFinger2(localFingersStream, fingersChunk, posToInsert, egChunkVolume/2);
        UpdateParentsOffsets2(localFingersStream, fingersChunk, parentFinger.nextChunkOffset, posToInsert, egChunkVolume/2+1, currentFinger.myLevel);
        UpdateParentsOffsets2(localFingersStream, newFingersChunk, fingerStream.device()->size(), 0, egChunkVolume/2, currentFinger.myLevel);

            // update parent fingers
        currentFinger.itemsCount = egChunkVolume/2+1;   // level up
        newFinger.itemsCount = egChunkVolume/2;         // level up
    }
    else
    {
        posToInsert -= egChunkVolume/2;
        // qDebug() << "posToInsert = " << posToInsert << ", egChunkVolume/2 - posToInsert = " << egChunkVolume/2 - posToInsert << FN;

        InsertNewFinger2(localFingersStream, newFingersChunk, posToInsert, egChunkVolume/2);
        UpdateParentsOffsets2(localFingersStream, newFingersChunk, fingerStream.device()->size(), 0, egChunkVolume/2+1, currentFinger.myLevel);

            // update parent fingers
        currentFinger.itemsCount = egChunkVolume/2;   // level up
        newFinger.itemsCount = egChunkVolume/2+1;         // level up
    }

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
    currentFinger.myOffset = parentFinger.myOffset;
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

template <typename KeyType> int EgFingers<KeyType>::SplitFingersChunk(QDataStream &localFingersStream)
{
    // KeyType minKey, maxKey;
/*
    PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    PrintFingerInfo(newFinger, "newFinger " + FNS);
    */

    posToInsert = currentFinger.myOffset/oneFingerSize + 1;

    // qDebug() << "initialPosToInsert = " << posToInsert << ", fingersCount = " << parentFinger.itemsCount << FN;

        // update current finger
    memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

    localFingersStream.device()->seek(currentFinger.myOffset);
    WriteFinger(localFingersStream, currentFinger);

    memcpy(fingersChunk, fingersBA.constData(), fingersChunkSize);

        // init new chunk
    memmove (newFingersChunk, zeroFingersChunk,  fingersChunkSize);

        // split chunks    
    memmove (newFingersChunk, fingersChunk + egChunkVolume*oneFingerSize/2,  egChunkVolume*oneFingerSize/2);
    // PrintFingersChunk(newFingersChunk, "new chunk after split up " + FNS);
        // add zeroes to first
    memmove (fingersChunk + egChunkVolume*oneFingerSize/2, zeroFingersChunk,  egChunkVolume*oneFingerSize/2);
    // PrintFingersChunk(fingersChunk, "old chunk after split up " + FNS);

        // recalc position to splitted chunks
    if ( posToInsert < (egChunkVolume/2+1)) // insert to first part
    {
        // qDebug() << "posToInsert = " << posToInsert << ", egChunkVolume/2 - posToInsert = " << egChunkVolume/2 - posToInsert << FN;

        // check if not last position, move tail
        if (posToInsert < egChunkVolume/2) // not last one
        {
            memmove (fingersChunk + (posToInsert+1)*oneFingerSize, fingersChunk + posToInsert*oneFingerSize,  oneFingerSize*(egChunkVolume/2 - posToInsert)); //  + 1
            // PrintFingersChunk(fingersChunk, "old chunk after memmove " + FNS);
        }

        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

            // write new finger
        localFingersStream.device()->seek(posToInsert*oneFingerSize);
        WriteFinger(localFingersStream, newFinger);

        // newFingerOffset = parentFinger.nextChunkOffset +  posToInsert*oneFingerSize; // current level

            // write level
        localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        localFingersStream << currentFinger.myLevel;

        memcpy(fingersChunk, fingersBA.constData(), fingersChunkSize);

        // PrintFingersChunk(fingersChunk, "old chunk after all " + FNS);

        localFingersStream.device()->seek(0);
        localFingersStream >> currentFinger.minKey; // level up

        localFingersStream.device()->seek(egChunkVolume/2 * oneFingerSize + sizeof(KeyType));
        localFingersStream >> currentFinger.maxKey;

            // update parent fingers
        currentFinger.itemsCount = egChunkVolume/2+1; // level up

        // qDebug() << "currentFinger.minKey =  " << hex << (int) currentFinger.minKey << ", currentFinger.maxKey = " << hex << (int) currentFinger.maxKey << FN;

        memcpy(fingersBA.data(), newFingersChunk, fingersChunkSize);

            // write level
        localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        localFingersStream << currentFinger.myLevel;

            // get new min/max
        memcpy(newFingersChunk, fingersBA.constData(), fingersChunkSize);

        localFingersStream.device()->seek(0);
        localFingersStream >> newFinger.minKey;

        localFingersStream.device()->seek((egChunkVolume/2 - 1) * oneFingerSize + sizeof(KeyType));
        localFingersStream >> newFinger.maxKey;

        newFinger.itemsCount = egChunkVolume/2; // level up

        // qDebug() << "newFinger.minKey =  " << hex << (int) newFinger.minKey << ", newFinger.maxKey = " << hex << (int) newFinger.maxKey << FN;

        // qDebug() << "posToInsert = " << posToInsert << ", currentFinger.itemsCount = " << currentFinger.itemsCount << FN;

            // update parent backlinks - shifted fingers
        for (int i=posToInsert; i < currentFinger.itemsCount; i++)
        {
            quint64 nextLevelOffset;

                // get next level offset
            localFingersStream.device()->seek(i * oneFingerSize + sizeof(KeyType)*2 + sizeof(keysCountType));
            localFingersStream >> nextLevelOffset;

                // write actual backlink
            if (currentFinger.myLevel == 0)
            {
                // indexChunks-> indexesChunkOffset = nextLevelOffset;
                indexChunks-> StoreFingerOffset(nextLevelOffset, currentFinger.myChunkOffset + i * oneFingerSize);
            }
            else
              StoreParentOffset(nextLevelOffset, currentFinger.myChunkOffset + i * oneFingerSize);
        }

    }
    else // insert to second Part
    {
        posToInsert -= egChunkVolume/2;

        // qDebug() << "posToInsert = " << posToInsert << ", egChunkVolume/2 - posToInsert = " << egChunkVolume/2 - posToInsert << FN;

        // check if not last position, move tail
        if (posToInsert < egChunkVolume/2) // not last one
        {
            // qDebug() << "posToInsert = " << posToInsert << ", itemsCount = " << currentFinger.itemsCount << FN;
            memmove (newFingersChunk + (posToInsert+1)*oneFingerSize, newFingersChunk + posToInsert*oneFingerSize,  oneFingerSize*(egChunkVolume/2 - posToInsert)); //  + 1
            // PrintFingersChunk(newFingersChunk, "new chunk after memmove " + FNS);
        }

        memcpy(fingersBA.data(), newFingersChunk, fingersChunkSize);

            // write new finger
        localFingersStream.device()->seek(posToInsert*oneFingerSize);
        WriteFinger(localFingersStream, newFinger);
        // qDebug() << "newFingersChunk after all " << fingersBA.toHex() << FN;

            // write level
        localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        localFingersStream << currentFinger.myLevel;

        // newFingerOffset = fingerStream.device()->size() +  posToInsert*oneFingerSize; // current level FIXME write to index chunk

        memcpy(newFingersChunk, fingersBA.constData(), fingersChunkSize);

        // PrintFingersChunk(newFingersChunk, "new chunk after all " + FNS);

            // update parent fingers
        localFingersStream.device()->seek(0);
        localFingersStream >> newFinger.minKey;

        localFingersStream.device()->seek(egChunkVolume/2 * oneFingerSize + sizeof(KeyType));
        localFingersStream >> newFinger.maxKey;

        newFinger.itemsCount = egChunkVolume/2+1; // level up

        // qDebug() << "posToInsert = " << posToInsert << ", newFinger.itemsCount = " << newFinger.itemsCount
        //         << ", currentFinger.myLevel = " << currentFinger.myLevel << FN;

            // update parent backlinks - all fingers
        for (int i=0; i < newFinger.itemsCount; i++)
        {
            quint64 nextLevelOffset;

                // get next level offset
            localFingersStream.device()->seek(i * oneFingerSize + sizeof(KeyType)*2 + sizeof(keysCountType));
            localFingersStream >> nextLevelOffset;

                // write actual backlink
            if (currentFinger.myLevel == 0)
            {
                // indexChunks-> indexesChunkOffset = nextLevelOffset;
                indexChunks-> StoreFingerOffset(nextLevelOffset, currentFinger.myChunkOffset + i * oneFingerSize);
            }
            else
                StoreParentOffset(nextLevelOffset, currentFinger.myChunkOffset + i * oneFingerSize);
        }

        // qDebug() << "newFinger.minKey =  " << hex << (int) newFinger.minKey << ", newFinger.maxKey = " << hex << (int) newFinger.maxKey << FN;

        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

            // write level
        localFingersStream.device()->seek(egChunkVolume * oneFingerSize + sizeof(quint64));
        localFingersStream << currentFinger.myLevel;

        memcpy(fingersChunk, fingersBA.constData(), fingersChunkSize);

        localFingersStream.device()->seek(0);
        localFingersStream >> currentFinger.minKey; // level up

        localFingersStream.device()->seek((egChunkVolume/2 - 1) * oneFingerSize + sizeof(KeyType));
        localFingersStream >> currentFinger.maxKey;

        currentFinger.itemsCount = egChunkVolume/2; // level up

        // qDebug() << "currentFinger.minKey =  " << hex << (int) currentFinger.minKey << ", currentFinger.maxKey = " << hex << (int) currentFinger.maxKey << FN;
    }


    currentFinger.nextChunkOffset = parentFinger.nextChunkOffset; // level up
    currentFinger.myOffset = parentFinger.myOffset;
    currentFinger.myChunkOffset = parentFinger.myChunkOffset;

    currentFinger.myLevel = parentFinger.myLevel;

    newFinger.myLevel = parentFinger.myLevel;

    newFinger.nextChunkOffset = fingerStream.device()->size(); // level up
    // newFingerChunkOffset = fingerStream.device()->size();

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

    newFinger.nextChunkOffset = fingerStream.device()->size();  // level up
    // newFingerChunkOffset = fingerStream.device()->size();            // current level

            // set parent back link
    if (currentFinger.myLevel == 0)
        indexChunks-> StoreFingerOffset(indexChunks->indexesChunkOffset, newFinger.nextChunkOffset);

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
