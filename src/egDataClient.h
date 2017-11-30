/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EG_DATA_CLIENT_H
#define EG_DATA_CLIENT_H

#include <QByteArray>
#include <QList>
#include <QVariant>
#include <QTcpSocket>

#include "egDataNodesType.h"
#include "egClientServer.h"

class EgDataClient // Data Conection Operations
{
public:
    EgDataNodesType* dataNodesType;
    QTcpSocket tcpSocket;
    QByteArray block;

    QDataStream in;
    QDataStream out;

    EgDataNodesTypeID egDataNodesTypeID; // server odb ID

    EgDataClient(const EgDataNodesType* nodesType);
    ~EgDataClient() {}

    int RemoteGetOdbId();             // get odb_id from server

    int RemoteLoadFieldDesc(QByteArray* field_descs, QByteArray* control_descs, EgDataNodeIDtype& obj_count, EgDataNodeIDtype& next_obj_id);    // load from server
    int RemoteStoreFieldDesc(QByteArray* field_descs, QByteArray* control_descs);

    // inline void* RemoteSendByteArray(QByteArray* ba);

    // int StoreData(QList<EgPackedDataNode*>& a_list, QList<EgPackedDataNode*>& d_list, QList<EgPackedDataNode*>& m_list); // new primary index support

    // inline int SendObjData(QList<EgPackedDataNode*>& a_list, command_id_type command);

    // int RemoteStoreData(QList<DataObj*>& a_list, QList<DataObj*>& d_list, QList<DataObj*>& m_list); 	// add, modify, delete - auto update
    int RemoteLoadData();       // select * from DataObjectsRecords

    // inline int RemoteSendObjData(QList<DataObj*>& a_list, command_id_type command);
    int RemoteSendList(QList<QVariant>& d_list); // send variants list FIXME replace to byte array

};

#endif // EG_DATA_CLIENT_H
