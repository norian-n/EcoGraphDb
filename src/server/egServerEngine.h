#ifndef EG_SERVER_ENGINE_H
#define EG_SERVER_ENGINE_H


#include <QString>
#include <QList>
#include <QByteArray>

#include <QMutex>
#include <QtNetwork>
#include <QTcpServer>

#include "../egMetaInfo.h"
#include "../egClientServer.h"
#include "../egLocalFiles.h"
#include "../indexes/egIndexConditions.h"

/*
class egDbTcpServer : public QTcpServer
{
    Q_OBJECT

public:
    egDbTcpServer(QObject *parent = 0);

protected:
    void incomingConnection(qintptr socketDescriptor) override;

private:

};
*/

class EgServerEngine : public QObject // Data Files operations
{   
    Q_OBJECT

public:

    QTcpServer* tcpServer;

    CommandIdType commandID;
    QString nodeTypeName;

    EgDataNodeTypeMetaInfo metaInfo;

    EgDataFiles localFiles;

    QByteArray block;

    QList<EgDataNode> addNodes;
    QList<EgDataNode> updateNodes;
    QList<EgDataNode> deleteNodes;

    QSet <quint64> IndexOffsets;

    QMap<EgIndexNodeIDtype, EgIndexNode> indexNodes;
    EgIndexNodeIDtype rootNodeID = 0;

    EgIndexConditionsTree* index_tree = nullptr;
    EgIndexCondition rootIndexCondition;

    EgServerEngine(); // QWidget *parent
    virtual ~EgServerEngine() { if (tcpServer) { tcpServer->close(); delete tcpServer; } }

    void run(); // Qt Thread Pool compatible TODO : another custom thread pool (Workbox-type)

    inline void ReceiveNodesList(QList<EgDataNode>& dataNodes, QDataStream& in);
    inline void ReceiveIndexesTree(QDataStream& in);

    inline void StoreMetaInfo(QDataStream &in);
    inline void LoadMetaInfo(QDataStream &out);

    inline void AppendData(QDataStream& in);
    inline void DeleteData(QDataStream& in);
    inline void UpdateData(QDataStream& in);

    inline void LoadDataNodes(QDataStream& out);
    inline void LoadSelectedDataNodes(QDataStream& out);

    QTcpSocket* clientConnection;

private slots:
    void processRequest();
    void getCommand();

};


#endif // EG_SERVER_ENGINE_H
