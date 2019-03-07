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

    EgFingers<KeyType>* fingersTree = nullptr;    // tree to find key chunk

    KeyType theKey;             // index key to process

    quint64 oldDataOffset;      // offsets in data nodes file
    quint64 newDataOffset;

    keysCountType chunkCount;   // real indexes in the chunk

    quint64 indexesChunkOffset; // file position
    qint32  indexPosition;      // position in the chunk

    quint64 prevOffsetPtr;      // chunks chain operations
    quint64 nextOffsetPtr;

    int oneIndexSize;           // KeyType deendent consts
    int indexChunkSize;

    EgIndexStruct<KeyType>* indexPtr = nullptr;

    char* chunk;            // current chunk buffer
    char* zero_chunk;       // filled with 0
    char* new_chunk;        // new chunk buffer to split

        // files and streams
    QFile indexFile;
    QDataStream indexStream;

    QByteArray indexBA;

    EgIndexes():
         oneIndexSize(sizeof(KeyType) + sizeof(quint64)) // tail: chain neighbors offsets (prev, next), count, parent finger offset
        ,indexChunkSize((egIndexesNamespace::egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2) + sizeof(keysCountType) + sizeof(quint64))          
        ,chunk(new char[indexChunkSize])
        ,zero_chunk(new char[indexChunkSize])
        ,new_chunk(new char[indexChunkSize])
    {
        memset(zero_chunk, 0, indexChunkSize);
        indexBA.resize(indexChunkSize);
    }

    ~EgIndexes() { if (zero_chunk) delete[] zero_chunk; if (chunk) delete[] chunk; if (new_chunk) delete[] new_chunk;}
                 // qDebug()  << "EgIndexes destructor called ";}

    typedef bool (*CompareFunctionType) (KeyType&, KeyType&);
    static bool CompareEQ (KeyType& currentIndex, KeyType& theKey) {return (currentIndex == theKey);}
    static bool CompareGT (KeyType& currentIndex, KeyType& theKey) {return (currentIndex > theKey);}
    static bool CompareGE (KeyType& currentIndex, KeyType& theKey) {return (currentIndex >= theKey);}
    static bool CompareLT (KeyType& currentIndex, KeyType& theKey) {return (currentIndex < theKey);}
    static bool CompareLE (KeyType& currentIndex, KeyType& theKey) {return (currentIndex <= theKey);}

    void PrintIndexesChunk(char* theChunk, const QString& theMessage);  // debug

    int OpenIndexFilesToUpdate(const QString& IndexFileName);
    int OpenIndexFilesToRead(const QString& IndexFileName);
    int OpenIndexFileToCheck(const QString& IndexFilePath); // debug

    void CloseIndexFiles();
    void RemoveIndexFiles(const QString& IndexFileName);

    void InitIndexChunk();

    void InitRootHeader(); // meta-info of indexes (first chunk for loadAll, empty chain(TBD)), also for non-zero offset
    void LoadRootHeader();
    void StoreRootHeader();

    // int StoreFingerOffset(quint64 fingerOffset);
    int StoreFingerOffset(quint64 chunkOffset, quint64 fingerOffset);   // update finger backptr

    void LoadIndexChunk(char *chunkPtr);    // by indexesChunkOffset
    void LoadIndexChunk();                  // loads to indexBA.data()

    int StoreIndexChunk(const char *chunkPtr);
    // int StoreIndexChunk(quint64 chunkOffset, char* chunkPtr);

    int FindIndexPositionToInsert(QDataStream &localIndexesStream);
    int InsertToIndexChunk();

    inline void ReadIndexValues(QDataStream &localIndexesStream, EgIndexStruct<KeyType>& indexStruct);
    inline void WriteIndexValues(QDataStream &localIndexesStream, EgIndexStruct<KeyType>& indexStruct);

    inline void ReadIndexOnly(QDataStream &localIndexesStream, KeyType& currentIndex);
    inline void UpdateChunkCount(QDataStream &localIndexesStream, keysCountType newCount);

    int InsertInsideIndexChunk(QDataStream &localIndexesStream);

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
    void DeleteIndex(bool isPrimary = false);

    int UpdateDataOffset(QDataStream &localIndexStream);
    int DeleteDataOffset(QDataStream &localIndexStream);

    void RemoveChunkFromChain();
};

#endif // EG_INDEXES3_H
