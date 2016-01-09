#ifndef EG_INDEXES3_H
#define EG_INDEXES3_H

#include <QDataStream>
#include <QList>
#include <QVariant>
#include <QFile>
#include <string.h>

#include "egIndexesCore.h"

template <typename KeyType> class EgFingers;

template <typename KeyType> class EgIndexes
{
public:

    EgFingers<KeyType> * fingersTree;

    KeyType theKey;             // current index key
    // quint64 data_offset;        // current index value - offset in data file
    // quint64 new_offset;         // offset in data file for updated objects

    qint32  indexPosition;
    quint64 oldDataOffset;
    quint64 newDataOffset;

    keysCountType chunkCount;

    quint64 indexesChunkOffset; // file position
/*
    egFinger<KeyType> fingersRootHeader; // (!) stored in the indexes file
    egFinger<KeyType> currentFinger;
    egFinger<KeyType> newFinger;         // to split chunks
*/

    int rootHeaderSize;
    int oneIndexSize;
    int indexChunkSize;

    char* chunk;            // current chunk buffer
    char* zero_chunk;       // filled with 0
    char* new_chunk;        // new chunk buffer to split

        // files and streams
    QFile indexFile;
    QDataStream indexStream;

    QByteArray indexBA;

    EgIndexes():
         rootHeaderSize(sizeof(KeyType) * 2 + sizeof(fingersLevelType) + sizeof(keysCountType) + sizeof(quint64))
        ,oneIndexSize(sizeof(KeyType) + sizeof(quint64))
        ,indexChunkSize((egIndexes3Namespace::egChunkVolume * oneIndexSize) + (sizeof(quint64) * 2) + sizeof(keysCountType) + sizeof(quint64))
            // chain neighbors offsets, count, parent chunk offset
        ,chunk(new char[indexChunkSize])
        ,zero_chunk(new char[indexChunkSize])
        ,new_chunk(new char[indexChunkSize])
    {
        memset(zero_chunk, 0, indexChunkSize);
        indexBA.resize(indexChunkSize);
    }

    ~EgIndexes() { if (zero_chunk) delete[] zero_chunk; if (chunk) delete[] chunk; if (new_chunk) delete[] new_chunk;}

    typedef bool (*CompareFunctionType) (KeyType&, KeyType&);
    static bool CompareEQ (KeyType& currentIndex, KeyType& theKey) {return (currentIndex == theKey);}
    static bool CompareGT (KeyType& currentIndex, KeyType& theKey) {return (currentIndex > theKey);}
    static bool CompareGE (KeyType& currentIndex, KeyType& theKey) {return (currentIndex >= theKey);}
    static bool CompareLT (KeyType& currentIndex, KeyType& theKey) {return (currentIndex < theKey);}
    static bool CompareLE (KeyType& currentIndex, KeyType& theKey) {return (currentIndex <= theKey);}

    void PrintIndexesChunk(char* theChunk, const QString& theMessage);

    int OpenIndexFilesToUpdate(const QString& IndexFileName);
    int OpenIndexFilesToRead(const QString& IndexFileName);

    void CloseIndexFiles();
    void RemoveIndexFiles(const QString& IndexFileName);

    void InitIndexChunk();
    void InitRootHeader();

    void LoadRootHeader();              // meta-info of fingers tree, stored into indexes file for not-zero offset
    void StoreRootHeader(bool minMaxOnly = false);

    int StoreFingerOffset(quint64 fingerOffset);

    void LoadIndexChunk(char *chunkPtr);
    int StoreIndexChunk(char* chunkPtr);

    int FindIndexPosition(QDataStream &localIndexesStream);
    int InsertToIndexChunk();

    int SplitIndexChunk(QDataStream &localIndexStream);
    int AppendIndexChunk(QDataStream &localIndexStream);

        // load data nodes
    int FindPosByKeyFirst(QDataStream &localIndexesStream, CompareFunctionType myCompareFunc);
    int FindPosByKeyLast(QDataStream &localIndexesStream, CompareFunctionType myCompareFunc);

    void LoadDataByChunkUp(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc);
    void LoadDataByChunkDown(QSet<quint64>& index_offsets, CompareFunctionType myCompareFunc);

    void LoadDataByChunkEqual(QSet<quint64>& index_offsets);

    int FindIndexByDataOffset(QDataStream &localIndexStream);

    void UpdateIndex();
    void DeleteIndex();

    int UpdateDataOffset(QDataStream &localIndexStream);
    int DeleteDataOffset(QDataStream &localIndexStream);

    void RemoveChunkFromChain();
};

#endif // EG_INDEXES3_H
