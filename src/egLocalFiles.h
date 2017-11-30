/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EG_LOCAL_FILES_H
#define EG_LOCAL_FILES_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QFile>
#include <QDir>

// class DataObjects;
#include "egMetaInfo.h"
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


/*
EgFingers<qint32> EgFingersqint32;
EgFingers<quint32> EgFingersquint32;
EgFingers<qint64>  EgFingersqint64;
EgFingers<quint64> EgFingersquint64;
EgFingers<float> EgFingersfloat;
EgFingers<double> EgFingersdouble;

EgIndexes<qint32> EgIndexesqint32;
EgIndexes<quint32> EgIndexesquint32;
EgIndexes<qint64> EgIndexesqint64;
EgIndexes<quint64> EgIndexesquint64;
EgIndexes<float> EgIndexesfloat;
EgIndexes<double> EgIndexesdouble;
*/

class EgDataNodesType;

class EgDataFiles // Data Files operations
{
public:

    EgDataNodeTypeMetaInfo* metaInfo;

    QDir dir;

    QFile ddt_file;                             // meta info file
    QDataStream ddt;

    QFile dat_file;                             // data file
    QDataStream dat;

    EgIndexFiles<qint32>* primIndexFiles;                   // primary index files
    QHash<QString, EgIndexFilesBase*> indexFiles;       // other indexes files

    FilterFunctionType FilterCallback;

        // filter callback support
    QList <QVariant> filterValues;                  // values for custom filter function parameters

    // EgDataNode tmpNode;                          // for data load and indexes update

    EgDataFiles() : /*indexes(new EgIndex()),*/ FilterCallback(nullptr) { /*indexes->LocalFiles = this;*/ }
    // EgDataFiles(const EgDataNodesType* my_class);

    ~EgDataFiles();

    // void Init(QString& FileNameBase, EgFieldDescriptors* a_FD);

    bool CheckMetaInfoFile(); // check if files exists and not empty

    int Init(EgDataNodeTypeMetaInfo& a_metaInfo); // set names and indexes control

    int LocalOpenFilesToRead();
    int LocalOpenFilesToUpdate();

    inline void LocalCloseFiles();

    // int LocalStoreMetaInfo(EgDataNodeTypeMetaInfo& metaInfo);
    // int LocalLoadMetaInfo(EgDataNodeTypeMetaInfo& metaInfo);

        // field descriptors

    // int LocalStoreFieldDesc(QByteArray* field_descs, QByteArray* control_descs);
    // int LocalLoadFieldDesc(QByteArray* field_descs, QByteArray* control_descs, EgDataNodeIDtype& obj_count, EgDataNodeIDtype& next_obj_id);

        // objdb data operations
    int LocalLoadData(QSet<quint64>& dataOffsets, QMap<EgDataNodeIDtype, EgDataNode>& dataNodesMap);
    int LocalStoreData(QMap<EgDataNodeIDtype, EgDataNode*>&  addedDataNodes, QMap<EgDataNodeIDtype, EgDataNode>& deletedDataNodes, QMap<EgDataNodeIDtype, EgDataNode*>&  updatedDataNodes);

    inline void LocalDeleteObjects(QMap<EgDataNodeIDtype, EgDataNode>& deletedDataNodes);
    inline void LocalAddObjects(QDataStream& dat, QMap<EgDataNodeIDtype, EgDataNode*>&  addedDataNodes);
    inline int LocalModifyObjects(QDataStream& dat, QMap<EgDataNodeIDtype, EgDataNode*>&  updatedDataNodes);


    int LocalCompressData();             // FIXME delete data or move to archive

        // internal operations
    // inline void AppendNewData(QDataStream& dat, QList<EgPackedDataNode*>& a_list);

        // service
    int RemoveLocalFiles();      // burn it

};

#endif // EG_LOCAL_FILES_H
