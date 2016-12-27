#ifndef EGLINKSTESTS_H
#define EGLINKSTESTS_H

#include <QtDebug>

#include "egGraphDatabase.h"

class EgLinksTests
{
public:

    EgGraphDatabase graphDB;
    EgDataNodesType testDataNodes;

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
            qDebug() << "PASSED" << theMessage;
        }
        else
        {
            qDebug() << "FAILED" << theMessage;
        }
    }

};


#endif // EGLINKSTESTS_H
