/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_SERVER_OPER_PROC_H
#define EG_SERVER_OPER_PROC_H

/*
#include <QString>
#include <QList>
#include <QByteArray>

#include <QMutex>
// #include <QTcpServer>
*/

#include <QtNetwork>


#include "../egMetaInfo.h"
#include "../egClientServer.h"
#include "../egLocalFiles.h"
#include "../indexes/egIndexConditions.h"
#include "../egGraphDatabase.h"

class EgServerOperProc // : public QObject // Data Files operations
{   
    // Q_OBJECT

public:

    QTcpSocket* clientConnection = nullptr;
    EgIndexConditionsTree* index_tree = nullptr;

    CommandIdType commandID;
    QString nodeTypeName;

    EgDataNodeTypeExtraInfo metaInfo;

    EgDataFiles localFiles;

    QByteArray block;

    QList<EgDataNode> addNodes;
    QList<EgDataNode> updateNodes;
    QList<EgDataNode> deleteNodes;

    QSet <quint64> IndexOffsets;

    QMap<EgIndexNodeIDtype, EgIndexNode> indexNodes;
    EgIndexNodeIDtype rootNodeID = 0;

    EgIndexCondition rootIndexCondition;

    EgServerOperProc();
    virtual ~EgServerOperProc() { if (clientConnection) delete clientConnection; if (index_tree) delete index_tree; }

    void createSocket();

    inline void ReceiveNodesList(QList<EgDataNode>& dataNodes, QDataStream& in);
    inline void ReceiveIndexesTree(QDataStream& in);

    inline void StoreMetaInfo(QDataStream &in);
    inline void LoadMetaInfo(QDataStream &out);

    inline void AppendData(QDataStream& in);
    inline void DeleteData(QDataStream& in);
    inline void UpdateData(QDataStream& in);

    inline void LoadDataNodes(QDataStream& out);
    inline void LoadSelectedDataNodes(QDataStream& out);

    void processCommand();
};


#endif // EG_SERVER_OPER_PROC_H
