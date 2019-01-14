/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EGINDEXES_FILES3_H
#define EGINDEXES_FILES3_H

#include <QDir>
#include <QVariant>

// #include "egMetaInfo.h"
#include "egIndexes.h"
#include "egFingers.h"

    // abstract interface for templates
class EgIndexFilesBase
{
public:

    virtual ~EgIndexFilesBase() {}

    virtual void setIndex(QVariant& ) = 0;
    virtual void setNewIndex(QVariant& ) = 0;

    virtual void setDataOffset(quint64 ) = 0;
    virtual void setNewOffset(quint64 ) = 0;

    virtual int OpenIndexFilesToUpdate() = 0;
    virtual int OpenIndexFilesToRead() = 0;

    virtual void CloseIndexFiles() = 0;
    virtual void RemoveIndexFiles() = 0;

    virtual void AddIndex() = 0;
    virtual int UpdateIndex(bool isChanged, bool isPrimary) = 0;
    virtual int DeleteIndex(bool isPrimary) = 0;

    virtual int LoadAllDataOffsets(QSet<quint64>& dataOffsets) = 0;

    virtual int Load_EQ(QSet<quint64>& index_offsets, QVariant& Key) = 0;

    // virtual int Load_EQ(QSet<quint64>& index_offsets, int key) {}

    virtual int Load_GE(QSet<quint64>& index_offsets, QVariant& Key) = 0;
    virtual int Load_GT(QSet<quint64>& index_offsets, QVariant& Key) = 0;

    virtual int Load_LE(QSet<quint64>& index_offsets, QVariant& Key) = 0;
    virtual int Load_LT(QSet<quint64>& index_offsets, QVariant& Key) = 0;

};

    // combined chunks and fingers API for index operations
template <typename KeyType> class EgIndexFiles: public EgIndexFilesBase
{
public:

    EgIndexes<KeyType> indexChunks;
    EgFingers<KeyType> fingersTree;
    // EgIndexesComparator<KeyType> indexesComparator;

    QDir dir;

    QString IndexFileName;

    EgIndexFiles() = delete; // constructor with IndexFileName only

    EgIndexFiles(const QString& anIndexFileName):
        IndexFileName(anIndexFileName)
    {
        indexChunks.fingersTree = &fingersTree;
        fingersTree.indexChunks = &indexChunks;
    }

    virtual ~EgIndexFiles() { }

    KeyType theIndex;      // current index key
    quint64 dataOffset;    // current index value - offset in data file

    KeyType newIndex;      // index key to update
    quint64 newOffset;     // offset in data file for updated objects

    void setIndex(QVariant& indexValue)      { if (indexValue.canConvert<KeyType>()) theIndex = indexValue.value<KeyType>(); } // FIXME STUB
    void setNewIndex(QVariant& indexValue)   { if (indexValue.canConvert<KeyType>()) newIndex = indexValue.value<KeyType>(); } // FIXME STUB

    void setDataOffset(quint64 theDataOffset) { dataOffset = theDataOffset; }
    void setNewOffset(quint64 theNewOffset)   { newOffset = theNewOffset; }

    int OpenIndexFilesToUpdate();
    int OpenIndexFilesToRead();

    void CloseIndexFiles();
    void RemoveIndexFiles();

        // single node operations
    void AddIndex();
    int UpdateIndex(bool isChanged, bool isPrimary = false);
    int DeleteIndex(bool isPrimary = false);

    // int GetOffsetByKey(); // improvement for server-side operations

        // load operations
    int LoadAllDataOffsets(QSet<quint64>& dataOffsets);

    int Load_EQ(QSet<quint64>& index_offsets, QVariant& KeyValue) { if (KeyValue.canConvert<KeyType>())
                                                                        Load_EQ(index_offsets, KeyValue.value<KeyType>());
                                                                    return 0;
                                                                  } // FIXME STU


    int Load_GE(QSet<quint64>& index_offsets, QVariant& KeyValue) { if (KeyValue.canConvert<KeyType>())
                                                                        Load_GE(index_offsets, KeyValue.value<KeyType>());
                                                                    return 0;
                                                                  } // FIXME STUB

    int Load_GT(QSet<quint64>& index_offsets, QVariant& KeyValue) { if (KeyValue.canConvert<KeyType>())
                                                                        Load_GT(index_offsets, KeyValue.value<KeyType>());
                                                                    return 0;
                                                                  } // FIXME STUB

    int Load_LE(QSet<quint64>& index_offsets, QVariant& KeyValue) { if (KeyValue.canConvert<KeyType>())
                                                                        Load_LE(index_offsets, KeyValue.value<KeyType>());
                                                                    return 0;
                                                                  } // FIXME STUB

    int Load_LT(QSet<quint64>& index_offsets, QVariant& KeyValue) { if (KeyValue.canConvert<KeyType>())
                                                                        Load_LT(index_offsets, KeyValue.value<KeyType>());
                                                                    return 0;
                                                                  } // FIXME STUB

    // int Load_NE(QSet<quint64>& index_offsets, KeyType& Key, const QString& IndexFileName);          // load not equal objects


    int Load_GE(QSet<quint64>& index_offsets, KeyType Key);       // load greater or equal objects
    int Load_GT(QSet<quint64>& index_offsets, KeyType Key);       // load greater objects
    int Load_LE(QSet<quint64>& index_offsets, KeyType Key);       // load lesser or equal objects
    int Load_LT(QSet<quint64>& index_offsets, KeyType Key);       // load lesser objects
    int Load_EQ(QSet<quint64>& index_offsets, KeyType Key);       // load equal objects


};

#endif // EGINDEXESFILES3_H
