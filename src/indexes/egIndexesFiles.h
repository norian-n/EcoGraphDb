/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EGINDEXES_FILES3_H
#define EGINDEXES_FILES3_H

#include <QDir>

// #include "egMetaInfo.h"
#include "egIndexes.h"
#include "egFingers.h"

template <typename KeyType> class EgIndexFiles // main API
{
public:


    EgIndexes<KeyType> indexChunks;
    EgFingers<KeyType> fingersTree;
    // EgIndexesComparator<KeyType> indexesComparator;

    QDir dir;

    QString IndexFileName;

    EgIndexFiles(); // constructor with IndexFileName only

    EgIndexFiles(const QString& anIndexFileName):
        IndexFileName(anIndexFileName)
    {
        indexChunks.fingersTree = &fingersTree;
        fingersTree.indexChunks = &indexChunks;
        fingersTree.rootHeaderSize = indexChunks.rootHeaderSize; // FIXME STUB
    }

    ~EgIndexFiles() { }

    // FIXME STUB
    KeyType theIndex;      // current index key
    quint64 dataOffset;    // current index value - offset in data file
    quint64 newOffset;     // offset in data file for updated objects

    int OpenIndexFilesToUpdate();
    int OpenIndexFilesToRead();

    void CloseIndexFiles();
    void RemoveIndexFiles();

    void AddObjToIndex();
    int UpdateIndex(bool isChanged);
    int DeleteIndex();

        // load data nodes
    int Load_EQ(QSet<quint64>& index_offsets, KeyType Key);                  // load equal objects
    // int Load_NE(QSet<quint64>& index_offsets, KeyType& Key, const QString& IndexFileName);          // load not equal objects

    int Load_GE(QSet<quint64>& index_offsets, KeyType Key);       // load greater or equal objects
    int Load_GT(QSet<quint64>& index_offsets, KeyType Key);       // load greater objects

    int Load_LE(QSet<quint64>& index_offsets, KeyType Key);       // load lesser or equal objects
    int Load_LT(QSet<quint64>& index_offsets, KeyType Key);       // load lesser objects

};

#endif // EGINDEXESFILES3_H
