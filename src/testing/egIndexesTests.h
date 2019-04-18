/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EGINDEXESTESTS_H
#define EGINDEXESTESTS_H

#include "indexes/egIndexesFiles.h"
#include "egExtraInfo.h"

#include <QtDebug>

template <typename KeyType> class EgIndexesTests
{
public:




    // add first index / init
    bool testEgDataIndexAddFirst(EgIndexFiles<KeyType>& egIndexFiles);

    bool testEgDataDeleteIndex(EgIndexFiles<KeyType>& egIndexFiles);

    bool testEgDataIndexDelFirst(EgIndexFiles<KeyType>& egIndexFiles);

    bool testEgDataInsertIndexes(EgIndexFiles<KeyType>& egIndexFiles);

    bool testEgDataDeleteAllIndexes(EgIndexFiles<KeyType>& egIndexFiles);

    bool testSelectOperations(EgIndexFiles<KeyType>& egIndexFiles);



    void testShowResult(bool res, const QString &theMessage)
    {
        if (res)
        {
            qDebug().noquote() << "PASS" << theMessage;
        }
        else
        {
            qDebug().noquote() << "FAILED" << theMessage;
        }
    }

};

#endif // EGINDEXESTESTS_H
