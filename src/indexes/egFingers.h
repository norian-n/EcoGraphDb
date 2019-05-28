/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_FINGERS_H
#define EG_FINGERS_H

#include <QList>
#include <QVariant>
#include <QFile>
#include <string.h>

#include "egIndexesCore.h"

template <typename KeyType> class EgIndexes;

template <typename KeyType> class EgFingers // Data Objects link / autolink support
{
public:

    const int rootHeaderSize = sizeof(KeyType) * 2 + sizeof(fingersLevelType) + sizeof(keysCountType) + sizeof(quint64);
    const int oneFingerSize = sizeof(KeyType) * 2 + sizeof(keysCountType) + sizeof(quint64);  // last is next chunk offset
    const int fingersChunkSize = egIndexesNamespace::egChunkVolume * oneFingerSize + sizeof(quint64) + sizeof(fingersLevelType); // parent chunk offset

    EgIndexes<KeyType>* indexChunks {nullptr};  // set by IndexesFiles interface class

    QString IndexFileName; // copy for debug messages

    int posToInsert;

    KeyType newMinValue;
    KeyType newMaxValue;

    // keysCountType newKeysCount;

    bool minValueChanged;
    bool maxValueChanged;

    char* fingersChunk;     // current fingers chunk buffer
    char* zeroFingersChunk; // filled with 0
    char* newFingersChunk;  // new chunk buffer to split

    quint64 parentFingerOffset;
    quint64 currentFingerOffset;

    // bool fingerIsMoved;             // index chunk changed

    quint64 fingersChunkOffset;

    keysCountType currentKeysCount;

    egFinger<KeyType> currentFinger;
    egFinger<KeyType> newFinger;         // to split chunks
    egFinger<KeyType> parentFinger;

    egFinger<KeyType> rootFinger;

    QList < egFinger<KeyType> > fingersChain;

    QDataStream fingerStream;           // file operations

    QByteArray   fingersBA;             // chunk stream operations
    QDataStream* localStream {nullptr};

    EgFingers():
         fingersChunk(new char[fingersChunkSize])
        ,zeroFingersChunk(new char[fingersChunkSize])
        ,newFingersChunk(new char[fingersChunkSize])
    {
        memset(zeroFingersChunk, 0, fingersChunkSize);
        fingersBA.resize(fingersChunkSize+oneFingerSize);
        localStream = new QDataStream(&fingersBA, QIODevice::ReadWrite);
    }


    ~EgFingers() { if (localStream) delete localStream; if (zeroFingersChunk) delete[] zeroFingersChunk; if (newFingersChunk) delete[] newFingersChunk; if (fingersChunk) delete[] fingersChunk;}

    void InitRootFinger();

    void LoadRootFinger();              // meta-info of fingers tree, also for non-zero offset
    void StoreRootFinger();
    void StoreRootFingerMinMax();

    void InitFingersChunk();

    inline int LoadFingersChunk(); // default: to fingersBA.data() from parentFinger.nextChunkOffset
    int LoadFingersChunk(char* chunkPtr, const quint64 fingersChunkOffset);

    int StoreFingersChunk(quint64 fingersChunkOffset, char* chunkPtr);

    // int FindIndexChunkToInsert();
    // int FindNextLevelOffsetToInsert();
    // int SelectClosestFingerToInsert(QDataStream &localFingersStream);

    int FindIndexChunkToInsert();

    inline int FindFingerInChunkToInsert();

    int  GetFingerByOffset(quint64 updatedFingerOffset);    // get by updatedFingerOffset if fingerIsMoved by indexes

    int UpdateCurrentFingerAfterInsert();

    int UpdateFingersChainAfterInsert();    
    int UpdateFingersChainAfterSplit();

    int UpdateFingersChainAfterDelete(); // FIXME check if needed

    int UpdateFingerCountAfterDelete(keysCountType newKeysCount);

    int UpdateMinValueUp();
    int UpdateMaxValueUp();

    int  DeleteParentFinger2();
    int  DeleteParentFinger();

    void DeleteSpecificFinger2(keysCountType keysCount);
    void DeleteSpecificFinger();
    void DeleteFingersChunk(quint64 fingersChunkOffset);

    void ReadFinger  (QDataStream &localFingersStream, egFinger<KeyType>& theFinger);
    inline void WriteFinger (QDataStream &localFingersStream, egFinger<KeyType>& theFinger);

    inline void UpdateTheFingerMinMax(egFinger<KeyType>& theFinger);

    // int InsertSplittedFinger(QDataStream &localFingersStream);
    // int SplitFingersChunk(QDataStream &localFingersStream);
    void AddNewRootChunk();

    inline void MoveTailToInsert(char* chunkPtr, int fingerPosition, int fingersToMove);

    int InsertSplittedFinger();

    int SplitFingersChunk2();

    int SplitFingersChunk(QDataStream &localFingersStream);
    int InsertNewFinger(int posToInsert, int itemsCount);
    int UpdateBackptrOffsets(quint64 myChunkOffset,  int posToInsert, int itemsCount, fingersLevelType myLocalLevel);

    void UpdateMinMax(egFinger<KeyType>& theFinger);

    int AppendFingersChunk();

    int FindIndexChunkFirst(bool isExactEqual); // CompareFunctionType myCompareFunc
    int FindNextLevelOffsetFirst(bool isExactEqual);

    int FindIndexChunkLast(bool isExactEqual); // CompareFunctionType myCompareFunc
    // int FindNextLevelOffsetLast(bool isExact);
    int FindNextLevelOffsetLast(bool isExactEqual);

    int StoreParentOffset(quint64 fingersChunkOffset, quint64 parentFingerOffset);
    int GetParentOffset(QDataStream &localFingersStream, quint64& parentFingerOffset); // debug

    // debug

    void PrintFingerInfo(egFinger<KeyType>& fingerInfo, const QString &theMessage);
    void PrintFingersChunk(char* theFingersChunk, const QString& theMessage);
    void PrintChunkInfo(quint64 fingersChunkOffset);

    void PrintAllChunksInfo(const QString& theMessage);

};

#endif // EG_FINGERS_H
