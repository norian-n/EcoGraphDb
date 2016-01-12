#ifndef EGINDEXESTESTS_H
#define EGINDEXESTESTS_H

#include "indexes/egIndexesFiles.h"
#include "egMetaInfo.h"

#include <QtDebug>

template <typename KeyType> class EgIndexesTests
{
public:
    // add first index / init
    bool testEgDataIndexAddFirst(EgIndexFiles<KeyType>& egIndexFiles);

    bool testEgDataInsertIndexes(EgIndexFiles<KeyType>& egIndexFiles);

    bool testSelectOperations(EgIndexFiles<KeyType>& egIndexFiles);

    bool testEgDataDeleteIndex(EgIndexFiles<KeyType>& egIndexFiles);

    void testShowResult(bool res, const QString &theMessage)
    {
        if (res)
        {
            qDebug() << "PASSED" << theMessage;
        }
        else
        {
            qDebug() << "FAILED" << theMessage;
        }
    }

};

#endif // EGINDEXESTESTS_H