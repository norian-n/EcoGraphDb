/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egLogging.h"

egAsyncLogger::egAsyncLogger(egAsyncLoggerEngine& logEngine) :
    theEngine(&logEngine),
    egStringBufStream(&logBufferString)
{

}

egAsyncLoggerEngine::egAsyncLoggerEngine(const char *theLogFileName) :
    logFileName(theLogFileName),
    loggerThread(&egAsyncLoggerHandler::egLogThreadLoop,
                 &theHandler,
                 std::ref(egLoggingStream),
                 std::ref(egLogPostbox))
{
    egLoggingFile.setFileName("egdb/" + QString(theLogFileName) + ".log");
}

int egAsyncLoggerEngine::LaunchLoggerEngine()
{
        // log file


    if (!egLoggingFile.open(QIODevice::Append | QIODevice::Text)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "Logging error: can't open file " << "egdb/" << logFileName;
        return -1;
    }

    egLoggingStream.setDevice(&egLoggingFile);

    return 0;
}

int64_t egAsyncLoggerEngine::getLogID()
{
    return idCounter++;
}

void egAsyncLoggerHandler::egLogThreadLoop(QTextStream& loggingStream, postbox<egLogLetter> & egLogPostbox)
{
    egLogLetter* theLetter;

    while(true)
    {
            // wait for message
        theLetter = egLogPostbox.receive();

            // check for terminate
        if (! theLetter-> errorID)
        {
            delete theLetter;
            return;
        }

            // write to file
        loggingStream << theLetter-> errorID << " " << theLetter-> logMessage << "\n";

        delete theLetter;
    }
}

egAsyncLogger::~egAsyncLogger()
{
    if(! logBufferString.isEmpty()) // flush
    {
            // send letter
        egLogLetter* theLetter = new egLogLetter();

        theLetter-> errorID =  theEngine-> getLogID();
        theLetter-> logMessage = logBufferString;

        logBufferString.clear();

        theEngine-> egLogPostbox.send(theLetter);
    }
}

egAsyncLoggerEngine::~egAsyncLoggerEngine()
{
        // send terminate letter
    egLogLetter* theLetter = new egLogLetter();

    theLetter-> errorID = 0;
    theLetter-> logMessage = QString();

    egLogPostbox.send(theLetter);

    // std::thread t2 = move(t);

        // join logging thread
    loggerThread.join();

        // close file
    egLoggingFile.close();
}

int64_t egAsyncLogger::egLogAsync(QString& logMessage)
{
        // send letter
    egLogLetter* theLetter = new egLogLetter();

    int64_t newID = theEngine-> getLogID();

    theLetter-> errorID = newID;
    theLetter-> logMessage = logMessage;

    theEngine-> egLogPostbox.send(theLetter);

    return newID;
}

int64_t egAsyncLogger::egLogAsyncFlush()
{
    egLogLetter* theLetter;

    egStringBufStream.flush();

    if(logBufferString.isEmpty())
        return 0;

        // send letter
    theLetter = new egLogLetter();

    int64_t newID = theEngine-> getLogID();

    theLetter-> errorID = newID;
    theLetter-> logMessage = logBufferString;

    logBufferString.clear();

    theEngine-> egLogPostbox.send(theLetter);

    return newID;
}

