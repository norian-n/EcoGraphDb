/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2020 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

// #ifndef EG_INDEXES3_H
// #define EG_INDEXES3_H

#pragma once

#include <QDataStream>
#include <QList>
#include <QVariant>
#include <QFile>
#include <string.h>

#include "egIndexesCore.h"

template <typename KeyType> class EgFingers;

template <typename KeyType> struct EgIndexStruct
{
    KeyType indexKey;
    quint64 dataOffset;
};

template <typename KeyType> class EgIndexes
{

public:

    const int oneIndexSize   = sizeof(KeyType) + sizeof(quint64);
    const int indexChunkSize = egIndexesNamespace::egChunkVolume * oneIndexSize + sizeof(quint64) * 2 + sizeof(keysCountType) + sizeof(quint64);

    EgFingers<KeyType>* fingersTree = nullptr;    // tree to find key chunk

    KeyType theKey;             // index key to process

    quint64 oldDataOffset;      // offsets in data nodes file
    quint64 newDataOffset;

    keysCountType chunkCount;   // indexes count in the chunk for chain opers

    quint64 indexesChunkOffset; // file position for chain connect
    qint32  indexPosition;      // position in the chunk

    quint64 prevOffsetPtr;      // chunks chain operations
    quint64 nextOffsetPtr;

    EgIndexStruct<KeyType>  indexData;  // index data wrapper for flexibility

    // char* chunk;                 // current chunk buffer

    QDataStream  indexStream;       // file operations

    QByteArray   indexBA;           // chunk stream operations
    QDataStream* localStream {nullptr};

    EgIndexes()
    {
        indexBA.resize(indexChunkSize + oneIndexSize);
        localStream = new QDataStream(&indexBA, QIODevice::ReadWrite);
    }

    ~EgIndexes() { if (localStream) delete localStream; }

    typedef bool (*CompareFunctionType) (KeyType&, KeyType&);
    static bool CompareEQ (KeyType& currentIndex, KeyType& theKey) {return (currentIndex == theKey);}
    static bool CompareGT (KeyType& currentIndex, KeyType& theKey) {return (currentIndex > theKey);}
    static bool CompareGE (KeyType& currentIndex, KeyType& theKey) {return (currentIndex >= theKey);}
    static bool CompareLT (KeyType& currentIndex, KeyType& theKey) {return (currentIndex < theKey);}
    static bool CompareLE (KeyType& currentIndex, KeyType& theKey) {return (currentIndex <= theKey);}

    inline void InitMinMaxFlags();
    inline void SetMinMaxFlags();

    void InitIndexChunk();
    void StoreRootHeader(); // meta-info of indexes (first chunk for loadAll, empty chain(TBD)), also for non-zero offset

    // int StoreFingerOffset(quint64 fingerOffset);
    int StoreFingerOffset(quint64 chunkOffset, quint64 fingerOffset);   // update finger backptr

    int GetFingerOffset(quint64& fingerOffset);                 // visual tree support
    int GetChainPointers(quint64& nextPtr, quint64& prevPtr);

    inline void GetKeyByFileOffset(quint64 chunkOffset, int indexPosition, KeyType& theKey);

    int LoadIndexChunk(char *chunkPtr, quint64 chunkOffset);    // by indexesChunkOffset
    int LoadIndexChunk();        // default: loads fingersTree-> currentFinger.nextChunkOffset to indexBA.data()

    int StoreIndexChunk(const char *chunkPtr, const quint64 nextChunkOffset);
    inline int StoreIndexChunk();       // default: indexBA.constData() to fingersTree-> currentFinger.nextChunkOffset

    int FindIndexPositionToInsert();
    int InsertToIndexChunk();

    inline void ReadIndexValues(EgIndexStruct<KeyType>& indexStruct);
    inline void WriteIndexValues(EgIndexStruct<KeyType>& indexStruct, int position);

    inline void ReadIndexOnly(KeyType& currentIndex);
    inline void UpdateChunkCount(keysCountType newCount);

    inline int InsertInsideIndexChunk();

    inline void MoveTailToInsert(char* chunkPtr, int indexPosition);

    int SplitIndexChunk();
    int AppendIndexChunk();

        // load data nodes
    int FindPosByKeyFirst(CompareFunctionType myCompareFunc);
    int FindPosByKeyLast(CompareFunctionType myCompareFunc);

    void LoadAllData(QSet<quint64>& index_offsets);
    void LoadDataUp(QSet<quint64>& index_offsets);

    int LoadDataByChunkUp(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc);
    int LoadDataByChunkDown(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc);

    int LoadDataByChunkEqual(QSet<quint64>& index_offsets);

    int FindIndexByDataOffset(bool isPrimary);

    void UpdateDataOffsetForIndex(bool isPrimary = false);
    int DeleteIndex(bool isPrimary = false);

    int DeleteIndexInChunk();

    void RemoveChunkFromChain();

    inline bool checkIndexesChainFwd(quint64 &doubleSpeedOffset);     // check indexes chain for loops
    inline bool checkIndexesChainBack();

    bool checkIndexesIntegrity();  // don't use it inside operations

    void PrintIndexesChunk(char* theChunk, const QString& theMessage);  // debug
};

// #endif // EG_INDEXES3_H
