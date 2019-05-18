/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "../egDataNodesType.h"
#include "egIndexesFiles.h"

template <typename KeyType> void EgIndexFiles<KeyType>::RemoveFiles()
{
    if (indexFile.isOpen())
        indexFile.close();

    indexFile.setFileName("egdb/" + IndexFileName + ".odx");
    indexFile.remove();

    if (fingerFile.isOpen())
        fingerFile.close();

    fingerFile.setFileName("egdb/" + IndexFileName + ".odf");
    fingerFile.remove();
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenIndexFileToUpdate(QDataStream& theIndexStream)
{
    indexFile.close();

    indexFile.setFileName("egdb/" + IndexFileName + ".odx");

    theIndexStream.setDevice(&indexFile);

    if (!indexFile.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open index file " << indexFile.fileName();
        return -1;
    }

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenIndexFileToRead(QDataStream& theIndexStream)
{
    indexFile.close();

    indexFile.setFileName("egdb/" + IndexFileName + ".odx");

    if (!indexFile.exists())
    {
        qDebug() << "Error: file doesn't exist " << indexFile.fileName() << FN;
        return -1;
    }

    theIndexStream.setDevice(&indexFile);

    if (!indexFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        if (! IndexFileName.contains(EgDataNodesNamespace::egGUIfileName))
            qDebug() << "Error: can't open index file " << indexFile.fileName() << FN;
        return -2;
    }

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenIndexFileToCheck(const QString& IndexFilePath, QDataStream& theIndexStream)
{
    indexFile.close();

    indexFile.setFileName(IndexFilePath);

    if (!indexFile.exists())
    {
        qDebug() << "Error: file doesn't exist " << IndexFilePath << FN;
        return -1;
    }

    theIndexStream.setDevice(&indexFile);

    if (!indexFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "Error: can't open index file " << indexFile.fileName() << FN;
        return -2;
    }

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenFingerFileToUpdate(QDataStream &theFingerStream)
{
    fingerFile.close();

    fingerFile.setFileName("egdb/" + IndexFileName + ".odf");
    theFingerStream.setDevice(&fingerFile);

    if (!fingerFile.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open finger file " << fingerFile.fileName();
        return -2;
    }

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenFingerFileToRead(QDataStream& theFingerStream)
{
    fingerFile.close();

    fingerFile.setFileName("egdb/" + IndexFileName + ".odf");

    if (!fingerFile.exists())
    {
        qDebug() << "file doesn't exist " << IndexFileName + ".odf" << FN;
        return -1;
    }

    theFingerStream.setDevice(&fingerFile);

    if (!fingerFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "can't open finger file " << fingerFile.fileName() << FN;
        return -2;
    }

    return 0;
}


template <typename KeyType> int EgIndexFiles<KeyType>::OpenFingerFileToCheck(const QString& IndexFilePath, QDataStream &theFingerStream)
{
    fingerFile.close();

    fingerFile.setFileName(IndexFilePath);

    if (!fingerFile.exists())
    {
        qDebug() << "file doesn't exist " << IndexFileName << FN;
        return -1;
    }

    theFingerStream.setDevice(&fingerFile);

    if (!fingerFile.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open finger file " << fingerFile.fileName() << FN;
        return -2;
    }

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenFilesToUpdate()
{
    // qDebug() << "IndexFileName = " << IndexFileName << FN;

    dir = QDir::current();

    if (! dir.exists("egdb"))
    {
        qDebug()  << "error: can't find the egdb dir " << FN;
        return -1;
    }

    fingersTree.IndexFileName = IndexFileName;

    int res = OpenIndexFileToUpdate(indexChunks.indexStream);
    res    += OpenFingerFileToUpdate(fingersTree.fingerStream);

    if (fingersTree.fingerStream.device()->size() && ! res) // is not empty FIXME check both files
    {
        // indexChunks.LoadRootHeader();
        fingersTree.LoadRootFinger();
    }
    // else
    //   qDebug() << "Can't load Root Header " << FN;

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::OpenFilesToRead()
{
    // qDebug() << "In OpenIndexFilesToRead()" << FN;

    dir = QDir::current();

    // if (dir.dirName() != "egdb")
    if (! dir.exists("egdb"))
    {
        qDebug()  << "error: can't find the egdb dir " << FN;
        return -1;
    }

    fingersTree.IndexFileName = IndexFileName;

    int res = OpenIndexFileToRead(indexChunks.indexStream);
    res    += OpenFingerFileToRead(fingersTree.fingerStream);

    if (res)    // error
    {
        CloseFiles();

        // if (dir.dirName() == "egdb")
        //    dir.setCurrent("..");

        return res;
    }

    if (fingersTree.fingerStream.device()->size()) // && indexChunks.indexStream.device()->size()) // is not empty
    {
        // indexChunks.LoadRootHeader();
        fingersTree.LoadRootFinger();
    }
    // else
    //    qDebug() << "Can't load Root Header " << res << FN;

    return res;
}

template <typename KeyType> void EgIndexFiles<KeyType>::CloseFiles()
{
    indexFile.close();
    fingerFile.close();
}


template <typename KeyType> void EgIndexFiles<KeyType>::AddIndex()
{

    indexChunks.theKey = theIndex;
    indexChunks.oldDataOffset = dataOffset;

    // qDebug()  << "IndexFileName = " << IndexFileName << FN;
    // qDebug()  << "Key: " << theIndex << " , offset: " << dataOffset << FN;

        // check for empty file
    if ( ! indexChunks.indexStream.device()->size() ) // is empty
    {
        // qDebug()  << "index" << theIndex << "empty file, calling AppendNewChunk()" << FN;

        indexChunks.InitRootHeader();
        indexChunks.InitIndexChunk();

        fingersTree.InitRootFinger();
        fingersTree.StoreRootFinger();

        indexChunks.StoreRootHeader();
        indexChunks.StoreIndexChunk(indexChunks.chunk, egIndexesNamespace::indexHeaderSize);

        return;
    }
    else
    {
        fingersTree.FindIndexChunkToInsert();   // FIXME check if no finger

        indexChunks.InsertToIndexChunk();
    }
}

template <typename KeyType> int EgIndexFiles<KeyType>::UpdateIndex(bool isChanged, bool isPrimary)
{
    indexChunks.theKey = theIndex;
    indexChunks.oldDataOffset = dataOffset;
    indexChunks.newDataOffset = newOffset;

    // qDebug() << "Update index files name:  " << IndexFileName << FN;

    if (fingersTree.FindIndexChunkFirst(true))
    {
        qDebug() << "IndexChunk not found " << FN;
        return -1;
    }

    if (! isChanged)
        indexChunks.UpdateIndex(isPrimary);
    else
    {
        if (indexChunks.DeleteIndex(isPrimary) == 1) // last index, recreate everything
        {
            RemoveFiles();
            indexChunks.theKey = newIndex;
            indexChunks.oldDataOffset = newOffset;

            indexChunks.InitRootHeader();
            indexChunks.InitIndexChunk();

            fingersTree.InitRootFinger();
            fingersTree.StoreRootFinger();

            indexChunks.StoreRootHeader();
            indexChunks.StoreIndexChunk(indexChunks.chunk, egIndexesNamespace::indexHeaderSize);

            return 0;
        }

        indexChunks.theKey = newIndex;
        indexChunks.oldDataOffset = newOffset;

        fingersTree.FindIndexChunkToInsert();   // FIXME check result

        // qDebug() << "nextOffset: " << hex << (int) indexChunks. << FN;

        indexChunks.InsertToIndexChunk();
    }

    if (isPrimary)
        dataOffset = indexChunks.oldDataOffset;

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::DeleteIndex(bool isPrimary)
{

    indexChunks.theKey = theIndex;
    indexChunks.oldDataOffset = dataOffset;

    // qDebug() << "Delete index file name:  " << IndexFileName << FN;

    if (fingersTree.FindIndexChunkFirst(true))
    {
        qDebug() << "IndexChunk not found " << FN;
        return -1;
    }

    if (indexChunks.DeleteIndex(isPrimary) == 1) // last index
    {
        RemoveFiles();
        return 0;
    }

    if (isPrimary)
        dataOffset = indexChunks.oldDataOffset;

    return 0;
}

template <typename KeyType> int EgIndexFiles<KeyType>::LoadAllDataOffsets(QSet<quint64>& dataOffsets)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

    fingersTree.IndexFileName = IndexFileName;  // debug info

    indexChunks.LoadAllData(dataOffsets);

    CloseFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_EQ(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

    indexChunks.theKey   = Key;

    res = fingersTree.FindIndexChunkFirst(true);

    // qDebug() << "filename: " << IndexFileName << " ,key: " << hex << (int) indexChunks.theKey
    //         << ", offset: " << hex << (int) indexChunks.indexesChunkOffset << "res = " << res << FN;


    if (! res)  // ok
        indexChunks.LoadDataByChunkEqual(index_offsets);

    CloseFiles();

    return res;
}


template <typename KeyType> int EgIndexFiles<KeyType>::Load_GE(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

    indexChunks.theKey   = Key;

    res = fingersTree.FindIndexChunkFirst(true); // FIXME - process borders

    if (! res)  // ok
        indexChunks.LoadDataByChunkUp(index_offsets, EgIndexes<KeyType>::CompareGE);

    CloseFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_GT(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

    indexChunks.theKey = Key;

    res = fingersTree.FindIndexChunkLast(false); // FIXME - process borders

    if (! res)
        indexChunks.LoadDataByChunkUp(index_offsets, EgIndexes<KeyType>::CompareGT);

    // qDebug()  << "res = " << res << "index_offsets count = " << index_offsets.count() << FN;

    CloseFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_LE(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

    indexChunks.theKey   = Key;

    res = fingersTree.FindIndexChunkLast(true); // FIXME - process borders

    if (! res)
        indexChunks.LoadDataByChunkDown(index_offsets, EgIndexes<KeyType>::CompareLE);

    CloseFiles();

    return res;
}

template <typename KeyType> int EgIndexFiles<KeyType>::Load_LT(QSet<quint64>& index_offsets, KeyType Key)
{
    int res = OpenFilesToRead();

    if (res)    // error
        return res;

    indexChunks.theKey = Key;

    res = fingersTree.FindIndexChunkFirst(false); // FIXME - process borders

    if (! res)
        indexChunks.LoadDataByChunkDown(index_offsets, EgIndexes<KeyType>::CompareLT);

    // qDebug()  << "res = " << res << "index_offsets count = " << index_offsets.count() << FN;

    CloseFiles();

    return res;
}

/*
EgIndexFiles<qint32>  EgIndexFilesqint32(QString("EgIndexFilesqint32"));
EgIndexFiles<quint32> EgIndexFilesquint32(QString("EgIndexFilesquint32"));
EgIndexFiles<qint64>  EgIndexFilesqint64(QString("EgIndexFilesqint64"));
EgIndexFiles<quint64> EgIndexFilesquint64(QString("EgIndexFilesquint64"));
EgIndexFiles<float>   EgIndexFilesfloat(QString("EgIndexFilesfloat"));
EgIndexFiles<double>  EgIndexFilesdouble(QString("EgIndexFilesdouble"));
*/

