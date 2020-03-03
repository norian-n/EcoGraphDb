/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2020 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

// #ifndef EG_LOCAL_FILES_H
// #define EG_LOCAL_FILES_H

#pragma once

#include <QString>
#include <QList>
#include <QVariant>
#include <QFile>
#include <QDir>

// class DataObjects;
#include "egExtraInfo.h"
#include "indexes/egIndexesFiles.h"

// templates instances to make linker happy

#ifdef EG_LIB_BUILD     // library build switch, define it in project or egCore.h

#include "ecographdb_global.h"

template class ECOGRAPHDBSHARED_EXPORT EgFingers<qint32>;
template class ECOGRAPHDBSHARED_EXPORT EgFingers<quint32>;
template class ECOGRAPHDBSHARED_EXPORT EgFingers<qint64>;
template class ECOGRAPHDBSHARED_EXPORT EgFingers<quint64>;
template class ECOGRAPHDBSHARED_EXPORT EgFingers<float>;
template class ECOGRAPHDBSHARED_EXPORT EgFingers<double>;

template class ECOGRAPHDBSHARED_EXPORT EgIndexes<qint32>;
template class ECOGRAPHDBSHARED_EXPORT EgIndexes<quint32>;
template class ECOGRAPHDBSHARED_EXPORT EgIndexes<qint64>;
template class ECOGRAPHDBSHARED_EXPORT EgIndexes<quint64>;
template class ECOGRAPHDBSHARED_EXPORT EgIndexes<float>;
template class ECOGRAPHDBSHARED_EXPORT EgIndexes<double>;

template class ECOGRAPHDBSHARED_EXPORT EgIndexFiles<qint32>;
template class ECOGRAPHDBSHARED_EXPORT EgIndexFiles<quint32>;
template class ECOGRAPHDBSHARED_EXPORT EgIndexFiles<qint64>;
template class ECOGRAPHDBSHARED_EXPORT EgIndexFiles<quint64>;
template class ECOGRAPHDBSHARED_EXPORT EgIndexFiles<float>;
template class ECOGRAPHDBSHARED_EXPORT EgIndexFiles<double>;

#else                   // not a library build

template class EgFingers<qint32>;
template class EgFingers<quint32>;
template class EgFingers<qint64>;
template class EgFingers<quint64>;
template class EgFingers<float>;
template class EgFingers<double>;

template class EgIndexes<qint32>;
template class EgIndexes<quint32>;
template class EgIndexes<qint64>;
template class EgIndexes<quint64>;
template class EgIndexes<float>;
template class EgIndexes<double>;

template class EgIndexFiles<qint32>;
template class EgIndexFiles<quint32>;
template class EgIndexFiles<qint64>;
template class EgIndexFiles<quint64>;
template class EgIndexFiles<float>;
template class EgIndexFiles<double>;

#endif

class EgDataNodesType;

class EgDataFiles // Data Files operations
{
public:

    EgDataNodeTypeExtraInfo* metaInfo;

    QDir dir;

    QFile ddt_file;                                 // meta info file
    QDataStream ddt;

    QFile dat_file;                                 // data file
    QDataStream dat;

    EgIndexFiles<qint32>* primIndexFiles;           // primary index files
    QHash<QString, EgIndexFilesBase*> indexFiles;   // other indexes files

            // filter callback support
    FilterFunctionType FilterCallback {nullptr};
    QList <QVariant> filterValues;                  // values for custom filter function parameters

    EgDataFiles() {} // : /*indexes(new EgIndex()),*/ FilterCallback(nullptr) { /*indexes->LocalFiles = this;*/ }

    ~EgDataFiles();

    bool CheckMetaInfoFile(); // check if files exists and not empty

    int Init(EgDataNodeTypeExtraInfo& an_extraInfo); // set names and indexes control

    int LocalOpenFilesToRead();
    int LocalOpenFilesToUpdate();

    void LocalCloseFiles();

        // load&store data operations
    int LocalLoadData(QSet<quint64>& dataOffsets, QMap<EgDataNodeIdType, EgDataNode>& dataNodesMap);

    int LocalLoadDataNodes(const QSet<quint64>& dataOffsets, QList<EgDataNode>& dataNodes);
    void ReceiveDataNodes(QMap<EgDataNodeIdType, EgDataNode>& dataNodesMap, QDataStream& in);

    int LocalStoreData(QMap<EgDataNodeIdType, EgDataNode*>&  addedDataNodes, QMap<EgDataNodeIdType, EgDataNode>& deletedDataNodes, QMap<EgDataNodeIdType, EgDataNode*>&  updatedDataNodes);

        // local (non-server) store
    inline void LocalAddNodesMap(const QMap<EgDataNodeIdType, EgDataNode*>&  addedDataNodes);
    inline int LocalModifyNodesMap(const QMap<EgDataNodeIdType, EgDataNode *> &updatedDataNodes);

        // server-side store
    void LocalAddNodesList(const QList<EgDataNode>& addedDataNodes);
    int  LocalModifyNodesList(const QList<EgDataNode>&  updatedDataNodes);

        // both
    void LocalDeleteNodesList(const QList<EgDataNode>&  deletedDataNodes);

        // client-side transfers
    void SendNodesToStream(QMap<EgDataNodeIdType, EgDataNode*>&  dataNodesMap, QDataStream &nodesStream);
    void SendNodesToStream(QMap<EgDataNodeIdType, EgDataNode>&  dataNodesMap, QDataStream &nodesStream);

    int LocalCompressData();             // FIXME TODO delete data or move to archive

        // service
    int RemoveLocalFiles();      // burn it
};

// #endif // EG_LOCAL_FILES_H
