/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2020 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

// #ifndef EGINDEXES_CORE_H
// #define EGINDEXES_CORE_H

#pragma once

typedef uint16_t keysCountType;
typedef uint16_t fingersLevelType;


template <typename KeyType> struct egFinger
{
    KeyType minKey;             // max key value of chunk
    KeyType maxKey;             // max key value of chunk

    keysCountType itemsCount;   // next level keys or fingers
    uint64_t nextChunkOffset;    // child chunk (fingers or indexes) file position

    fingersLevelType myLevel;   // 0 - just above index chunk

    uint64_t myOffsetInChunk {0};       // finger position in current chunk
    uint64_t myChunkOffset {0};  // file offset of current chunk -> parent
};

namespace egIndexesNamespace
{
    const int indexHeaderSize = sizeof(uint64_t) * 2;    // header of index file
    const keysCountType egChunkVolume = 4;               // keys in chunk

} // egIndexesNamespace




// #endif // EGINDEXES_CORE_H
