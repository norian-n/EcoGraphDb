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
