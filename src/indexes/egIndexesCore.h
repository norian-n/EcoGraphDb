#ifndef EGINDEXES_CORE_H
#define EGINDEXES_CORE_H

typedef quint16 keysCountType;
typedef quint16 fingersLevelType;

namespace egIndexes3Namespace
{

const keysCountType egChunkVolume = 4;              // keys in chunk


} // egIndexesNamespace


template <typename KeyType> struct egFinger
{
    KeyType minKey;             // max key value of chunk
    KeyType maxKey;             // max key value of chunk

    keysCountType itemsCount;   // next level keys or fingers
    quint64 nextChunkOffset;    // child chunk (fingers or indexes) file position

    fingersLevelType myLevel;   // 0 - just above index chunk
    quint64 myOffset;           // finger position in current chunk
    quint64 myChunkOffset;      // file offset of current chunk -> parent

    // keysCountType fingersCount;      // fingers of current chunk -> parent
};


#endif // EGINDEXES_CORE_H
