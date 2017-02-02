#ifndef EG_ATTRIBUTES_TESTS_H
#define EG_ATTRIBUTES_TESTS_H

#include <QtDebug>

#include "egGraphDatabase.h"

class EgNamedAttributesTests
{
public:

    EgGraphDatabase graphDB;
    EgDataNodesType testDataNodes;

    bool testCreateAttributes();

    bool testAddAttributes();

    bool testLoadAttributes();

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

#endif // EG_ATTRIBUTES_TESTS_H
