#ifndef EG_SERVER_ENGINE_H
#define EG_SERVER_ENGINE_H

#include <QString>
#include <QList>
#include <QByteArray>

#include <QMutex>
#include <QtNetwork>

#include "egMetaInfo.h"
#include "egClientServer.h"
#include "egLocalFiles.h"

#include "objdb_filters.h"

class EgServerEngine  //  : public QRunnable // Data Files operations
{
public:
    EgDataFiles d_files;
    quint16 field_count;
    obj_id_type obj_count;
    obj_id_type next_obj_id;

    command_id_type command_id;
    odb_id_type db_id;

    qint32 filter_id;   // data filter callback ID
    int socketDescriptor;

    odb_id_type* glob_new_id;
    QMutex* OdbMapMutex;         // lock
    QMap<QString, odb_id_type>* glob_odb_map;
    QList<QString>* glob_odb_list;
        // data transfer
    QTcpSocket srvSocket;
    QByteArray block;

    QDataStream in;
    QDataStream out;

    QList<EgPackedDataNode> PackedList;
    EgFieldDescriptors FD;

    QList<EgDataNode*> data_obj_list;          // 1-element list to support local files interface
    QMap<obj_id_type, EgDataNode> dobj_map;    // map to send to client

        // data filter
    FilterInterface* filterInterface;

    EgServerEngine();
    ~EgServerEngine() {}

    // void Init();
    void Init();   // read stored Odb names from file
    void SetFileName(QString& FNameBase);   // set name for files
    bool loadPlugin();

    void run(); // Qt Thread Pool compatible TODO : another custom thread pool (Workbox-type)

        // field descriptors
    int ServerSendFieldDescs(QByteArray* field_descs, QByteArray* control_descs);    // send to client
        // service
    void LoadFilterArgs();
    int ServerSendOdbID();              // get ODB entry ID

    inline void ServerRecvDataObj();    // receive data obj to add/modify
    int ServerSendObjects();            // send data objects

};


#endif // EG_SERVER_ENGINE_H
