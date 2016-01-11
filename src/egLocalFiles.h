#ifndef EG_LOCAL_FILES_H
#define EG_LOCAL_FILES_H

#include <QString>
#include <QList>
#include <QVariant>
#include <QFile>

// class DataObjects;
#include "egMetaInfo.h"
#include "indexes/egIndexesFiles.h"

class EgDataNodesType;

class EgDataFiles // Data Files operations
{
public:
    // DataObjects* d_class;                    // link to ObjDB class ??

    // EgFieldDescriptors* FD;                     // field descriptors ptr - FIXME
    // EgIndex* indexes;                           // indexes class ptr

    EgDataNodeTypeMetaInfo* metaInfo;

    QFile ddt_file;                             // meta info file
    QDataStream ddt;

    QFile dat_file;                             // data file
    QDataStream dat;

    EgIndexFiles<qint32>* primIndexFiles;                // primary index files
    QHash<QString, EgIndexFiles<qint32>*> indexFiles;   // other indexes files

    FilterCallbackType FilterCallback;
    QList<QVariant> filter_values;              // filter callback parameters

    // EgDataNode tmpNode;                        // for data load and indexes update

    EgDataFiles() : /*indexes(new EgIndex()),*/ FilterCallback(NULL) { /*indexes->LocalFiles = this;*/ }
    // EgDataFiles(const EgDataNodesType* my_class);

    ~EgDataFiles();

    // void Init(QString& FileNameBase, EgFieldDescriptors* a_FD);

    bool CheckFiles(EgDataNodeTypeMetaInfo& a_metaInfo); // check if files exists and not empty

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
