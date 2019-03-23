/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egDataClient.h"
#include <QHostAddress>

EgServerConnection::EgServerConnection():
   in(&tcpSocket),
   out(&block, QIODevice::WriteOnly),
   serverAddress(QHostAddress(QHostAddress::LocalHost).toString())
{
    // out.setVersion(QDataStream::Qt_4_0);    // FIXME check version hardcoded
    // in.setVersion(QDataStream::Qt_4_0);
}



EgServerConnection::EgServerConnection(const QString& aServerAddress):
    in(&tcpSocket),
    out(&block, QIODevice::WriteOnly),
    serverAddress(aServerAddress)
{
    // out.setVersion(QDataStream::Qt_4_0);    // FIXME check version hardcoded
    // in.setVersion(QDataStream::Qt_4_0);
}


int  EgServerConnection::OpenStoreStream(const CommandIdType command, QDataStream*& metaInfoStream, const QString& typeName)
{  
    int res = SendCommand(command, typeName);

    metaInfoStream = &out;

    return res;
}

int  EgServerConnection::OpenLoadStream(const CommandIdType command, QDataStream*& metaInfoStream, const QString& typeName)
{
    int res = SendCommand(command, typeName);

    metaInfoStream = &in;

    return res;
}

int  EgServerConnection::WaitForSending()
{
    tcpSocket.write(block);

    if (! tcpSocket.waitForBytesWritten(egServerTimeout)) // wait up to 10 sec
    {
        qDebug() << "waitForBytesWritten error" << FN;
        // process error
        return -1;
    }

    block.clear();
    out.device()->seek(0);

    return 0;
}

int  EgServerConnection::WaitForReadyRead()
{
    if (! tcpSocket.waitForReadyRead(egServerTimeout)) // wait up to 10 sec
    {
        qDebug() << FN << "waitForReadyRead error";
        // process error
        return -1;
    }

    return 0;
}

void  EgServerConnection::Disconnect()
{
    if (tcpSocket.state() == QAbstractSocket::ConnectedState)
        tcpSocket.disconnectFromHost();

    if (tcpSocket.state() == QAbstractSocket::ConnectedState)
        tcpSocket.waitForDisconnected(egServerTimeout);
}

int EgServerConnection::SendCommand(const CommandIdType command, const QString &nodeTypeName)
{
    // tcpSocket.abort();
    if (tcpSocket.state() != QAbstractSocket::ConnectedState)
        tcpSocket.connectToHost(serverAddress, server_port);
    else
        qDebug()  << "tcpSocket was not disconnected before" << FN;

    if (! tcpSocket.waitForConnected(egServerTimeout)) // wait up to 10 sec
    {
            // process error
        // qDebug() << "tcpSocket waitForConnected timeout"  << FN;
        qDebug() << "can't connect to the server: " << serverAddress << " port " << server_port << FN;
        return -2;
    }

        // send opcode
    block.clear();
    out.device()->seek(0);

    out << command;
    out << nodeTypeName;

    return 0;
}

/*
int EgServerConnection::RemoteStoreFieldDesc(QByteArray* field_descs, QByteArray* control_descs)
{
        // get/set egDataNodesTypeID
    */ /*
    if (RemoteGetOdbId())
    {
        qDebug() << FN << "RemoteGetOdbId got an error";
        return -2;
    }
    */
    // qDebug() << FN << "opcode_store_fdesc";

    /*
        // send opcode
    out << opcode_store_metainfo;  // operation code

    tcpSocket.write(block);     // clean up
    block.clear();
    out.device()->seek(0);
    */

    // out << (qint16) field_descs->size();

    /*
    tcpSocket.write(*field_descs);        // packed field descriptors

    out << (qint16) control_descs->size();
    tcpSocket.write(block);
    block.clear();              // clean up

    if(control_descs->size())
    tcpSocket.write(*control_descs);      // packed control descriptors

    // tcpSocket.write(*field_descs);        // packed field descriptors
    // tcpSocket.write(*control_descs);      // packed control descriptors


    // out.device()->seek(0);

    tcpSocket.waitForDisconnected();

    // qDebug() << FN << "Disconnected";

    return 0;
}
*/

/*
int EgDataClient::StoreData(QList<EgPackedDataNode*>& a_list, QList<EgPackedDataNode*>& d_list, QList<EgPackedDataNode*>& m_list)
{
    int error_code1 = 0;
    int error_code2 = 0;
    int error_code3 = 0;
        // new records
    if (a_list.count())
        error_code1 = SendObjData(a_list, opcode_append_dataobj);
        // deleted
    if (d_list.count())
        error_code2 = SendObjData(d_list, opcode_delete_dataobj);
        // changed
    if (m_list.count())
        error_code3 = SendObjData(m_list, opcode_update_dataobj);

    if (error_code1 | error_code2 | error_code3)
       qDebug() << FN << "ERROR: got non-zero error code from callee" ;

    return error_code1 | error_code2 | error_code3;
}
*/

/*
int EgServerConnection::RemoteLoadFieldDesc(QByteArray* field_descs, QByteArray* control_descs, EgDataNodeIDtype& obj_count, EgDataNodeIDtype& next_obj_id)
{
    qint16 a_size = 0;

        // get/set egDataNodesTypeID
    */ /*if (RemoteGetOdbId())
    {
        qDebug() << FN << "RemoteGetOdbId got an error";
        return -1;
    }
    */
    // qDebug() << FN << "opcode_load_fdesc";

    /*
        // send opcode
    out << opcode_load_metainfo; // operation code
    tcpSocket.write(block);
    block.clear();
    out.device()->seek(0);
        // start read
    if (! tcpSocket.waitForReadyRead(egServerTimeout)) // wait up to 10 sec
    {
        qDebug() << FN << "waitForReadyRead error";
        // process error
        return -2;
    }

    field_descs->clear();
    control_descs->clear();

    in >> obj_count;       // data objects (NOT field descriptors) count
    in >> next_obj_id;     // next_obj_id

    in >> *field_descs;    // packed field descriptors
    in >> a_size;
    // qDebug() << FN << "control desc size =" << a_size;
    if (a_size)
        in >> *control_descs; // packed control descriptors

    tcpSocket.disconnectFromHost();

    // qDebug() << FN << *field_descs;
    // qDebug() << FN << *control_descs;

    return 0;
}
*/

/*
int EgDataClient::RemoteStoreData(QList<DataObj*>& a_list, QList<DataObj*>& d_list, QList<DataObj*>& m_list)
{
    int error_code1 = 0;
    int error_code2 = 0;
    int error_code3 = 0;
        // new records
    if (a_list.count())
        error_code1 = RemoteSendObjData(a_list, opcode_append_dataobj);
        // deleted
    if (d_list.count())
        error_code2 = RemoteSendObjData(d_list, opcode_delete_dataobj);
        // changed
    if (m_list.count())
        error_code3 = RemoteSendObjData(m_list, opcode_update_dataobj);

    if (error_code1 | error_code2 | error_code3)
       qDebug() << FN << "ERROR: got non-zero error code from callee" ;

    return error_code1 | error_code2 | error_code3;
}


inline int EgDataClient::RemoteSendObjData(QList<DataObj*>& a_list, command_id_type command)
{
    QList<DataObj*>::iterator cur_obj;
        // get id
    if (RemoteGetOdbId())
    {
        qDebug() << FN << "RemoteGetOdbId got an error";
        return -1;
    }
    // qDebug() << FN << "opcode: " << command;
        // send opcode
    out << command; // operation code
    tcpSocket.write(block);
    block.clear();
    out.device()->seek(0);
        // send recs count
    out << (quint32) a_list.count();
    tcpSocket.write(block);
    block.clear();
    out.device()->seek(0);
        // walk the list
    cur_obj = a_list.begin();
    while (cur_obj != a_list.end())
    {
        // qDebug() << FN << " ID " << (*cur_obj)-> OBJ_ID;
            // send as bytearray
        out << (EgDataNodeIDtype)(*cur_obj)-> OBJ_ID;
        out << (DataObj&) *(*cur_obj);
        tcpSocket.write(block);
        block.clear();
        out.device()->seek(0);

        cur_obj++;
    }

    tcpSocket.waitForDisconnected();

    return 0;
}
*/

/*
inline int EgDataClient::SendObjData(QList<EgPackedDataNode*>& a_list, command_id_type command)
{
    QList<EgPackedDataNode*>::iterator cur_obj;
        // get id
    if (RemoteGetOdbId())
    {
        qDebug() << FN << "RemoteGetOdbId got an error";
        return -1;
    }
    // qDebug() << FN << "opcode: " << command;
        // send opcode
    out << command; // operation code
    tcpSocket.write(block);
    block.clear();
    out.device()->seek(0);
        // send recs count
    out << (quint32) a_list.count();
    tcpSocket.write(block);
    block.clear();
    out.device()->seek(0);
        // walk the list
    cur_obj = a_list.begin();
    while (cur_obj != a_list.end())
    {
        // qDebug() << FN << " ID " << (*cur_obj)-> OBJ_ID;
            // send as bytearray
        out << (EgDataNodeIDtype)(*cur_obj)-> OBJ_ID;
        tcpSocket.write(block);
        block.clear();

        // out.device()->seek(0);

        tcpSocket.write((*cur_obj)-> bar); // (QByteArray&)

        // FIXME - send indexes to update

        cur_obj++;
    }

    tcpSocket.waitForDisconnected();

    return 0;
}
*/

int EgServerConnection::RemoteLoadData()
{
    /*
    QByteArray bar;
    EgDataNode tmpObj;
    qint32 rec_count = 0;
        // init
    tmpObj.field_descs = &(d_class->FD);
    tmpObj.data_obj_status = is_unchanged;
        // get ID
    if (RemoteGetOdbId())
    {
        qDebug() << FN << "RemoteGetOdbId got an error";
        return -1;
    }

        // check if filter exists
    if (d_class->RemoteFilterID)
    {
        // qDebug() << FN << "opcode_load_filtered";
            // send opcode
        out << opcode_load_filtered; // operation code
        tcpSocket.write(block);
        block.clear();
        out.device()->seek(0);
            // send filter ID
        out << (qint32) d_class->RemoteFilterID; // filter ID
        tcpSocket.write(block);
        block.clear();
        out.device()->seek(0);
            // send arguments count
        RemoteSendList(d_class->filter_values);
    }
    else
    {
        // qDebug() << FN << "opcode_load_data";
            // send opcode
        out << opcode_load_data; // operation code
        tcpSocket.write(block);
        block.clear();
        out.device()->seek(0);
    }
        // start read
    if (! tcpSocket.waitForReadyRead(egServerTimeout)) // wait up to 10 sec
    {
        qDebug() << FN << "waitForReadyRead error";
        return -2;
    }
        // get records count
    in >> rec_count;
    // qDebug() << FN << "rec_count = " << rec_count;
        // get records
    for (int i = 0; i < rec_count; i++)
    {

        in >> tmpObj.OBJ_ID;
        in >> bar;  // QByteArray
        bar >> tmpObj;

        d_class->dobj_map.insert(tmpObj.OBJ_ID, tmpObj);
    }
    // d_class->PrintObjData(); // debug

    tcpSocket.disconnectFromHost();
*/
    return 0;
}

int EgServerConnection::RemoteSendList(QList<QVariant>& d_list) // send variant list
{
    // qint16 arg_size;
        // send arguments count
    out << (qint16) d_list.count();
    tcpSocket.write(block);
    block.clear();
    out.device()->seek(0);

        // send arguments
    for (int i = 0; i < d_list.count(); i++)
    {
            // case type
        switch ( d_list[i].type() )
        {
        case QVariant::Int :
                // send size
            out << (qint32) 0; // int flag is zero size
            tcpSocket.write(block);
            block.clear();
            out.device()->seek(0);
                // send value
            out << (qint32) d_list[i].toInt();
            tcpSocket.write(block);
            block.clear();
            out.device()->seek(0);
            break;
        case QVariant::String :
            out << (qint32) d_list[i].toString().length();
            out.writeRawData( d_list[i].toString().toLocal8Bit(), d_list[i].toString().length()); //data().toByteArray();
            tcpSocket.write(block);
            block.clear();
            out.device()->seek(0);
            break;
        default:
                // process error
            qDebug() << FN << "RemoteSendList(): not supported QVariant subtype";
        }
    }

    return 0;
}
