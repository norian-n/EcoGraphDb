#include "serverfiles.h"

#include <QtDebug>

ServerFiles::ServerFiles():
        field_count(0),
        filter_id(-1),
        glob_odb_map(NULL),
        in(&srvSocket),
        out(&block, QIODevice::WriteOnly)
{
    out.setVersion(QDataStream::Qt_4_0);
    in.setVersion(QDataStream::Qt_4_0);
}

void ServerFiles::Init()   // read stored Odb names from file - do it ONCE on server launch, not for each thread
{ 
    // mutex lock is not required, init called only once on server start
    qint32 rSize;
    char buf[2048]; // FIXME
    QString odb_map_name;

    QFile ddt_file("odb_names.onf");
    QDataStream ddt(&ddt_file);
        // check global ID counter
    if (! glob_new_id)
    {
        qDebug() << "ServerFiles::Init() got bad glob_new_id ptr";
        return;
    }
        // check global map ptr
    if (! glob_odb_map)
    {
        qDebug() << "ServerFiles::Init() got bad glob_odb_map ptr ";
        return;
    }
        // init ID counter
    *glob_new_id = 1;
        // clear map and list
    glob_odb_map->clear();
    glob_odb_list->clear();

    if (!ddt_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "ServerFiles::Init() can't open " << "odb_names.onf" << " file";
        return;
    }
        // read odb names from file
    while (! ddt.atEnd())
    {
        ddt >> rSize;
        // qDebug() << "rSize= " << rSize;
        ddt.readRawData(buf, rSize);
        buf[rSize] = 0;
        odb_map_name = QString(buf); // codec->toUnicode(buf);

        glob_odb_map->insert(odb_map_name, (*glob_new_id)++);
        glob_odb_list->append(odb_map_name);  // sync list for backward access
    }

    ddt_file.close();

}
    // set obj databse name for local file operations
void ServerFiles::SetFileName(QString& FNameBase)
{
    // FileNameBase  = FNameBase;

    d_files.Init(FNameBase, &FD);
        // load from file TODO : make it global ?
    // d_files.LocalLoadFieldDesc2(desc_list, field_indexes, field_count, obj_count, next_obj_id); FIXME
}
    // load filter callbacks plugin
bool ServerFiles::loadPlugin()
{
    QDir pluginsDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = pluginLoader.instance();
        if (plugin) {
            filterInterface = qobject_cast<FilterInterface *>(plugin);
            if (filterInterface)
            {
                // filterInterface->PingFilter();
                // LocalFilterCallback = filterInterface->FilterByID(1);
                return true;
            }
        }
    }

    return false;
}
    // primary processor
// int ServerFiles::Execute(int socketDescriptor)                     // execute client's request
void ServerFiles::run()
{
    // quint16 desc_count = 0;
    odb_id_type  odb_id = 0; // 0 means bad Odb ID
    qint16 a_size = 0;

    QByteArray field_descs, control_descs;

    if ( !loadPlugin())
        qDebug() << FN << "can't load plugins ";

    qDebug() << FN << &srvSocket;

    srvSocket.moveToThread(QThread::currentThread());

    qDebug() << FN << QThread::currentThread();

    if (!srvSocket.setSocketDescriptor(socketDescriptor)) {
        qDebug() << FN << "can't set socket descriptor";
        // emit error(srvSocket.error());
        return; // -1;
    }
        // start read
    if (! srvSocket.waitForReadyRead(10000)) // wait up to 10 sec
    {
        qDebug() << FN << "waitForReadyRead error 1";
      // process error
        return; // -2;
    }

        // get/set database ID
    in >> command_id;
    // qDebug() << "command_id 1 = " << command_id;
        // process first command - opcode_get_odb_id or opcode_send_odb_id
    switch ( command_id )
    {
    case opcode_get_odb_id:
        ServerSendOdbID();  // get ID by name and send to client
        // start read
        if (! srvSocket.waitForReadyRead(10000)) // wait up to 10 sec
        {
            qDebug() << FN << "waitForReadyRead error 2";
            // process error
            return; // -3;
        }
        break;
    case opcode_send_odb_id:      // get ID from client
        in >> odb_id;
        // qDebug() << "odb_id = " << odb_id;
            // TODO: get odb name by odb_id
        SetFileName((QString&) glob_odb_list->at(odb_id-1));
        // qDebug() << "file_name_base = " << FileNameBase;
        break;
    default:
        qDebug() << FN << "ERROR: bad opcode " << command_id;
        srvSocket.disconnectFromHost();
        return; // -4;
        break;
    }
        // get (second) command
    in >> command_id;
    // qDebug() << FN << "command_id 2 = " << command_id;

        // process command
    switch ( command_id )
    {
    case opcode_store_fdesc: // store field descriptors on server

        field_descs.clear();
        control_descs.clear();

        // in >> a_size;
        // qDebug() << FN << "field desc size =" << a_size;
        // if (a_size)
            in >> field_descs;

        in >> a_size;
        // qDebug() << FN << "control desc size =" << a_size;
        if (a_size)
            in >> control_descs;

        srvSocket.disconnectFromHost();
        // qDebug() << FN << "disconnect ";

        d_files.RemoveLocalFiles(); // total destruction
        d_files.LocalStoreFieldDesc(&field_descs, &control_descs);

        break;
    case opcode_load_fdesc:  // load field descriptors from server

        d_files.LocalLoadFieldDesc(&field_descs, &control_descs, obj_count, next_obj_id);    // load from file
        ServerSendFieldDescs(&field_descs, &control_descs);    // send to client
        srvSocket.waitForDisconnected();
        break;
    case opcode_append_dataobj:  // append new data obj to file

        ServerRecvDataObj();       // receive the list
        srvSocket.disconnectFromHost();

        // d_files.AppendNewData(PackedList); FIXME

        break;
    case opcode_delete_dataobj:  // mark data obj as deleted

        ServerRecvDataObj();       // receive the list
        srvSocket.disconnectFromHost();

        // d_files.DeleteObjects(PackedList); FIXME

        break;
    case opcode_update_dataobj:  // append new data obj to file

        ServerRecvDataObj();       // receive the list
        srvSocket.disconnectFromHost();

        // d_files.UpdateModifiedData2(PackedList); FIXME

        break;
    case opcode_load_data:  // append new data obj to file

        // d_files.LocalLoadData(PackedList, FD); // load data FIXME

        ServerSendObjects();                    // send it
        srvSocket.waitForDisconnected();
        break;
    case opcode_load_filtered:  // append new data obj to file
            // process filter
        in >> filter_id;    // download filter ID
        qDebug() << FN << "filter_id = " << filter_id;
        // d_files.FilterCallback = filterInterface->FilterByID(filter_id); // set filter callback
            // get filter arguments
        LoadFilterArgs();

        // d_files.LocalLoadData(PackedList, FD); // load data FIXME

        ServerSendObjects();                    // send it
        srvSocket.waitForDisconnected();

        break;
    default:
        qDebug() << FN << "ERROR: bad command_id = " << command_id;
        srvSocket.disconnectFromHost();
        return; // -6;
        break;
    }

    return; // 0;
}

void ServerFiles::LoadFilterArgs()
{
    qint16 argsCount;
    qint32 rSize, intValue;
    char buf[2048]; // FIXME
    // QTextCodec *codec = QTextCodec::codecForName("Windows-1251");

    // d_files.filter_values.clear();
        // get args count
    in >> argsCount;
    qDebug() << "LoadFilterArgs(): argsCount = " << argsCount;
        // get args
    for(int i = 0; i < argsCount; i++)
    {
        in >> rSize;   // TODO : remove stub
        qDebug() << "LoadFilterArgs(): value = " << rSize;
            // zero is int flag
        if (!rSize) // int
        {
            in >> intValue;
            // d_files.filter_values << intValue;
        }
        else
        {
            if ((rSize >= 2048)) // FIXME
            {
                qDebug() <<  "LoadFilterArgs(): fSize >= MAX_BUF_SIZE " << rSize;
                return;  // TODO : process error
            }
                // read string
            in.readRawData(buf, rSize);
            buf[rSize] = 0;
            //d_files.filter_values << codec->toUnicode(buf);
        }
    }
}


int ServerFiles::ServerSendObjects()    // FIXME send objects
{
    QList<PackedDataObj>::iterator cur_obj;
        // send recs count
    out << (quint32) PackedList.count();
    srvSocket.write(block);
    block.clear();
    out.device()->seek(0);
        // send data
    cur_obj = PackedList.begin();
    while (cur_obj != PackedList.end())
    {
        out << (*cur_obj).OBJ_ID;
        srvSocket.write(block);
        block.clear();
        out.device()->seek(0);

        out << (*cur_obj).bar;
        srvSocket.write(block);
        block.clear();
        out.device()->seek(0);

        cur_obj++;
    }
    return 0;
}

inline void ServerFiles::ServerRecvDataObj()
{
    qint32 obj_count;

    PackedList.clear();
    PackedDataObj tmpObj;

    in >> obj_count;
    qDebug() << FN << "obj_count = " << obj_count;
        // get args
    for(int i = 0; i < obj_count; i++)
    {
        in >> tmpObj.OBJ_ID;
        in >> tmpObj.bar;

        PackedList << tmpObj;
    }
}


int ServerFiles::ServerSendOdbID() 		// find or create new ODB entry in global map and save it to file
{
    QString odb_name;
    odb_id_type  odb_id = 0; // 0 means bad Odb ID
        // object description data
    QFile ddt_file("odb_names.onf");
    QDataStream ddt(&ddt_file);
        // get name from client
    in >> odb_name;
    // qDebug() << FN << "Got odb_name = " <<  odb_name;
    SetFileName(odb_name);
        // lock mutex
    OdbMapMutex->lock();
        // search for name // ??? check pointer if (!glob_odb_map)
    QMap<QString, odb_id_type>::iterator my_field = glob_odb_map->find(odb_name);
    if (my_field != glob_odb_map->end())
    {
        odb_id = my_field.value();
            // unlock mutex - branch 1
        OdbMapMutex->unlock();
            // send odb_id
        out << odb_id;
        srvSocket.write(block);
            // clear after use
        block.clear();
        out.device()->seek(0);

        // qDebug() << "ServerSendOdbID(): Sent old odb_id = " <<  odb_id;

        return 0;
    }
        // TODO : check Odb files existance

        // get new odb_id
    odb_id = (*glob_new_id)++; // TODO : check pointer if (!glob_new_id)
        // add new record
    glob_odb_map->insert(odb_name, odb_id);
    glob_odb_list->append(odb_name);  // sync list for backward access
        // add to file
        // open file
    if (!ddt_file.open(QIODevice::Append)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << FN << "can't open " << "odb_names.onf" << " file";
        return -1;
    }
        // append to end
    // ddt.device()->seek(ddt.device()->size());
    ddt << (qint32) odb_name.length();
    ddt.writeRawData( odb_name.toLocal8Bit(), odb_name.length()); //data().toByteArray();
        // close file
    ddt_file.close();
        // unlock mutex - branch 2
    OdbMapMutex->unlock();
        // send odb_id - new value
    out << odb_id;
    srvSocket.write(block);
        // clear after use
    block.clear();
    out.device()->seek(0);

    qDebug() << FN << "Sent new odb_id " <<  odb_id;

    return 0;
}

int ServerFiles::ServerSendFieldDescs(QByteArray* field_descs, QByteArray* control_descs)    // send field descriptors to client app
{
        // send recs count
    // qDebug() << FN << "data objects count = " << obj_count;
    out << obj_count;
    out << next_obj_id;
    srvSocket.write(block);
    block.clear();
    out.device()->seek(0);

    srvSocket.write(*field_descs);

    out << (qint16) control_descs->size();
    srvSocket.write(block);
    block.clear();
    out.device()->seek(0);

    if (control_descs->size())
        srvSocket.write(*control_descs);

    return 0;
}

// =============================================================================================
//                              JUNKYARD
// =============================================================================================

/*
int ServerFiles::SendSingleObject(DataObj& cur_obj)
{
    QList<QVariant>::iterator cur_field;
    // quint16 blockSize = 0;
    quint16 f_index = 0;
        // send ID
    out << (quint32) cur_obj.OBJ_ID;
    srvSocket.write(block);
    block.clear();
    out.device()->seek(0); */
         // send size
    /*out << (qint32)  (cur_obj.data_obj_size); // record is not deleted, positive
    srvSocket.write(block);
    block.clear();
    out.device()->seek(0); */
    /*
         // send fields
    cur_field = cur_obj.data_fields.begin();
    while (cur_field != cur_obj.data_fields.end())
    {
        // check field type
        switch ( desc_list[f_index].field_type ) // d_class->field_desc_list
        {
        case d_int32 :
        case d_link  :
            out << (qint32)(*cur_field).toInt();
            break;
        case d_float:
            out << (*cur_field).toFloat();
            break;
        case d_date :
            out << (qint32) cur_obj.DateToInt(*cur_field);
            break;
        default: // d_string
            out << (qint32) (*cur_field).toString().length();
            out.writeRawData( (*cur_field).toString().toLocal8Bit(), (*cur_field).toString().length()); //data().toByteArray();
            // qDebug() << "to sent: " << (*cur_field).toString();
            break;
        }

        srvSocket.write(block);
        block.clear();
        out.device()->seek(0);

        cur_field++;
        f_index++;
    }

    return 0;
}
*/


/*
int ServerFiles::ServerSendSingleDesc(DFieldDesc& f_desc)
{
    out << (quint32) f_desc.field_name.length();
    out.writeRawData( f_desc.field_name.toLocal8Bit(), f_desc.field_name.length()); //data().toByteArray();

    srvSocket.write(block);
    block.clear();
    out.device()->seek(0);

    // out << (quint32) f_desc.field_label.length();
    // out.writeRawData( f_desc.field_label.toLocal8Bit(), f_desc.field_label.length()); //data().toByteArray();

    srvSocket.write(block);
    block.clear();
    out.device()->seek(0);

    out << (qint8)f_desc.field_type;

    srvSocket.write(block);
    block.clear();
    out.device()->seek(0);

    return 0;
}
*/

/*
int ServerFiles::ServerRecvSingleDesc(DFieldDesc& f_desc)
{
    quint32 blockSize;
    qint8 f_type;
    char buffer[MAX_BUF_SIZE];

    in >> blockSize;

    if (srvSocket.bytesAvailable() < blockSize)
    {
        qDebug() << "ServerRecvSingleDesc(): tcpSocket data size mismatch: " << srvSocket.bytesAvailable() << " available, required " << blockSize;
        return -1;
    }

    // QString test_str;
    if (blockSize < MAX_BUF_SIZE)
    {
        in.readRawData (buffer, blockSize);
        buffer[blockSize] = 0;
    }

    // qDebug() << "Got name: " <<  buffer;
    f_desc.field_name = QString(buffer);

    in >> blockSize;

    if (srvSocket.bytesAvailable() < blockSize)
    {
        qDebug() << "ServerRecvSingleDesc(): tcpSocket data size mismatch: " << srvSocket.bytesAvailable() << " available, required " << blockSize;
        return -1;
    }

    // QString test_str;
    if (blockSize < MAX_BUF_SIZE)
    {
        in.readRawData (buffer, blockSize);
        buffer[blockSize] = 0;
    }

    // qDebug() << "Got label: " <<  buffer;
    // f_desc.field_label = QString(buffer);

    in >> f_type;
    // qDebug() << "Got type: " <<  f_type;
    f_desc.field_type = (DType)f_type; // <static_cast>

    return 0;
}
*/

/*
int ServerFiles::ServerRecvFieldDescs()    // receive field descriptors from client app
{
    QByteArray field_descs, control_descs;

    in >> field_descs;
    in >> control_descs;

    d_files.LocalStoreFieldDesc2(&field_descs, &control_descs);

    return 0;
}
*/

/*
int ServerFiles::ServerAppendDataObj()
{
    QList<DataObj*>::iterator cur_obj;
        // TODO lock table

        // data objects file
    QFile data_file(FileNameBase + ".dat");
    QDataStream dat(&data_file);
        // primary index table
    QFile pit_file(FileNameBase + ".pit");
    QDataStream pit(&pit_file);
        // open files
    if (!data_file.open(QIODevice::Append)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "ServerStoreData():can't open " << FileNameBase << ".dat" << " file";
        return -1;
    }
    if (!pit_file.open(QIODevice::Append)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "ServerStoreData():can't open " << FileNameBase << ".pit" << " file";
        return -1;
    } */
        // TODO : update data definition table in other method
    /*
        // object description data
    QFile ddt_file(DataDescName);
    QDataStream ddt(&ddt_file);

    if (!ddt_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "ServerStoreData():can't open " << DataDescName << " file";
        return -1;
    }
            // update ddt
        ddt.device()->seek(0);
        ddt << obj_count;
        ddt << next_obj_id;

    ddt_file.close();
     */
        // add new records

       // append to end
   // pit.device()->seek(pit.device()->size());
   // dat.device()->seek(dat.device()->size());
 /*      // parse table
   cur_obj = data_obj_list.begin();
   if (cur_obj != data_obj_list.end())
   {
       // qDebug() << "ServerStoreData(): Add position " << dat->device()-> pos() <<  " ID " << (*cur_obj)-> OBJ_ID;
           // primary index file
       pit << (quint32) (*cur_obj)-> OBJ_ID;
       pit << (qint32)  ((*cur_obj)->data_obj_size); // record is not deleted, positive // TODO : calc size on receive
       pit << dat.device()->pos();

       // d_files.WriteObjData((*cur_obj), dat, desc_list);  // TODO : load desc list from file, then change to global link
   }
        // close all
    data_file.close();
    pit_file.close();
        // TODO : unlock table

    return 0;
}
*/
/*
int ServerFiles::ServerUpdateDataObj()
{
    QList<DataObj*>::iterator cur_obj;
    qint64 data_pos;
    qint32 old_size;

    // TODO lock table

        // data objects file
    QFile data_file(FileNameBase + ".dat");
    QDataStream dat(&data_file);
        // primary index table
    QFile pit_file(FileNameBase + ".pit");
    QDataStream pit(&pit_file);
        // open files
    if (!data_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "ServerUpdateDataObj():can't open " << FileNameBase << ".dat" << " file";
        return -1;
    }
    if (!pit_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "ServerUpdateDataObj():can't open " << FileNameBase << ".pit" << " file";
        return -1;
    }

    cur_obj = data_obj_list.begin();
    if (cur_obj != data_obj_list.end())
    {
        // look for position by OBJ_ID in index file
        d_files.go_pit_offset2((*cur_obj)-> OBJ_ID, pit); //
        // pit->device()->seek(pit->device()->pos() + 4);  //  TODO : change constants
        // qDebug() << "ServerUpdateDataObj():pit_pos = " << pit_pos;
        if (! pit.atEnd())
        {
            pit << (qint32) (*cur_obj)->data_obj_size;  // update size
            // get position in data file
            pit >> data_pos;
            // qDebug()  << "ServerUpdateDataObj():Delete to modify : OBJ_ID = " << (*cur_obj)-> OBJ_ID << ", pit file obj_id = " << dbg_obj_id << ",pos = " << data_pos;
            dat.device()->seek(data_pos + 4); // TODO : change number to constant
            if (! dat.atEnd())
            {
                dat >> old_size;                   // get old
                dat.device()->seek(data_pos + 4);  // step back
                dat << (qint32) -old_size; // mark data as deleted - negative size
            }
            else            // ERROR - position not found in data file, log it
                qDebug()  << "ERROR: ServerUpdateDataObj(): data_pos + 4 = " << data_pos + 4 << " not found in data file ";
                // add new data record
            dat.device()->seek(dat.device()->size());
                // set new position in index file
            pit.device()->seek(pit.device()->pos() - 8); // TODO : change constants
            pit << dat.device()-> pos();

            // d_files.WriteObjData(*cur_obj, dat, desc_list);
        }
        else
        {
            // ERROR - OBJ_ID not found in index file, log it
            qDebug()  << "ERROR: ServerUpdateDataObj(): OBJ_ID = " << (*cur_obj)-> OBJ_ID << " not found in index file ";
        }
        // cur_obj++;
    }
        // close all
    data_file.close();
    pit_file.close();
        // TODO : unlock table

    return 0;
}
*/
/*
int ServerFiles::ServerDeleteDataObj()
{
    QList<DataObj*>::iterator cur_obj;
        // TODO lock table

        // data objects file
    QFile data_file(FileNameBase + ".dat");
    QDataStream dat(&data_file);
        // primary index table
    QFile pit_file(FileNameBase + ".pit");
    QDataStream pit(&pit_file);
        // open files
    if (!data_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "ServerStoreData():can't open " << FileNameBase << ".dat" << " file";
        return -1;
    }
    if (!pit_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "ServerStoreData():can't open " << FileNameBase << ".pit" << " file";
        return -1;
    }
    cur_obj = data_obj_list.begin();
    if (cur_obj != data_obj_list.end())
    {
        // d_files.MarkObjectToDelete2((*cur_obj), pit); // FIXME
    }
         // close all
    data_file.close();
    pit_file.close();
         // TODO : unlock table
    return 0;
}
*/


// write field descriptions to file
/*
int ServerFiles::ServerStoreFieldDescs()
{
    QList<DFieldDesc>::iterator cur_desc;
        // lock table

        // object description data
    QFile ddt_file(FileNameBase + ".ddt");
    QDataStream ddt(&ddt_file);
    if (!ddt_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "can't open " << FileNameBase << ".ddt" << " file";
        return -1;
    }

    ddt << (qint32) 0; // obj_count
    ddt << (qint32) 1; // next_obj_id

    cur_desc = desc_list.begin();
    while (cur_desc != desc_list.end())
    {
        d_files.StoreSingleFieldDesc(ddt, *cur_desc); //.field_name, (*cur_desc).field_label, (*cur_desc).field_type, (*cur_desc).control_type);
        cur_desc++;
    }

    ddt_file.close();

    return 0;
}
*/

/*  filterInterface
    QString mystr = "Server test string " + QVariant(desc_count).toString();
    out << (quint16)0;
    out << mystr;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    srvSocket.write(block);
*/
        // disconnect socket

    // srvSocket.waitForDisconnected();

// check descriptors
/*QList<DFieldDesc>::iterator c_desc = desc_list.begin();
while (c_desc != desc_list.end())
{
qDebug() << "f_name= " << (*c_desc).field_name << " ,f label = " << (*c_desc).field_label << " ,f type = " << (*c_desc).field_type;
c_desc++;
}*/

// read field descriptions from file
/*
int ServerFiles::ServerLoadFieldDescs()
{
    DFieldDesc f_desc;
        // lock table

        // object description data
    QFile ddt_file(FileNameBase + ".ddt");
    QDataStream ddt(&ddt_file);
    if (!ddt_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "can't open " << FileNameBase << ".ddt file";
        return -1;
    }
        // clear
    desc_list.clear();
    // f_indexes.clear();
    // f_count = 0;
        // read file
    if (ddt.device()->size() > sizeof(obj_count)*2) // size_t(obj_count)
    {
        // ddt.device()->seek(sizeof(obj_count) + sizeof(next_obj_id));
        // qDebug() << sizeof(obj_count) + sizeof(next_obj_id);
        ddt >> obj_count;
        ddt >> next_obj_id;

        while (! ddt.atEnd())
        {
            d_files.LoadSingleFieldDesc(ddt, f_desc);
                // add desc to list
            desc_list << f_desc;
                // add to map index
            // f_indexes.insert(f_desc.field_name, f_count);
            // ++f_count; // d_class->field_count
        }

    }
    */

        // test
    /*
    QList<DFieldDesc>::iterator cur_desc;
    cur_desc = field_desc_list.begin();
    while (cur_desc != field_desc_list.end())
    {
            qDebug() << "f_name= " << (*c_desc).field_name << " ,f label = " << (*c_desc).field_label << " ,f type = " << (*c_desc).field_type;
        cur_desc++;
    }
    */
    /*
    ddt_file.close();

    return 0;
}
*/

/*
    quint32 fSize;  // positive for client-server ops
    qint32 my_int;
    float my_float;

    char buf[MAX_BUF_SIZE]; //= (char *) malloc(MAX_BUF_SIZE);
    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
        // TODO: check if there are desc list

    tmpObj.data_fields.clear();

    in >> tmpObj.OBJ_ID;
    in >> tmpObj.data_obj_size;  // TODO : check total record size

        // receive all fields
    for(int i = 0; i < desc_list.count(); i++)
    {
        switch ( desc_list[i].field_type )
        {
        case d_int32 :
        case d_link  :
            in >> my_int;
            tmpObj.data_fields << my_int;
            */
            /*fSize = d_int_size;
            in.readRawData(buf, fSize);
            buf[fSize] = 0;
            tmpObj.data_fields << atoi(buf);*/
/*
            break;
        case d_float:
            in >> my_float;
            tmpObj.data_fields << my_float;
            break;
        case d_date :
            in >> my_int;
            // qDebug() << "ServerLoadData(): my_int= " << my_int << ", date= " << d_files.IntToDate(my_int).toString();
            tmpObj.data_fields << tmpObj.IntToDate(my_int);
            break;
        default     :   // string
            in >> fSize;
                // check string size
            if ((fSize >= MAX_BUF_SIZE) || (fSize >= tmpObj.data_obj_size))
            {
                qDebug() <<  "ServerRecvDataObj(): fSize >= MAX_BUF_SIZE or >= rSize, " << fSize;
                return -1;
            }
                // read string
            in.readRawData(buf, fSize);
            buf[fSize] = 0;
            tmpObj.data_fields << codec->toUnicode(buf);
        }
        // qDebug() << "ServerRecvDataObj() got "<< tmpObj.data_fields[i];
    }

        // set link explicitely
    data_obj_list.clear();
    data_obj_list.append(&tmpObj);

    return 0;

 */
