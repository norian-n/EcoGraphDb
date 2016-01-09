/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/


#include "odb_server.h"

#include <stdlib.h>

OdbServer::OdbServer(QObject *parent)
    : QTcpServer(parent),
    global_new_id(1)
{
    global_req_processor.glob_odb_map = &global_odb_map;
    global_req_processor.glob_odb_list = &global_odb_list;
    global_req_processor.glob_new_id = &global_new_id;
    global_req_processor.Init();

    // threadPool = QThreadPool::globalInstance();
}

void OdbServer::incomingConnection(int socketDescriptor)
{
    OdbThread *thread = new OdbThread(socketDescriptor, this); // FIXME : thread pool
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    thread-> req_processor.OdbMapMutex = &GlobOdbMapMutex;
    thread-> req_processor.glob_odb_map = &global_odb_map;
    thread-> req_processor.glob_odb_list = &global_odb_list;
    thread-> req_processor.glob_new_id = &global_new_id;

    thread-> req_processor.srvSocket.moveToThread(thread); // to prevent warning messages

    thread-> req_processor.socketDescriptor = socketDescriptor;

    thread-> start();
}

/*
void OdbServer::incomingConnection(int socketDescriptor)
{
    req_processor = new ServerFiles;

    req_processor-> OdbMapMutex = &GlobOdbMapMutex;
    req_processor-> glob_odb_map = &global_odb_map;
    req_processor-> glob_odb_list = &global_odb_list;
    req_processor-> glob_new_id = &global_new_id;

    req_processor-> socketDescriptor = socketDescriptor;

    threadPool->start(req_processor);
}
*/
