#include "egServerEngine.h"

EgServerEngine::EgServerEngine():
        field_count(0),
        filter_id(-1),
        glob_odb_map(NULL),
        in(&srvSocket),
        out(&block, QIODevice::WriteOnly)
{
    out.setVersion(QDataStream::Qt_4_0);
    in.setVersion(QDataStream::Qt_4_0);
}

void EgServerEngine::Init()   // read stored Odb names from file - do it ONCE on server launch, not for each thread
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
        qDebug() << "EgServerEngine::Init() got bad glob_new_id ptr";
        return;
    }
        // check global map ptr
    if (! glob_odb_map)
    {
        qDebug() << "EgServerEngine::Init() got bad glob_odb_map ptr ";
        return;
    }
        // init ID counter
    *glob_new_id = 1;
        // clear map and list
    glob_odb_map->clear();
    glob_odb_list->clear();

    if (!ddt_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "EgServerEngine::Init() can't open " << "odb_names.onf" << " file";
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
void EgServerEngine::SetFileName(QString& FNameBase)
{
    // FileNameBase  = FNameBase;

    d_files.Init(FNameBase, &FD);
        // load from file TODO : make it global ?
    // d_files.LocalLoadFieldDesc2(desc_list, field_indexes, field_count, obj_count, next_obj_id); FIXME
}
    // load filter callbacks plugin
bool EgServerEngine::loadPlugin()
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
// int EgServerEngine::Execute(int socketDescriptor)                     // execute client's request
void EgServerEngine::run()
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

void EgServerEngine::LoadFilterArgs()
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


int EgServerEngine::ServerSendObjects()    // FIXME send objects
{
    QList<EgPackedDataNode>::iterator cur_obj;
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

inline void EgServerEngine::ServerRecvDataObj()
{
    qint32 obj_count;

    PackedList.clear();
    EgPackedDataNode tmpObj;

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


int EgServerEngine::ServerSendOdbID() 		// find or create new ODB entry in global map and save it to file
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

int EgServerEngine::ServerSendFieldDescs(QByteArray* field_descs, QByteArray* control_descs)    // send field descriptors to client app
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
