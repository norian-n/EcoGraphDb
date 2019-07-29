/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egServerEngine.h"
#include <QThread>

EgServerEngine::EgServerEngine()
    // field_count(0),
    // filter_id(-1),
    // glob_odb_map(NULL),
    // in(&srvSocket),
    // out(&block, QIODevice::WriteOnly),
    // tcpServer(new QTcpServer(this))
{
    // out.setVersion(QDataStream::Qt_4_0);
    // in.setVersion(QDataStream::Qt_4_0);

    // connect(tcpServer, &QTcpServer::newConnection, this, &EgServerEngine::processRequest);
}


EgDbTcpServer::EgDbTcpServer(QObject *parent)
      : QTcpServer(parent)
{

}


void EgDbTcpServer::incomingConnection(qintptr socketDescriptor)
{
    // EG_LOG_STUB << "incomingConnection() called" << FN;

    EgDbServerThread *thread = new EgDbServerThread(socketDescriptor, this);

    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread->start();

    // clientConnection = tcpServer->nextPendingConnection();

    // connect(clientConnection, &QAbstractSocket::readyRead, this, &EgServerEngine::getCommand);
}

EgDbServerThread::EgDbServerThread(int socketDescriptor, QObject *parent)
    : QThread(parent)
    , socketDescriptor(socketDescriptor)
    , operProcessor(new EgServerOperProc())
{
}

EgDbServerThread::~EgDbServerThread()
{
    if (operProcessor)
        delete operProcessor;
}

void EgDbServerThread::run()
{
    // operProcessor->clientConnection-> setParent(this);

    // operProcessor->clientConnection-> moveToThread(this);

    operProcessor-> createSocket();

    if (! operProcessor->clientConnection-> setSocketDescriptor(socketDescriptor)) {
        EG_LOG_STUB  << "Socket descriptor error " << FN ;
        return;
    }

    // EG_LOG_STUB  << "Thread started " << FN ;

    operProcessor-> processCommand();
}

void EgServerEngine::processRequest()
{
    // QByteArray block;
    // QDataStream out(&block, QIODevice::WriteOnly);
    // out.setVersion(QDataStream::Qt_4_0);

    // CommandIdType command_id;

    // EG_LOG_STUB << "processRequest() called";
/*
    clientConnection = tcpServer->nextPendingConnection();

    connect(clientConnection, &QAbstractSocket::readyRead, this, &EgServerEngine::getCommand);
    connect(clientConnection, &QAbstractSocket::disconnected, clientConnection, &QObject::deleteLater);
*/
    return;
}

    // primary processor
// int EgServerEngine::Execute(int socketDescriptor)                     // execute client's request
void EgServerEngine::run()
{
    // if (!tcpServer-> listen(QHostAddress::LocalHost, server_port))
    if (! customServer.listen(QHostAddress::LocalHost, server_port))
    {
        // EG_LOG_STUB << "Unable to start the server: " << tcpServer-> errorString();
        EG_LOG_STUB << "Unable to start the server: " << customServer.errorString();

        return;
    }



    // QString ipAddress = QHostAddress(QHostAddress::LocalHost).toString(); << ipAddress

    // EG_LOG_STUB << "The egDb server is running on"  << " port " << tcpServer-> serverPort();
    EG_LOG_STUB << "The egDb server is running on"  << " port " << customServer.serverPort();
}

