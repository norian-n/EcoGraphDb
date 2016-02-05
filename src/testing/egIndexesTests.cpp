#include "egIndexesTests.h"

template <typename KeyType> bool EgIndexesTests<KeyType>::testEgDataIndexAddFirst(EgIndexFiles<KeyType> &egIndexFiles)
{
    egIndexFiles.OpenIndexFilesToUpdate();

    egIndexFiles.theIndex  = 0x1; // 0x6144;
    egIndexFiles.dataOffset = 0xabcd01;

    egIndexFiles.AddObjToIndex();

    // egIndexFiles.LoadFingersChunk();
    // egIndexFiles.LoadIndexChunk();

    egIndexFiles.CloseIndexFiles();

    return true;
}

template <typename KeyType> bool EgIndexesTests<KeyType>::testEgDataInsertIndexes(EgIndexFiles<KeyType> &egIndexFiles)
{
    qDebug() <<  "Launch " << FN;

    egIndexFiles.OpenIndexFilesToUpdate();

    for (int i= 0; i < 15; i++) // 14 23 19
    {
        if (i % 2)
        {
            egIndexFiles.theIndex   = 0x100 - i ;
            egIndexFiles.dataOffset = 0xabcd20 -  i ;
        }
        else
        {
            egIndexFiles.theIndex   = i + 0x10;
            egIndexFiles.dataOffset = 0xabcd00 + i + 0x10;
        }

        egIndexFiles.AddObjToIndex();
    }

    egIndexFiles.CloseIndexFiles();

    return true;
}


template <typename KeyType> bool EgIndexesTests<KeyType>::testSelectOperations(EgIndexFiles<KeyType> &egIndexFiles)
{
    bool res = true;

    KeyType theKey = 0x15; // 0x15

    QSet<quint64> theIndexOffsets;

    egIndexFiles.Load_EQ(theIndexOffsets, theKey);

    qDebug() <<  "Load_EQ count = " << theIndexOffsets.count() << FN;
    res = res && (theIndexOffsets.count() == 1) ;

    theIndexOffsets.clear();
    egIndexFiles.Load_GE(theIndexOffsets, theKey);

    qDebug() <<  "Load_GE count = " << theIndexOffsets.count() << FN;
    res = res && (theIndexOffsets.count() == 18) ;

    theIndexOffsets.clear();
    egIndexFiles.Load_GT(theIndexOffsets, theKey);

    qDebug() <<  "Load_GT count = " << theIndexOffsets.count() << FN;
    res = res && (theIndexOffsets.count() == 17) ;

    theIndexOffsets.clear();
    egIndexFiles.Load_LE(theIndexOffsets, theKey);

    qDebug() <<  "Load_LE count = " << theIndexOffsets.count() << FN;
    res = res && (theIndexOffsets.count() == 6) ;

    theIndexOffsets.clear();
    egIndexFiles.Load_LT(theIndexOffsets, theKey);

    qDebug() <<  "Load_LT count = " << theIndexOffsets.count() << FN;
    res = res && (theIndexOffsets.count() == 5) ;

    // for (QSet<quint64>::iterator offsets_iter = theIndexOffsets.begin(); offsets_iter != theIndexOffsets.end(); ++offsets_iter)
    //    qDebug() <<  "Seek data pos = " << hex << (unsigned int) *offsets_iter << FN;

    testShowResult(res, FNS);

    return res;
}

template <typename KeyType> bool EgIndexesTests<KeyType>::testEgDataDeleteIndex(EgIndexFiles<KeyType> &egIndexFiles)
{
    egIndexFiles.OpenIndexFilesToUpdate();

    egIndexFiles.theIndex  = 0x1; // 0x6144;
    egIndexFiles.dataOffset = 0xabcd01;

    egIndexFiles.DeleteIndex();

    // egIndexFiles.LoadFingersChunk();
    // egIndexFiles.LoadIndexChunk();

    egIndexFiles.CloseIndexFiles();

    return true;
}

template class EgIndexesTests<qint32>;
