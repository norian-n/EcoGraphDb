#include "../egDataNodesType.h"
#include "egFingers.h"
#include "egIndexes.h"

using namespace egIndexes3Namespace;

template <typename KeyType> void EgFingers<KeyType>::PrintFingerInfo(egFinger<KeyType>& fingerInfo, const QString &theMessage)
{
    qDebug() << theMessage;
    qDebug() << "fingerInfo.minKey = " << hex << (int) fingerInfo.minKey;
    qDebug() << "fingerInfo.maxKey = " << hex << (int) fingerInfo.maxKey;
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

template <typename KeyType> void EgFingers<KeyType>::RemoveIndexFiles(const QString& IndexFileName)
{
    fingerFile.setFileName(IndexFileName + ".odf");
    fingerFile.remove();
}

template <typename KeyType> int EgFingers<KeyType>::OpenIndexFilesToUpdate(const QString& IndexFileName)
{

    // fingers
    fingerFile.setFileName(IndexFileName + ".odf");
    fingerStream.setDevice(&fingerFile);

    if (!fingerFile.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open finger file " << fingerFile.fileName();
        return -2;
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::OpenIndexFilesToRead(const QString& IndexFileName)
{

    // fingers
    fingerFile.setFileName(IndexFileName + ".odf");
    fingerStream.setDevice(&fingerFile);

    if (!fingerFile.exists())
    {
        qDebug() << FN << "file doesn't exist " << IndexFileName + ".odf";
        return -1;
    }

    if (!fingerFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open finger file " << fingerFile.fileName();
        return -2;
    }

    return 0;
}

template <typename KeyType> void EgFingers<KeyType>::CloseIndexFiles()
{
    fingerFile.close();
}


template <typename KeyType> inline void EgFingers<KeyType>::InitFingersChunk()
{
    QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);

    memcpy(fingersBA.data(), zeroFingersChunk, fingersChunkSize);

        // write one finger
    localFingersStream << (KeyType) indexChunks-> theKey;               // min
    localFingersStream << (KeyType) indexChunks-> theKey;               // max
    localFingersStream << (keysCountType) 1;    // count
    localFingersStream << (quint64) indexChunks-> rootHeaderSize; // indexChunks-> indexesChunkOffset; // theFinger.chunkOffset;

    // qDebug() << fingersBA.toHex() << FN;

    memcpy(fingersChunk, fingersBA.constData(), fingersChunkSize);

    // fingersChunkOffset = 0;
}



template <typename KeyType> int EgFingers<KeyType>::StoreFingersChunk(quint64 fingersChunkOffset, char* chunkPtr)
{
    fingerStream.device()->seek(fingersChunkOffset);
    fingerStream.writeRawData(chunkPtr, fingersChunkSize);

    return 0; // FIXME
}

template <typename KeyType> int EgFingers<KeyType>::StoreParentOffset(quint64 fingersChunkOffset, quint64 parentFingerOffset)
{
    fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
    fingerStream << parentFingerOffset;

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
    localFingersStream.device()->seek(theFinger.myOffset);

    localFingersStream >> theFinger.minKey;
    localFingersStream >> theFinger.maxKey;
    localFingersStream >> theFinger.itemsCount;
    localFingersStream >> theFinger.nextChunkOffset;
}

template <typename KeyType> int EgFingers<KeyType>::FindNextLevelOffsetToInsert()
{
    int chunkDirection;  // shift fingers until match

    QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);
    memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

    switch (parentFinger.itemsCount)  // currentFingersChunkHeader.fingersCount
    {
        case 0: return -1;              // no chunks to insert

        case 1: // get first
            currentFinger.myOffset = 0;
            ReadFinger(localFingersStream, currentFinger);

            return 0;
        case 2: // select closest by key
            localFingersStream.device()->seek(0); // fingersHeaderSize
            SelectClosestFingerToInsert(localFingersStream);

            return 0;
    }

    // FIXME - write tests

        // proportional finger lookup
    int posToInsert = (indexChunks-> theKey - parentFinger.minKey)*(parentFinger.itemsCount)/(parentFinger.maxKey - parentFinger.minKey);

    if ((indexChunks-> theKey < parentFinger.minKey) || (posToInsert <= 0)) // take first one
    {
        currentFinger.myOffset = 0;
        ReadFinger(localFingersStream, currentFinger);

        return 0;
    }

    if ((indexChunks-> theKey >= parentFinger.maxKey) || (posToInsert >= (parentFinger.itemsCount-1))) // take last one
    {
        currentFinger.myOffset = (parentFinger.itemsCount-1)*oneFingerSize;
        ReadFinger(localFingersStream, currentFinger);

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

            return 0;
        }
    }
    while (chunkDirection);

    return 0;
}


template <typename KeyType> int EgFingers<KeyType>::FindNextLevelOffsetFirst(bool isExact)
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

        if (isExact && (indexChunks-> theKey == localFingers[activeFinger].maxKey) && (indexChunks-> theKey > localFingers[activeFinger].minKey)) // exact match
        {
            currentFinger = localFingers[activeFinger];
            currentFinger.myOffset = posToInsert*oneFingerSize;

            // PrintFingerInfo(currentFinger, "currentFinger 2 " + FNS);

            return 0;
        }

        if ((direction < 0) && (indexChunks-> theKey < localFingers[!activeFinger].minKey) && (indexChunks-> theKey > localFingers[activeFinger].maxKey))
        {
            currentFinger = localFingers[!activeFinger];
            currentFinger.myOffset = (posToInsert+1)*oneFingerSize;
            return 1; // key between fingers
        }

        if ((direction > 0) && (indexChunks-> theKey < localFingers[activeFinger].minKey) && (indexChunks-> theKey > localFingers[!activeFinger].maxKey))
        {
            currentFinger = localFingers[activeFinger];
            currentFinger.myOffset = posToInsert*oneFingerSize;
            return 1; // key between fingers
        }
            // ???
        if ((direction < 0) && (indexChunks-> theKey > localFingers[activeFinger].maxKey) && (indexChunks-> theKey == localFingers[!activeFinger].minKey))
        {
            currentFinger = localFingers[!activeFinger];
            currentFinger.myOffset = (posToInsert+1)*oneFingerSize;

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


template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkToInsert()
{
    // PrintFingerInfo(fingersRootHeader);

    int myLevel  = fingersRootHeader.myLevel;
    parentFinger = fingersRootHeader;

    fingersChain.clear();

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

    currentFinger.myChunkOffset = parentFinger.nextChunkOffset;

        // fill fingers chain
    while (myLevel > 0)
    {
        LoadFingersChunk(parentFinger.nextChunkOffset);
        FindNextLevelOffsetToInsert();  // get currentFinger

        currentFinger.myChunkOffset = parentFinger.nextChunkOffset; // store chunk offset to update parent finger instantly

        fingersChain.append(currentFinger);

        parentFinger = currentFinger;
        myLevel--;
    }

    LoadFingersChunk(parentFinger.nextChunkOffset);
    FindNextLevelOffsetToInsert();

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    indexChunks-> indexesChunkOffset = currentFinger.nextChunkOffset;

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkFirst(bool isExact)
{
    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);

    int res = 0;

    int myLevel  = fingersRootHeader.myLevel;
    parentFinger = fingersRootHeader;

        // fill fingers chain
    while ((myLevel >= 0) && (res > -2))
    {
        // qDebug() << "myLevel = " << myLevel << FN;

        LoadFingersChunk(parentFinger.nextChunkOffset);
        res = FindNextLevelOffsetFirst(isExact);  // get currentFinger

        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        parentFinger = currentFinger;
        myLevel--;
    }

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    indexChunks-> indexesChunkOffset = currentFinger.nextChunkOffset;

    return res;
}


template <typename KeyType> int EgFingers<KeyType>::FindIndexChunkLast(bool isExact)
{
    // PrintFingerInfo(fingersRootHeader, "fingersRootHeader " + FNS);

    int res = 0;

    int myLevel  = fingersRootHeader.myLevel;
    parentFinger = fingersRootHeader;

        // fill fingers chain
    while ((myLevel >= 0) && (res > -2))
    {
        // qDebug() << "myLevel = " << myLevel << FN;

        LoadFingersChunk(parentFinger.nextChunkOffset);
        res = FindNextLevelOffsetLast(isExact);  // get currentFinger

        // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

        parentFinger = currentFinger;
        myLevel--;
    }

    // PrintFingerInfo(parentFinger, "parentFinger " + FNS);
    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);

    indexChunks-> indexesChunkOffset = currentFinger.nextChunkOffset;

    return res;
}


template <typename KeyType> int EgFingers<KeyType>::UpdateFingerAfterInsert()
{

    UpdateTheFinger(currentFinger);

    // PrintFingerInfo(currentFinger, "currentFinger " + FNS );

    fingerStream.device()->seek(parentFinger.nextChunkOffset + currentFinger.myOffset);

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
        UpdateTheFinger(fingersRootHeader, false);   // false means do not increment count

        indexChunks-> StoreRootHeader(true); // true means store min max only
    }

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateFingerAfterDelete()
{
    fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType) * 2 );
    fingerStream << indexChunks-> chunkCount;

    // qDebug() << "newKeysCount = " << (int) indexChunks-> chunkCount << FN;

    if (maxValueChanged)
    {
        fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType));
        fingerStream << newMaxValue;

        // qDebug() << "newMaxValue = " << (int) newMaxValue << FN;
    }
    else if (minValueChanged)
    {
        fingerStream.device()->seek(parentFingerOffset);
        fingerStream << newMinValue;

        // qDebug() << "newMinValue = " << (int) newMinValue << FN;
    }

    if (maxValueChanged || minValueChanged)
        UpdateFingersChainAfterDelete();

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::UpdateFingersChainAfterDelete()
{
    KeyType oldValue;

    keysCountType fingersCount;

    bool isFirstFinger;
    bool isLastFinger;

        // calc this chunk offset
    quint64 fingersChunkOffset = parentFingerOffset - ( parentFingerOffset % fingersChunkSize );

        // check first/last finger for min/max update of root finger
    keysCountType fingerNum = (parentFingerOffset - fingersChunkOffset) / oneFingerSize;

    fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType)*2);
    fingerStream >> fingersCount;

    isFirstFinger = (fingerNum == 0);
    isLastFinger  = (fingerNum == fingersCount-1);

        // get next parent finger offset
    fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
    fingerStream >> parentFingerOffset;

        // qDebug() << "parentFingerOffset = " << parentFingerOffset << FN;

    while (parentFingerOffset) // not root
    {

        fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType)*2);
        fingerStream >> fingersCount;

        if (maxValueChanged && isLastFinger)
        {
            fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType));
            fingerStream >> oldValue;

            if (newMaxValue < oldValue)
            {
                fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType));
                fingerStream << newMaxValue;
            }
            else
            {
                maxValueChanged = false;
                break;
            }

            // qDebug() << "newMaxValue = " << newMaxValue << FN;
        }
        else if (minValueChanged && isFirstFinger)
        {
            fingerStream.device()->seek(parentFingerOffset);
            fingerStream >> oldValue;

            if (newMinValue > oldValue)
            {
                fingerStream.device()->seek(parentFingerOffset);
                fingerStream << newMinValue;
            }
            else
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
            // calc this chunk offset
        fingersChunkOffset = parentFingerOffset - ( parentFingerOffset % fingersChunkSize );
        fingerNum = (parentFingerOffset - fingersChunkOffset) / oneFingerSize;

        fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType)*2);
        fingerStream >> fingersCount;

        isFirstFinger = (fingerNum == 0);
        isLastFinger  = (fingerNum == (fingersCount-1));

            // get next parent finger offset
        fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
        fingerStream >> parentFingerOffset;
    }

        // update root header
    if ((maxValueChanged && isLastFinger) || (minValueChanged && isFirstFinger))
    {
        if (maxValueChanged && isLastFinger)
            fingersRootHeader.maxKey =  newMaxValue;
        else if (minValueChanged && isFirstFinger)
            fingersRootHeader.minKey =  newMinValue;

        indexChunks-> indexStream.device()->seek(0);

        indexChunks-> indexStream << fingersRootHeader.minKey;
        indexChunks-> indexStream << fingersRootHeader.maxKey;
    }

        // reset
    maxValueChanged = false;
    minValueChanged = false;

    return 0;
}

template <typename KeyType> bool EgFingers<KeyType>::DeleteSpecificFinger()
{
    keysCountType fingersCount;

    bool isFirstFinger;
    bool isLastFinger;

    // calc this chunk offset
    quint64 fingersChunkOffset = parentFingerOffset - ( parentFingerOffset % fingersChunkSize );

    // qDebug() << "parentFingerOffset = " << parentFingerOffset << FN;

    // check first/last finger for min/max update
    keysCountType fingerNum = (parentFingerOffset - fingersChunkOffset) / oneFingerSize;

    fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType)*2);
    fingerStream >> fingersCount;

        // update count
    fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType)*2);
    fingerStream >> fingersCount;

    isFirstFinger = (fingerNum == 0);
    isLastFinger  = (fingerNum == fingersCount-1);

        // FIXME

    if (fingersCount > 1)
    {
            // load chunk
        LoadFingersChunk(fingersChunkOffset);
        // memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        // PrintFingersChunk(fingersChunk, "fingers chunk before delete " + FNS);

        // move tail if required
        if (! isLastFinger)
        {
            memmove (fingersChunk + fingerNum*oneFingerSize, fingersChunk + (fingerNum+1)*oneFingerSize,  oneFingerSize*(fingersCount - fingerNum)); // + 1
        }
            // write zeroes to last and decrease count
        memmove (fingersChunk + oneFingerSize*(fingersCount - 1), zeroFingersChunk,  oneFingerSize);

        // PrintFingersChunk(fingersChunk, "fingers chunk after delete " + FNS);

            // store chunk
        StoreFingersChunk(fingersChunkOffset,fingersChunk);

            // TODO - update min/max


    }
    else   // write zeroes
    {
        StoreFingersChunk(fingersChunkOffset, zeroFingersChunk);
    }

    // update parent

    fingerStream.device()->seek(fingersChunkOffset + egChunkVolume * oneFingerSize);
    fingerStream >> parentFingerOffset;

    if (parentFingerOffset)
    {
        fingerStream.device()->seek(parentFingerOffset + sizeof(KeyType)*2);
        fingerStream << (keysCountType)(fingersCount - 1);
    }

    return (fingersCount <= 1); // TODO process < 1
}

template <typename KeyType> void EgFingers<KeyType>::DeleteFinger()
{
    bool isOnlyFinger = true; // stub

    while (parentFingerOffset && isOnlyFinger) // not root and to update
    {
        isOnlyFinger = DeleteSpecificFinger(); // parentFingerOffset updated
    }

    if (parentFingerOffset)
        UpdateFingersChainAfterDelete(); // FIXME

}

template <typename KeyType> int EgFingers<KeyType>::UpdateFingersChainAfterSplit(bool appendMode)
{
    bool splitParentChunk;

    QDataStream localFingersStream(&fingersBA, QIODevice::ReadWrite);

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
    while ((! fingersChain.isEmpty()) && splitParentChunk)
    {
            // shift parent up
        fingersChain.removeLast();

        if (! fingersChain.isEmpty())
            parentFinger = fingersChain.last();
        else
            parentFinger = fingersRootHeader;

        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

        LoadFingersChunk(parentFinger.nextChunkOffset);
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        if (parentFinger.itemsCount < egChunkVolume)
        {
            InsertSplittedFinger(localFingersStream);     // if have space just add new item for new chunk and recalc min/max
            splitParentChunk = false; 
        }
        else
        {
            if (appendMode)
                AppendFingersChunk(localFingersStream);
            else
                SplitFingersChunk(localFingersStream);

            splitParentChunk = true;
        }

        // qDebug() << "newFinger.nextChunkOffset = " << hex << newFinger.nextChunkOffset << FN;

        StoreParentOffset(newFinger.nextChunkOffset, newFingerOffset);
    }

    if (splitParentChunk) // new root chunk is required
    {
            // shift up
        fingersRootHeader.nextChunkOffset = fingerStream.device()->size();
        AddNewRootChunk(localFingersStream);

        StoreParentOffset(currentFinger.nextChunkOffset, fingersRootHeader.nextChunkOffset); // FIXME CHECK
        StoreParentOffset(newFinger.nextChunkOffset, fingersRootHeader.nextChunkOffset + oneFingerSize);

        UpdateTheFinger(fingersRootHeader, false);   // false means do not increment count

        fingersRootHeader.myLevel++;
        fingersRootHeader.itemsCount = 2; // new top chunk

        indexChunks-> StoreRootHeader();
    }
    else
    {
        if (! fingersChain.isEmpty())
        {
            // PrintFingerInfo(currentFinger);

            UpdateFingersChainAfterInsert();
        }
        else // update root only
        {
            UpdateTheFinger(fingersRootHeader);

            indexChunks-> StoreRootHeader();
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
    memcpy(fingersBA.data(), zeroFingersChunk, fingersChunkSize);

    localFingersStream.device()->seek(0);

    WriteFinger(localFingersStream, currentFinger);     // update current finger
    WriteFinger(localFingersStream, newFinger);         // add new finger

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

template <typename KeyType> int EgFingers<KeyType>::InsertSplittedFinger(QDataStream &localFingersStream)
{
    posToInsert = currentFinger.myOffset/oneFingerSize + 1; // FIXME - insert to left

    // PrintFingerInfo(currentFinger, "currentFinger " + FNS);
    // PrintFingerInfo(newFinger, "newFinger " + FNS);

    // qDebug() << "posToInsert = " << posToInsert << ", fingersCount = " << parentFinger.itemsCount << FN;

        // check if not last position, move tail
    if (posToInsert != parentFinger.itemsCount) // last one
    {
        // qDebug() << "posToInsert = " << posToInsert << ", fingersCount = " << parentFinger.itemsCount << FN;
        memmove (fingersChunk + (posToInsert+1)*oneFingerSize, fingersChunk + posToInsert*oneFingerSize,  oneFingerSize*(parentFinger.itemsCount - posToInsert));
        // PrintFingersChunk(fingersChunk, "fingers chunk after memmove " + FNS);
    }

    memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

    localFingersStream.device()->seek(currentFinger.myOffset);

    WriteFinger(localFingersStream, currentFinger);     // update current finger
    WriteFinger(localFingersStream, newFinger);         // add new finger

    // PrintFingersChunk(fingersBA.data(), "fingers chunk after split/append " + FNS);

    StoreFingersChunk(parentFinger.nextChunkOffset, fingersBA.data());

    newFingerOffset = parentFinger.nextChunkOffset + posToInsert * oneFingerSize; // current level for back link

        // update parent finger
    if (! fingersChain.isEmpty()) // parent is not a root
    {
        // PrintFingerInfo(parentFinger, "parentFinger " + FNS);

        LoadFingersChunk(parentFinger.myChunkOffset);
        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        // qDebug() << "fingersChunk" << fingersBA.toHex() << FN;

        UpdateTheFinger(parentFinger);

        localFingersStream.device()->seek(parentFinger.myOffset);
        WriteFinger(localFingersStream, parentFinger);

        StoreFingersChunk(parentFinger.myChunkOffset, fingersBA.data());
    }

    return 0;
}


template <typename KeyType> int EgFingers<KeyType>::SplitFingersChunk(QDataStream &localFingersStream)
{
    KeyType minKey, maxKey;

    posToInsert = currentFinger.myOffset/oneFingerSize + 1;

    // qDebug() << "initialPosToInsert = " << posToInsert << ", fingersCount = " << currentFinger.fingersCount << FN;

        // init new chunk
    memmove (newFingersChunk, zeroFingersChunk,  egChunkVolume*oneFingerSize);

        // split chunks    
    memmove (newFingersChunk, fingersChunk + egChunkVolume*oneFingerSize/2,  egChunkVolume*oneFingerSize/2);
    // PrintFingersChunk(newFingersChunk, "new chunk after split up " + FNS);
        // add zeroes to first
    memmove (fingersChunk + egChunkVolume*oneFingerSize/2, zeroFingersChunk,  egChunkVolume*oneFingerSize/2);
    // PrintFingersChunk(fingersChunk, "old chunk after split up " + FNS);

        // recalc position to splitted chunks
    if ( posToInsert <= egChunkVolume/2) // insert to first part
    {
        // check if not last position, move tail
        if (posToInsert != (egChunkVolume/2)) // not last one
            memmove (fingersChunk + (posToInsert+1)*oneFingerSize, fingersChunk + posToInsert*oneFingerSize,  oneFingerSize*(egChunkVolume/2 - posToInsert + 1));
        // PrintFingersChunk(fingersChunk, "old chunk after memmove " + FNS);

        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

            // write new finger
        localFingersStream.device()->seek((posToInsert-1)*oneFingerSize);
        WriteFinger(localFingersStream, currentFinger);
        WriteFinger(localFingersStream, newFinger);

        newFingerOffset = parentFinger.nextChunkOffset +  posToInsert*oneFingerSize; // current level

        memcpy(fingersChunk, fingersBA.constData(), fingersChunkSize);

        // PrintFingersChunk(fingersChunk, "old chunk after all " + FNS);

        localFingersStream.device()->seek(0);
        localFingersStream >> minKey;

        localFingersStream.device()->seek(egChunkVolume/2 * oneFingerSize + sizeof(KeyType));
        localFingersStream >> maxKey;

            // update parent fingers
        currentFinger.itemsCount = egChunkVolume/2+1; // level up
        currentFinger.minKey = minKey; // get first index
        currentFinger.maxKey = maxKey; // get last index

        // qDebug() << "currentFinger.minKey =  " << hex << (int) currentFinger.minKey << ", currentFinger.maxKey = " << hex << (int) currentFinger.maxKey << FN;

        memcpy(fingersBA.data(), newFingersChunk, fingersChunkSize);

        localFingersStream.device()->seek(0);
        localFingersStream >> minKey;

        localFingersStream.device()->seek((egChunkVolume/2 - 1) * oneFingerSize + sizeof(KeyType));
        localFingersStream >> maxKey;

        newFinger.itemsCount = egChunkVolume/2; // level up
        newFinger.minKey = minKey; // get first index
        newFinger.maxKey = maxKey; // get last index

        // qDebug() << "newFinger.minKey =  " << hex << (int) newFinger.minKey << ", newFinger.maxKey = " << hex << (int) newFinger.maxKey << FN;

    }
    else // insert to second Part
    {
        posToInsert -= egChunkVolume/2;

        // check if not last position, move tail
        if (posToInsert != (egChunkVolume/2)) // last one
        {
            // qDebug() << "posToInsert = " << posToInsert << ", itemsCount = " << currentFinger.itemsCount << FN;
            memmove (newFingersChunk + (posToInsert+1)*oneFingerSize, newFingersChunk + posToInsert*oneFingerSize,  oneFingerSize*(egChunkVolume/2 - posToInsert + 1));
            // PrintFingersChunk(newFingersChunk, "new chunk after memmove " + FNS);
        }

        memcpy(fingersBA.data(), newFingersChunk, fingersChunkSize);

        // write index key and offset
        localFingersStream.device()->seek((posToInsert-1)*oneFingerSize);
        WriteFinger(localFingersStream, currentFinger);
        WriteFinger(localFingersStream, newFinger);
        // qDebug() << "newFingersChunk after all " << fingersBA.toHex() << FN;

        newFingerOffset = fingerStream.device()->size() +  posToInsert*oneFingerSize; // current level

        memcpy(newFingersChunk, fingersBA.constData(), fingersChunkSize);

        // PrintFingersChunk(newFingersChunk, "new chunk after all " + FNS);

            // update parent fingers
        localFingersStream.device()->seek(0);
        localFingersStream >> minKey;

        localFingersStream.device()->seek(egChunkVolume/2 * oneFingerSize + sizeof(KeyType));
        localFingersStream >> maxKey;

        newFinger.itemsCount = egChunkVolume/2+1; // level up
        newFinger.minKey = minKey; // get first index
        newFinger.maxKey = maxKey; // get last index

        // qDebug() << "newFinger.minKey =  " << hex << (int) newFinger.minKey << ", newFinger.maxKey = " << hex << (int) newFinger.maxKey << FN;

        memcpy(fingersBA.data(), fingersChunk, fingersChunkSize);

        localFingersStream.device()->seek(0);
        localFingersStream >> minKey;

        localFingersStream.device()->seek((egChunkVolume/2 - 1) * oneFingerSize + sizeof(KeyType));
        localFingersStream >> maxKey;

        currentFinger.itemsCount = egChunkVolume/2; // level up
        currentFinger.minKey = minKey; // get first index
        currentFinger.maxKey = maxKey; // get last index

        // qDebug() << "currentFinger.minKey =  " << hex << (int) currentFinger.minKey << ", currentFinger.maxKey = " << hex << (int) currentFinger.maxKey << FN;
    }

    newFinger.nextChunkOffset = fingerStream.device()->size(); // level up

    currentFinger.nextChunkOffset = parentFinger.nextChunkOffset; // level up
    currentFinger.myOffset = parentFinger.myOffset;
    currentFinger.myChunkOffset = parentFinger.myChunkOffset;

    // qDebug() << "parentFinger.myOffset = " << parentFinger.myOffset << ", parentFinger.myChunkOffset = " << parentFinger.myChunkOffset << FN;

        // save both chunks, fingers already updated to upper level
    StoreFingersChunk(parentFinger.nextChunkOffset, fingersChunk);
    StoreFingersChunk(fingerStream.device()->size(), newFingersChunk);

    return 0;
}

template <typename KeyType> int EgFingers<KeyType>::AppendFingersChunk(QDataStream &localFingersStream)
{
        // init new chunk
    memmove (newFingersChunk, zeroFingersChunk,  fingersChunkSize);

    memcpy(fingersBA.data(), newFingersChunk, fingersChunkSize);

        // write new finger
    localFingersStream.device()->seek(0);
    WriteFinger(localFingersStream, newFinger);

    newFinger.nextChunkOffset = fingerStream.device()->size();  // level up
    newFingerOffset = fingerStream.device()->size();            // current level

    currentFinger = parentFinger;   // level up

        // save new chunk, fingers already updated to upper level
    StoreFingersChunk(fingerStream.device()->size(), fingersBA.data());

    return 0;
}


template class EgFingers<qint32>; // for unit tests