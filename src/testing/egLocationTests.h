/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EGLOCATIONTESTS_H
#define EGLOCATIONTESTS_H

#include <QtDebug>

#include "egGraphDatabase.h"

class EgLocationTests
{
public:

    EgGraphDatabase graphDB;
    EgDataNodesType testDataNodes;

    bool testCreateLocations();

    bool testAddLocations();

    bool testLoadLocationsData();

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

#endif // EGLOCATIONTESTS_H
