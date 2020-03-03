/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2020 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

// #ifndef EGLINKSTESTS_H
// #define EGLINKSTESTS_H

#pragma once

#include <QtDebug>

#include "egGraphDatabase.h"

class EgLinksTests
{
public:

    EgGraphDatabase graphDB;
    EgDataNodesType testDataNodes;

    EgLinkType testTree;
    // EgLinkType funcblocksOfProject;

    bool testLinksTree();

    void testCreateNodes();

    void testCreateLinks();

    void testAddIndexedNodes();

    void testUpdateIndexedNodes();

    // void testDeleteIndexedNodes();

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


// #endif // EGLINKSTESTS_H
