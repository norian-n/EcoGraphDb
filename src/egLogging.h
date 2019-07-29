/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_LOGGING_ASYNC_H
#define EG_LOGGING_ASYNC_H

#include <QString>

#include <atomic>
#include <thread>

#include <QtDebug>

#include <QFile>
#include <QDir>
#include <QTextStream>

#include "postbox.h"

struct egLogLetter // should be deleted after use
{
    int64_t  errorID;
    QString logMessage;
};

class egAsyncLoggerHandler
{
public:

   void egLogThreadLoop(QTextStream& egLoggingStream, postbox<egLogLetter> & egLogPostbox);

};

struct egLogFlush // flush the stream manipulator
{

};

class egAsyncLoggerEngine
{
public:

    postbox<egLogLetter> egLogPostbox;

    egAsyncLoggerEngine(const char* theLogFileName);
    ~egAsyncLoggerEngine();

    int LaunchLoggerEngine();  // tries to open file
    int64_t getLogID();

private:

    std::atomic<int64_t> idCounter {1};

    QString logFileName;

    QFile egLoggingFile;
    QTextStream egLoggingStream;

    egAsyncLoggerHandler theHandler;

    std::thread loggerThread;
};

// engine interface per thread
class egAsyncLogger
{
public:

    egAsyncLogger(const char* theLogFileName);

    egAsyncLogger(egAsyncLoggerEngine& logEngine);

    ~egAsyncLogger();

    int64_t egLogAsync(QString &logMessage);
    int64_t egLogAsyncFlush();

    inline egAsyncLogger &operator<<(QChar t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(bool t) { egStringBufStream << (t ? "true" : "false"); egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(char t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(signed short t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(unsigned short t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
#ifdef Q_COMPILER_UNICODE_STRINGS
    inline egAsyncLogger &operator<<(char16_t t) { egStringBufStream << QChar(ushort(t)); return *this; }
    inline egAsyncLogger &operator<<(char32_t t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
#endif
    inline egAsyncLogger &operator<<(signed int t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(unsigned int t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(signed long t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(unsigned long t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(qint64 t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(quint64 t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(float t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(double t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(const char* t) { egStringBufStream << QString::fromUtf8(t); egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(const QString & t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(const QStringRef & t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(QLatin1String t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(const QByteArray & t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
    inline egAsyncLogger &operator<<(const void * t) { egStringBufStream << t; egStringBufStream << ' '; return *this; }
#ifdef Q_COMPILER_NULLPTR
    inline egAsyncLogger &operator<<(std::nullptr_t) { egStringBufStream << "(nullptr)"; egStringBufStream << ' '; return *this; }
#endif
    inline egAsyncLogger &operator<<(QTextStreamFunction f) {
        egStringBufStream << f;
        return *this;
    }

    inline egAsyncLogger &operator<<(QTextStreamManipulator m) { egStringBufStream << m; return *this; }

        inline egAsyncLogger &operator<<(const QVariant & t) { egStringBufStream << t.toString(); egStringBufStream << ' '; return *this; }

    inline egAsyncLogger &operator<<(egLogFlush m) { (void) m; egLogAsyncFlush(); return *this; }

private:

    std::atomic<int64_t> idCounter {1};

    egAsyncLoggerEngine* theEngine {nullptr};

    QString logFileName;

    QFile egLoggingFile;
    QTextStream egLoggingStream;

    QString logBufferString;
    QTextStream egStringBufStream;

    postbox<egLogLetter> egLogPostbox;

    egAsyncLoggerHandler theHandler;

    std::thread loggerThread;
};

template <class T>
inline egAsyncLogger& operator<<(egAsyncLogger& theLogger, const QList<T> &c)
{
    theLogger << "(";

    auto it = c.begin(), end = c.end();
    if (it != end) {
        theLogger << *it;
        ++it;
    }
    while (it != end) {
        theLogger << ", " << *it;
        ++it;
    }

    theLogger << ") ";

    return theLogger;
}


template <class K, class V>
inline egAsyncLogger& operator<<(egAsyncLogger& theLogger, const QMap<K, V> &c)
{
    theLogger << "(";

    auto it = c.begin(), end = c.end();
    if (it != end) {
        theLogger << "[" << it.key() << ", " << it.value() << "]";
        ++it;
    }
    while (it != end) {
        theLogger << ", " << "[" << it.key() << ", " << it.value() << "]";
        ++it;
    }

    theLogger << ") ";

    return theLogger;
}

template <class K, class V>
inline egAsyncLogger& operator<<(egAsyncLogger& theLogger, const QHash<K, V> &c)
{
    theLogger << "(";

    auto it = c.begin(), end = c.end();
    if (it != end) {
        theLogger << "[" << it.key() << ", " << it.value() << "]";
        ++it;
    }
    while (it != end) {
        theLogger << ", " << "[" << it.key() << ", " << it.value() << "]";
        ++it;
    }

    theLogger << ") ";

    return theLogger;
}

#endif // EG_LOGGING_ASYNC_H
