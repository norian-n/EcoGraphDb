/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egServerOperProc.h"

namespace egDbMutexSpace {

QMutex masterMutex;

QMap<QString, QMutex*> nodeTypesMutexes;

}


EgServerOperProc::EgServerOperProc() // QObject(parent)
{
}

void EgServerOperProc::createSocket()
{
    clientConnection = new QTcpSocket();
}

inline void EgServerOperProc::ReceiveNodesList(QList<EgDataNode>& dataNodes, QDataStream& in)
{
    dataNodes.clear();

    uint32_t count = 0;
    EgDataNode node;

    in >> count;

    // qDebug()  << "count = " << count << FN ;

    for (uint32_t i =0; i < count; ++i)
    {
        node.dataFields.clear();
        in >> node;

        dataNodes.append(node);
    }
}

inline void EgServerOperProc::ReceiveIndexesTree(QDataStream& in)
{
    indexNodes.clear();

    uint32_t count = 0;

    EgIndexNode node;

    in >> count;
    in >> rootNodeID;

    // qDebug()  << "count = " << count << FN ;

    for (uint32_t i =0; i < count; ++i)
    {
        in >> node;

        // qDebug()  << "node IDs = " << node.nodeID << " " << node.leftID << " " << node.rightID << FN ;

        indexNodes.insert(node.nodeID, node);
    }
}

inline void EgServerOperProc::StoreExtraInfo(QDataStream& in)
{
    extraInfo.LoadExtraInfoFromStream(in);
    // in >> metaInfo;

    QDir dir(QDir::current());

    if (! dir.exists("egdb"))
    {
        // dir.mkdir("egdb");

        EgGraphDatabase graphDB;

        graphDB.CreateLinksMetaInfo();
    }

    extraInfo.LocalStoreExtraInfo();
}

inline void EgServerOperProc::LoadExtraInfo(QDataStream& out)
{
    extraInfo.LocalLoadExtraInfo();
    extraInfo.SendExtraInfoToStream(out);

    // out << metaInfo;

    // qDebug()  << block;

    clientConnection-> write(block);

    if (! clientConnection-> waitForBytesWritten(egServerTimeout)) // wait up to 10 sec
    {
        qDebug() << "waitForBytesWritten error" << FN;
    }

    block.clear();
    out.device()->seek(0);
}

inline void EgServerOperProc::LoadDataNodes(QDataStream& out)
{

    addNodes.clear();
    IndexOffsets.clear();

    localFiles.primIndexFiles -> LoadAllDataOffsets(IndexOffsets);

    localFiles.LocalLoadDataNodes(IndexOffsets, addNodes);

    out << (uint32_t) addNodes.count();

    for (auto sendIter = addNodes.begin(); sendIter != addNodes.end(); ++sendIter)
    {
        // qDebug() << "Sending node " << (int) addIter.value()-> dataNodeID << FN ;

        out << *sendIter;
    }

    // qDebug()  << block;

    clientConnection-> write(block);

    if (! clientConnection-> waitForBytesWritten(egServerTimeout)) // wait up to N sec
    {
        qDebug() << "waitForBytesWritten error" << FN;
    }

    block.clear();
    out.device()->seek(0);

    addNodes.clear();
    IndexOffsets.clear();
}

inline void EgServerOperProc::LoadSelectedDataNodes(QDataStream& out)
{
    addNodes.clear();
    IndexOffsets.clear();

    if (index_tree)
        index_tree-> CalcTreeSet(rootIndexCondition.iTreeNode, IndexOffsets, &localFiles);

    // qDebug() << "IndexOffsets count: " << IndexOffsets.count() << FN ;

    if (! IndexOffsets.isEmpty())
        localFiles.LocalLoadDataNodes(IndexOffsets, addNodes);

    out << (uint32_t) addNodes.count();

    for (auto sendIter = addNodes.begin(); sendIter != addNodes.end(); ++sendIter)
    {
        // qDebug() << "Sending node " << (int) addIter.value()-> dataNodeID << FN ;

        out << *sendIter;
    }

    // qDebug()  << block;

    clientConnection-> write(block);

    if (! clientConnection-> waitForBytesWritten(egServerTimeout)) // wait up to N sec
    {
        qDebug() << "waitForBytesWritten error" << FN;
    }

    block.clear();
    out.device()->seek(0);

    addNodes.clear();
    IndexOffsets.clear();
}

inline void EgServerOperProc::AppendData(QDataStream& in)
{
    // addNodes.clear();
    ReceiveNodesList(addNodes, in);

    // qDebug()  << "addNodes.count(): " << addNodes.count() << FN ;

    if (addNodes.count() > 0)
        localFiles.LocalAddNodes(addNodes);
}

inline void EgServerOperProc::DeleteData(QDataStream& in)
{
    // deleteNodes.clear();
    ReceiveNodesList(deleteNodes, in);

    // qDebug()  << "deleteNodes.count(): " << deleteNodes.count() << FN ;

    if (deleteNodes.count() > 0)
        localFiles.LocalDeleteNodes(deleteNodes);
}

inline void EgServerOperProc::UpdateData(QDataStream& in)
{
    // updateNodes.clear();
    ReceiveNodesList(updateNodes, in);

    // qDebug()  << "updateNodes.count(): " << updateNodes.count() << FN ;

    if (updateNodes.count() > 0)
        localFiles.LocalModifyNodesList(updateNodes);
}

void EgServerOperProc::processCommand()
{

    // qDebug() << "processCommand() called";

    if (! clientConnection-> waitForReadyRead(egServerTimeout)) // wait up to N sec
    {
        qDebug() << "waitForReadyRead error" << FN;
    }

    QDataStream in(clientConnection);
    in.setVersion(QDataStream::Qt_4_0);

    block.clear();

    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);

    in >> commandID;
    in >> nodeTypeName;

    qDebug() << "commandID = " << hex << commandID << ", nodeTypeName = " << nodeTypeName;

    extraInfo.typeName = nodeTypeName;

    egDbMutexSpace::masterMutex.lock();

    if (egDbMutexSpace::nodeTypesMutexes.contains(nodeTypeName))
    {
        // if(! egDbMutexSpace::nodeTypesMutexes[nodeTypeName]->tryLock())
        //{
        //    qDebug() << "Mutex is locked for nodeTypeName = " << nodeTypeName;
            egDbMutexSpace::nodeTypesMutexes[nodeTypeName]-> lock();
        //}
    }
    else
    {
        QMutex* newMutex = new QMutex(); // intentional mem leak

        egDbMutexSpace::nodeTypesMutexes.insert(nodeTypeName, newMutex);
        egDbMutexSpace::nodeTypesMutexes[nodeTypeName]-> lock();
    }

    egDbMutexSpace::masterMutex.unlock();

    // process command
    switch ( commandID )
    {
    case opcode_store_metainfo: // store metainfo on server

        StoreExtraInfo(in);
        break;

    case opcode_load_metainfo: // load metainfo from server

        LoadExtraInfo(out);
        break;

    case opcode_store_data: // store data nodes changes on server

        extraInfo.LocalLoadExtraInfo();
        localFiles.Init(extraInfo);

        localFiles.LocalOpenFilesToUpdate();

        AppendData(in);
        DeleteData(in);
        UpdateData(in);

        localFiles.LocalCloseFiles();

        break;

    case opcode_load_all_data: // load metainfo from server

        extraInfo.LocalLoadExtraInfo();
        localFiles.Init(extraInfo);

        LoadDataNodes(out);

        localFiles.LocalCloseFiles();

        break;

    case opcode_load_selected_data: // load metainfo from server

        // clientConnection-> waitForReadyRead(egServerTimeout);
        // QThread::msleep(100);

        ReceiveIndexesTree(in);

        if (rootNodeID && indexNodes.count())
        {

            extraInfo.LocalLoadExtraInfo();
            localFiles.Init(extraInfo);

            index_tree = new EgIndexConditionsTree();
            rootIndexCondition.BuildTreeFromMap(indexNodes, rootNodeID);

            LoadSelectedDataNodes(out);

            localFiles.LocalCloseFiles();
        }
        else
          qDebug()  << "ERROR: empty indexes map or rootID: " << rootNodeID << "count(): " << indexNodes.count() << FN ;

        break;


    default:
        qDebug()  << "ERROR: bad opcode " << commandID << FN ;
    }

    // qDebug()  << "Mutex map keys: " << egDbMutexSpace::nodeTypesMutexes.keys() << FN ;

    egDbMutexSpace::nodeTypesMutexes[nodeTypeName]-> unlock();

    // clientConnection->write(block);
    clientConnection->disconnectFromHost();

    if (clientConnection-> state() == QAbstractSocket::ConnectedState)
        clientConnection-> waitForDisconnected(egServerTimeout);
}
