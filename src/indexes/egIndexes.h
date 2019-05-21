/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_INDEXES3_H
#define EG_INDEXES3_H

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

    void InitIndexChunk();
    void StoreRootHeader(); // meta-info of indexes (first chunk for loadAll, empty chain(TBD)), also for non-zero offset

    // int StoreFingerOffset(quint64 fingerOffset);
    int StoreFingerOffset(quint64 chunkOffset, quint64 fingerOffset);   // update finger backptr

    int GetFingerOffset(quint64& fingerOffset);                 // visual tree support
    int GetChainPointers(quint64& fwdPtr, quint64& backPtr);

    int LoadIndexChunk(char *chunkPtr, quint64 chunkOffset);    // by indexesChunkOffset
    int LoadIndexChunk();        // default: loads fingersTree-> currentFinger.nextChunkOffset to indexBA.data()

    int StoreIndexChunk(const char *chunkPtr, const quint64 nextChunkOffset);
    inline int StoreIndexChunk();       // default: indexBA.constData() to fingersTree-> currentFinger.nextChunkOffset

    int FindIndexPositionToInsert(QDataStream &localIndexesStream);
    int InsertToIndexChunk();

    inline void ReadIndexValues(QDataStream &localIndexesStream, EgIndexStruct<KeyType>& indexStruct);
    inline void WriteIndexValues(QDataStream &localIndexesStream, EgIndexStruct<KeyType>& indexStruct, int position);

    inline void ReadIndexOnly(QDataStream &localIndexesStream, KeyType& currentIndex);
    inline void UpdateChunkCount(QDataStream &localIndexesStream, keysCountType newCount);

    inline int InsertInsideIndexChunk(QDataStream &localIndexesStream);

    inline void MoveTailToInsert(char* chunkPtr, int indexPosition);

    int SplitIndexChunk(QDataStream &localIndexStream);
    int AppendIndexChunk(QDataStream &localIndexStream);

        // load data nodes
    int FindPosByKeyFirst(QDataStream &localIndexesStream, CompareFunctionType myCompareFunc);
    int FindPosByKeyLast(QDataStream &localIndexesStream, CompareFunctionType myCompareFunc);

    void LoadAllData(QSet<quint64>& index_offsets);

    void LoadDataUp(QSet<quint64>& index_offsets, QDataStream &localIndexStream);

    void LoadDataByChunkUp(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc);
    int LoadDataByChunkDown(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc);

    void LoadDataByChunkEqual(QSet<quint64>& index_offsets);

    int FindIndexByDataOffset(QDataStream &localIndexStream, bool isPrimary);

    void UpdateIndex(bool isPrimary = false);
    int DeleteIndex(bool isPrimary = false);

    int UpdateDataOffset();
    int DeleteDataOffset(QDataStream &localIndexStream);

    void RemoveChunkFromChain();

    void PrintIndexesChunk(char* theChunk, const QString& theMessage);  // debug
};

#endif // EG_INDEXES3_H
