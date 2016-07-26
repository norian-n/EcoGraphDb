/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
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

    // typedef bool (*CompareFunctionType) (KeyType&, KeyType&);
    // static bool CompareEQ (KeyType& key1, KeyType& key2) {return (key1 == key2);}

    EgIndexes<KeyType> * indexChunks;

    QString IndexFileName;

    // quint64 fingersChunkOffset; // finger file position

    // int rootHeaderSize;
    int rootHeaderSize;
    int oneFingerSize;
    int fingersChunkSize;

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

    keysCountType currentKeysCount;

    egFinger<KeyType> currentFinger;
    egFinger<KeyType> newFinger;         // to split chunks
    egFinger<KeyType> parentFinger;
    egFinger<KeyType> fingersRootHeader;

    QList < egFinger<KeyType> > fingersChain;

        // files and streams
    QFile fingerFile;
    QDataStream fingerStream;

    QByteArray fingersBA;

    EgFingers():
        //  rootHeaderSize(sizeof(KeyType) * 2 + sizeof(egIndexes3Namespace::fingersLevelType) + sizeof(egIndexes3Namespace::keysCountType) + sizeof(quint64))
        // ,fingersHeaderSize(sizeof(KeyType) * 2 + sizeof(egIndexes3Namespace::fingersCountType) + sizeof(egIndexes3Namespace::fingersLevelType))
         rootHeaderSize(sizeof(KeyType) * 2 + sizeof(fingersLevelType) + sizeof(keysCountType) + sizeof(quint64))
        ,oneFingerSize(sizeof(KeyType) * 2 + sizeof(keysCountType) + sizeof(quint64)) // next chunk offset
        ,fingersChunkSize(/*fingersHeaderSize + */(egIndexesNamespace::egChunkVolume * oneFingerSize) + sizeof(quint64) + sizeof(fingersLevelType)) // parent chunk offset, level

        ,fingersChunk(new char[fingersChunkSize])
        ,zeroFingersChunk(new char[fingersChunkSize])
        ,newFingersChunk(new char[fingersChunkSize])
    {
        memset(zeroFingersChunk, 0, fingersChunkSize);
        // qDebug() << FN << "zeroFingersChunk size = " << sizeof(zeroFingersChunk);
        fingersBA.resize(fingersChunkSize);
    }

    ~EgFingers() { if (zeroFingersChunk) delete[] zeroFingersChunk; if (newFingersChunk) delete[] newFingersChunk; if (fingersChunk) delete[] fingersChunk;}

    void PrintFingerInfo(egFinger<KeyType>& fingerInfo, const QString &theMessage);
    void PrintFingersChunk(char* theFingersChunk, const QString& theMessage);
    void PrintChunkInfo(quint64 fingersChunkOffset);

    void PrintAllChunksInfo(const QString& theMessage);

    int OpenIndexFilesToUpdate(const QString& IndexFileName);
    int OpenIndexFilesToRead(const QString& IndexFileName);

    void CloseIndexFiles();
    void RemoveIndexFiles(const QString& IndexFileName);

    void InitFingersChunk();

    void LoadFingersChunk(quint64 fingersChunkOffset);
    int StoreFingersChunk(quint64 fingersChunkOffset, char* chunkPtr);

    void InitRootHeader();

    void LoadRootHeader();              // meta-info of fingers tree, also for non-zero offset
    void StoreRootHeader(bool minMaxOnly = false);

    // int FindIndexChunkToInsert();
    // int FindNextLevelOffsetToInsert();
    // int SelectClosestFingerToInsert(QDataStream &localFingersStream);

    int FindIndexChunkToInsert();

    int UpdateFingerAfterInsert();
    int UpdateFingersChainAfterInsert();    
    int UpdateFingersChainAfterSplit();

    int UpdateFingerAfterDelete();
    int UpdateFingersChainAfterDelete();

    int UpdateMinValueUp();
    int UpdateMaxValueUp();

    void DeleteParentFinger();
    void DeleteSpecificFinger();
    void DeleteFingersChunk(quint64 fingersChunkOffset);

    inline void ReadFinger  (QDataStream &localFingersStream, egFinger<KeyType>& theFinger);
    inline void WriteFinger (QDataStream &localFingersStream, egFinger<KeyType>& theFinger);

    inline void UpdateTheFinger(egFinger<KeyType>& theFinger, bool addOne = true);

    int InsertSplittedFinger(QDataStream &localFingersStream);
    int SplitFingersChunk(QDataStream &localFingersStream);
    void AddNewRootChunk(QDataStream &localFingersStream);

    int InsertSplittedFinger2(QDataStream &localFingersStream);

    int SplitFingersChunk2(QDataStream &localFingersStream);
    int InsertNewFinger2(QDataStream &localFingersStream, char* theChunk, int posToInsert, int itemsCount);
    int UpdateParentsOffsets2(QDataStream &localFingersStream, char* theChunk, quint64 myChunkOffset,  int posToInsert, int itemsCount);

    void UpdateMinMax(QDataStream& localFingersStream, egFinger<KeyType>& theFinger, char* theChunk);

    int AppendFingersChunk(QDataStream &localFingersStream);

    int FindIndexChunkFirst(bool isExactEqual); // CompareFunctionType myCompareFunc
    int FindNextLevelOffsetFirst(QDataStream &localFingersStream, bool isExactEqual);

    int FindIndexChunkLast(bool isExactEqual); // CompareFunctionType myCompareFunc
    // int FindNextLevelOffsetLast(bool isExact);
    int FindNextLevelOffsetLast(QDataStream &localFingersStream, bool isExactEqual);

    int StoreParentOffset(quint64 fingersChunkOffset, quint64 parentFingerOffset);

};

#endif // EG_FINGERS_H
