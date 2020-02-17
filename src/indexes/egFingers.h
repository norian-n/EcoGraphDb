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

    bool minValueChanged;
    bool maxValueChanged;

    char* fingersChunk;     // current fingers chunk buffer

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
    {
        fingersBA.resize(fingersChunkSize+oneFingerSize);
        localStream = new QDataStream(&fingersBA, QIODevice::ReadWrite);
    }

    ~EgFingers() { if (localStream) delete localStream; if (fingersChunk) delete[] fingersChunk;}

        // init, load, store ops
    void InitRootFinger();
    void InitFingersChunk();

    void LoadRootFinger();          // meta-info of fingers tree, also for non-zero offset
    void StoreRootFinger();
    void StoreRootFingerMinMaxOnly();

    int LoadFingersChunk();         // default: to fingersBA.data() from parentFinger.nextChunkOffset
    int StoreFingersChunk(quint64 fingersChunkOffset, char* chunkPtr);

        // insert ops
    inline void GetChunkLevel(); // to currentFinger.myLevel

    int FindIndexChunkToInsert();

    int AppendFingersChunk();
    int SplitFingersChunk();

    inline void AddNewSubRootChunk();

    inline void MoveTailToInsert(char* chunkPtr, int fingerPosition, int fingersToMove);
    int InsertSplittedFinger();
    int InsertNewFinger(int posToInsert, int itemsCount);

    inline void GetFingerByOffset(quint64 updatedFingerOffset);    // get by updatedFingerOffset if fingerIsMoved by indexes
    int StoreParentOffset(quint64 fingersChunkOffset, quint64 parentFingerOffset);

    int UpdateCurrentFingerAfterInsert();

    int UpdateFingersChainAfterInsert();    
    int UpdateFingersChainAfterSplit();

    int UpdateFingersChainAfterDelete(); // FIXME check if needed

    int UpdateFingerCountAfterDelete(keysCountType newKeysCount);

    int UpdateMinValueUp();
    int UpdateMaxValueUp();

        // delete ops
    inline void DeleteSpecificFinger(keysCountType keysCount);
    int  DeleteParentFinger();

    void DeleteFingersChunk(quint64 fingersChunkOffset);

        // basic finger ops
    void ReadFinger (egFinger<KeyType>& theFinger, const int fingerPosition);
    inline void WriteFinger (QDataStream &localFingersStream, egFinger<KeyType>& theFinger); 

    inline void UpdateTheFingerMinMax(egFinger<KeyType>& theFinger);

        // updates
    int UpdateBackptrOffsets(quint64 myChunkOffset,  int posToInsert, int itemsCount, fingersLevelType myLocalLevel);
    void UpdateMinMax(egFinger<KeyType>& theFinger);

        // lookups
    void FindFingerGE();
    int  FindIndexChunkGE(); // first finger for key greater or equal then min value

    void FindFingerGT(); // first finger for key greater then its min value
    int  FindIndexChunkGT();

    void FindFingerLE(); // last finger for key less or equal then max value
    int  FindIndexChunkLE();

    void FindFingerLT(); // last finger for key less then its max value
    int  FindIndexChunkLT();

    int FindFingerEQ(); // first finger for key greater then its min value
    int FindIndexChunkEQ(); // first EQ fnger on index chunk (0 level)

        // debug
    void PrintFingerInfo(egFinger<KeyType>& fingerInfo, const QString &theMessage);
    void PrintFingersChunk(char* theFingersChunk, const QString& theMessage);
    void PrintChunkInfo(quint64 fingersChunkOffset);

    void PrintAllChunksInfo(const QString& theMessage);

};

#endif // EG_FINGERS_H
