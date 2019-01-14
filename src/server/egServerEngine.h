/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_SERVER_ENGINE_H
#define EG_SERVER_ENGINE_H


#include <QString>
#include <QList>
#include <QByteArray>

#include <QMutex>
#include <QtNetwork>
#include <QTcpServer>

#include <QThread>

#include "../egMetaInfo.h"
#include "../egClientServer.h"
#include "../egLocalFiles.h"
#include "../indexes/egIndexConditions.h"
#include "../egGraphDatabase.h"

#include "egServerOperProc.h"

class EgDbTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    EgDbTcpServer(QObject *parent = 0);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:

};

class EgDbServerThread : public QThread
{
    Q_OBJECT

public:
    EgDbServerThread(int socketDescriptor, QObject *parent);

    virtual ~EgDbServerThread();

    void run() override;

signals:
    // void error(QTcpSocket::SocketError socketError);

private:
    int socketDescriptor;

    EgServerOperProc* operProcessor = nullptr;
};


class EgServerEngine : public QObject // Data Files operations
{   
    Q_OBJECT

public:

    EgDbTcpServer customServer;

    EgServerEngine(); // QWidget *parent
    virtual ~EgServerEngine() { }

    void run(); // Qt Thread Pool compatible TODO : another custom thread pool (Workbox-type)

    // QTcpSocket* clientConnection;

private slots:
    void processRequest();

};


#endif // EG_SERVER_ENGINE_H
