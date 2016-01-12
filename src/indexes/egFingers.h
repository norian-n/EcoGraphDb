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

    // quint64 fingersChunkOffset; // finger file position

    // int rootHeaderSize;
    int oneFingerSize;
    int fingersChunkSize;

    int posToInsert;

    quint64 newFingerOffset;
    quint64 parentFingerOffset;

    KeyType newMinValue;
    KeyType newMaxValue;

    // keysCountType newKeysCount;

    bool minValueChanged;
    bool maxValueChanged;

    char* fingersChunk;     // current fingers chunk buffer
    char* zeroFingersChunk; // filled with 0
    char* newFingersChunk;  // new chunk buffer to split

    egFinger<KeyType> currentFinger;
    egFinger<KeyType> newFinger;         // to split chunks
    egFinger<KeyType> parentFinger;
    egFinger<KeyType> fingersRootHeader; // (!) stored in the indexes file

    QList < egFinger<KeyType> > fingersChain;

        // files and streams
    QFile fingerFile;
    QDataStream fingerStream;

    QByteArray fingersBA;

    EgFingers():
        //  rootHeaderSize(sizeof(KeyType) * 2 + sizeof(egIndexes3Namespace::fingersLevelType) + sizeof(egIndexes3Namespace::keysCountType) + sizeof(quint64))
        // ,fingersHeaderSize(sizeof(KeyType) * 2 + sizeof(egIndexes3Namespace::fingersCountType) + sizeof(egIndexes3Namespace::fingersLevelType))
         oneFingerSize(sizeof(KeyType) * 2 + sizeof(keysCountType) + sizeof(quint64))
        ,fingersChunkSize(/*fingersHeaderSize + */(egIndexes3Namespace::egChunkVolume * oneFingerSize) + sizeof(quint64)) // parent chunk offset

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

    int OpenIndexFilesToUpdate(const QString& IndexFileName);
    int OpenIndexFilesToRead(const QString& IndexFileName);

    void CloseIndexFiles();
    void RemoveIndexFiles(const QString& IndexFileName);

    void InitFingersChunk();

    void LoadFingersChunk(quint64 fingersChunkOffset);
    int StoreFingersChunk(quint64 fingersChunkOffset, char* chunkPtr);

    int FindIndexChunkToInsert();
    int FindNextLevelOffsetToInsert();
    int SelectClosestFingerToInsert(QDataStream &localFingersStream);

    int UpdateFingerAfterInsert();
    int UpdateFingersChainAfterInsert();    
    int UpdateFingersChainAfterSplit(bool appendMode);

    int UpdateFingerAfterDelete();
    int UpdateFingersChainAfterDelete();

    void DeleteFinger();
    bool DeleteSpecificFinger();

    inline void ReadFinger  (QDataStream &localFingersStream, egFinger<KeyType>& theFinger);
    inline void WriteFinger (QDataStream &localFingersStream, egFinger<KeyType>& theFinger);

    inline void UpdateTheFinger(egFinger<KeyType>& theFinger, bool addOne = true);

    int InsertSplittedFinger(QDataStream &localFingersStream);
    int SplitFingersChunk(QDataStream &localFingersStream);
    void AddNewRootChunk(QDataStream &localFingersStream);


    int AppendFingersChunk(QDataStream &localFingersStream);

    int FindIndexChunkFirst(bool isExact); // CompareFunctionType myCompareFunc
    int FindNextLevelOffsetFirst(bool isExact);

    int FindIndexChunkLast(bool isExact); // CompareFunctionType myCompareFunc
    int FindNextLevelOffsetLast(bool isExact);

    int StoreParentOffset(quint64 fingersChunkOffset, quint64 parentFingerOffset);

};

#endif // EG_FINGERS_H