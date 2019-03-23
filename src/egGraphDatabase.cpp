/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egGraphDatabase.h"
#include "egDataNodesLink.h"
#include "egDataClient.h"

EgGraphDatabase::~EgGraphDatabase()
{
    if (extraInfo)
        delete extraInfo;
    if (locationExtraInfo)
        delete locationExtraInfo;
    if (attributesExtraInfo)
        delete attributesExtraInfo;
    if (GUIcontrolsExtraInfo)
        delete GUIcontrolsExtraInfo;
    if (entryNodesExtraInfo)
        delete entryNodesExtraInfo;
}


bool EgGraphDatabase::CheckLinksMetaInfoLocal()
{
    // EgDataNodesType linksMetaInfo;

    if (dir.dirName() == "egdb")
    {
        qDebug() << "error: unexpected folder: " << dir.path() << FN;

        return false;
    }

        // check folder and ddt file
    if (! dir.exists(QString("egdb/") + EgDataNodesLinkNamespace::egLinkTypesFileName + ".ddt"))
    {
        // qDebug()  << "No egDb meta info found: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

        return false;   // no db/metainfo
    }

        // check folder and dat file
    if (! dir.exists(QString("egdb/") + EgDataNodesLinkNamespace::egLinkTypesFileName + ".dat"))
    {
        // qDebug()  << "No egDb meta info found: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

        return false;   // no db/metainfo
    }

    /*
        // try to connect
    if (linksMetaInfo.Connect(*this, EgDataNodesLinkNamespace::egLinkTypesFileName))
    {
        // qDebug()  << "No egDb meta info connected: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

        return false;   // no db/metainfo
    }
    */

    return true;
}

bool EgGraphDatabase::CheckNodeTypesMetaInfoLocal()
{
    // EgDataNodesType linksMetaInfo;

    if (dir.dirName() == "egdb")
    {
        qDebug() << "error: unexpected folder: " << dir.path() << FN;

        return false;
    }

        // check folder and ddt file
    if (! dir.exists(QString("egdb/") + EgDataNodesNamespace::egDataNodesTypesFileName + ".ddt"))
    {
        // qDebug()  << "No egDb meta info found: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

        return false;   // no db/metainfo
    }

        // check folder and dat file
    if (! dir.exists(QString("egdb/") + EgDataNodesNamespace::egDataNodesTypesFileName + ".dat"))
    {
        // qDebug()  << "No egDb meta info found: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

        return false;   // no db/metainfo
    }

    /*
        // try to connect
    if (linksMetaInfo.Connect(*this, EgDataNodesLinkNamespace::egLinkTypesFileName))
    {
        // qDebug()  << "No egDb meta info connected: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

        return false;   // no db/metainfo
    }
    */

    return true;
}

int EgGraphDatabase::CreateNodeTypesMetaInfo()
{
    // EgNodeTypeSettings typeSettings;

    if (dir.dirName() == "egdb")
    {
        qDebug() << "error: unexpected folder: " << dir.path() << FN;

        return false;
    }

    if (! dir.exists("egdb"))
        dir.mkdir("egdb");

        // init links meta-info

    fieldsCreated = 0;
    locationFieldsCreated = 0;

    EgDataNodeTypeExtraInfo* typesMetaInfo = new EgDataNodeTypeExtraInfo(EgDataNodesNamespace::egDataNodesTypesFileName);

        // add fields
    typesMetaInfo-> AddDataField("nodeTypeName");

    typesMetaInfo-> LocalStoreExtraInfo();

    delete typesMetaInfo;

        // add dummy type
    QList<QVariant> addValues;
    EgDataNodesType nodesTypesMetaInfo;

    nodesTypesMetaInfo.ConnectServiceNodeType(*this, EgDataNodesNamespace::egDataNodesTypesFileName);

    addValues << QString(EgDataNodesNamespace::egDummyNodesType);

    nodesTypesMetaInfo.AddDataNode(addValues);
    nodesTypesMetaInfo.StoreData();

    return 0;
}


int EgGraphDatabase::CreateLinksMetaInfo()
{
    // EgNodeTypeSettings typeSettings;

    if (dir.dirName() == "egdb")
    {
        qDebug() << "error: unexpected folder: " << dir.path() << FN;

        return false;
    }

    if (! dir.exists("egdb"))
        dir.mkdir("egdb");

        // init links meta-info
    if (extraInfo)
    {
        delete extraInfo;
    }

    fieldsCreated = 0;
    locationFieldsCreated = 0;

    extraInfo = new EgDataNodeTypeExtraInfo(EgDataNodesLinkNamespace::egLinkTypesFileName);

    ClearExtraInfo(GUIcontrolsExtraInfo);
    ClearExtraInfo(entryNodesExtraInfo);
    ClearExtraInfo(locationExtraInfo);
    ClearExtraInfo(attributesExtraInfo);

        // add fields
    AddDataField("linkTypeName");
    AddDataField("firstNodeType");
    AddDataField("secondNodeType");

    CommitNodeType();

    QList<QVariant> addValues;
    EgDataNodesType linksMetaInfo;

    linksMetaInfo.ConnectServiceNodeType(*this, EgDataNodesLinkNamespace::egLinkTypesFileName);

    addValues << QString(EgDataNodesNamespace::egDummyLinkType)
              << QString(EgDataNodesNamespace::egDummyNodesType)
              << QString(EgDataNodesNamespace::egDummyNodesType);

    linksMetaInfo.AddDataNode(addValues);
    linksMetaInfo.StoreData();

    return 0;
}

inline void EgGraphDatabase::ClearExtraInfo(EgDataNodeTypeExtraInfo* metaInfo)
{
    if (metaInfo)
    {
        delete metaInfo;
        metaInfo = nullptr;
    }
}

int EgGraphDatabase::CreateNodeType(const QString& typeName,  EgNodeTypeSettings& typeSettings, const QString &theServerAddress)
{
    // check/create DB metainfo

    serverAddress = theServerAddress;

    if (theServerAddress.isEmpty())
    {

        dir = QDir::current();

        if(! CheckLinksMetaInfoLocal())
            CreateLinksMetaInfo();

        if(! CheckNodeTypesMetaInfoLocal())
            CreateNodeTypesMetaInfo();

        // check if node type exists
        if (dir.exists("egdb/" + typeName + ".ddt"))
        {
            qDebug()  << "Data node type already exists: " << typeName << FN;

            return 1;
        }
    }

        // init all
    if (extraInfo)
    {
        qDebug()  << "Warning: node type extra info exists, CommitNodeType() wasn't called properly before " << typeName << FN;
        delete extraInfo;
    }

    fieldsCreated = 0;
    locationFieldsCreated = 0;

    extraInfo = new EgDataNodeTypeExtraInfo(typeName);

        // store settings
    extraInfo-> typeSettings = typeSettings;

    ClearExtraInfo(GUIcontrolsExtraInfo);
    ClearExtraInfo(entryNodesExtraInfo);
    ClearExtraInfo(locationExtraInfo);
    ClearExtraInfo(attributesExtraInfo);

    if (typeSettings.useGUIsettings)
        GUIcontrolsExtraInfo = new EgDataNodeTypeExtraInfo(typeName + EgDataNodesNamespace::egGUIfileName);

    if (typeSettings.useEntryNodes)
        entryNodesExtraInfo = new EgDataNodeTypeExtraInfo(typeName + EgDataNodesNamespace::egEntryNodesFileName);

    if (typeSettings.useLocation)
        locationExtraInfo = new EgDataNodeTypeExtraInfo(typeName + EgDataNodesNamespace::egLocationFileName);

    if (typeSettings.useNamedAttributes)
        attributesExtraInfo = new EgDataNodeTypeExtraInfo(typeName + EgDataNodesNamespace::egAttributesFileName);

    if (! theServerAddress.isEmpty())  // FIXME delete connections
    {
        extraInfo-> myECoGraphDB-> serverConnection= new EgServerConnection();

        if (GUIcontrolsExtraInfo)
            GUIcontrolsExtraInfo-> myECoGraphDB-> serverConnection= new EgServerConnection();
        if (entryNodesExtraInfo)
            entryNodesExtraInfo-> myECoGraphDB-> serverConnection= new EgServerConnection();
        if (locationExtraInfo)
            locationExtraInfo-> myECoGraphDB-> serverConnection= new EgServerConnection();
        if (attributesExtraInfo)
            attributesExtraInfo-> myECoGraphDB-> serverConnection= new EgServerConnection();
    }

    return 0;
}


int EgGraphDatabase::AddDataField(const QString& fieldName, bool uint32index)
{
    if (! extraInfo)
    {
        qDebug()  << "CreateNodeType() wasn't called, field not added " << fieldName << FN;
        return -1;
    }

    extraInfo-> AddDataField(fieldName, uint32index);

    return 0;
}

int EgGraphDatabase::AddLocationField(const QString& fieldName, bool uint32index)
{
    if (! locationExtraInfo)
    {
        qDebug()  << "CreateNodeType() wasn't called for node location field, it was not added " << fieldName << FN;
        return -1;
    }

    locationExtraInfo-> AddDataField(fieldName, uint32index);

    return 0;
}


int EgGraphDatabase::AddDataField(const QString& fieldName, EgIndexSettings indexSettings)
{
    if (! extraInfo)
    {
        qDebug()  << "CreateNodeType() wasn't called, field not added " << fieldName << FN;
        return -1;
    }

    extraInfo-> AddDataField(fieldName, indexSettings);

    return 0;
}

int EgGraphDatabase::AddLocationField(const QString& fieldName, EgIndexSettings indexSettings)
{
    if (! locationExtraInfo)
    {
        qDebug()  << "CreateNodeType() wasn't called for node location field, it was not added " << fieldName << FN;
        return -1;
    }

    locationExtraInfo-> AddDataField(fieldName, indexSettings);

    return 0;
}

int EgGraphDatabase::CommitNodeType()
{
    if (! extraInfo)
    {
        qDebug()  << "CreateNodeType wasn't called" << FN;
        return -1;
    }

    QString storedTypeName = extraInfo-> typeName;

    fieldsCreated = extraInfo->dataFields.count();

    if (serverAddress.isEmpty())
        extraInfo-> LocalStoreExtraInfo();
    else
        extraInfo-> ServerStoreExtraInfo();   // FIXME check result

    if (GUIcontrolsExtraInfo)
    {
        GUIcontrolsExtraInfo-> AddDataField("name");
        GUIcontrolsExtraInfo-> AddDataField("label");
        GUIcontrolsExtraInfo-> AddDataField("width");

        if (serverAddress.isEmpty())
            GUIcontrolsExtraInfo-> LocalStoreExtraInfo();
        else
            GUIcontrolsExtraInfo-> ServerStoreExtraInfo();

        delete GUIcontrolsExtraInfo;
        GUIcontrolsExtraInfo = nullptr;
    }

    if (entryNodesExtraInfo)
    {
        entryNodesExtraInfo-> AddDataField("subgraphid");    // types hard linked by node id, this field is optional

        if (serverAddress.isEmpty())
            entryNodesExtraInfo-> LocalStoreExtraInfo();
        else
            entryNodesExtraInfo-> ServerStoreExtraInfo();

        delete entryNodesExtraInfo;
        entryNodesExtraInfo = nullptr;
    }

    if (locationExtraInfo)
    {
            // add default fields: x,y
        locationExtraInfo-> AddDataField("x");
        locationExtraInfo-> AddDataField("y");

        locationFieldsCreated = locationExtraInfo->dataFields.count();

        if (serverAddress.isEmpty())
            locationExtraInfo-> LocalStoreExtraInfo();
        else
            locationExtraInfo-> ServerStoreExtraInfo();

        delete locationExtraInfo;
        locationExtraInfo = nullptr;
    }

    if (attributesExtraInfo)
    {
            // add named attributes fields
        attributesExtraInfo-> AddDataField("nodeid", isIndexed);
        attributesExtraInfo-> AddDataField("key");
        attributesExtraInfo-> AddDataField("value");

        if (serverAddress.isEmpty())
            attributesExtraInfo-> LocalStoreExtraInfo();
        else
            attributesExtraInfo-> ServerStoreExtraInfo();

        delete attributesExtraInfo;
        attributesExtraInfo = nullptr;
    }

    if (extraInfo)
    {
        delete extraInfo;
        extraInfo = nullptr;
    }

    AddDataNodeTypeMetaInfo(storedTypeName);  // save to metainfo database

    dataNodeTypes.insert(storedTypeName, storedTypeName);

    return 0;
}

int  EgGraphDatabase::AddLinkType(const QString& linkName, const QString& firstDataNodeType, const QString& secondDataNodeType)
{
    QList<QVariant> addValues;
    EgDataNodesType linksMetaInfo; // == LinkTypes

    dir = QDir::current();

    if(! CheckLinksMetaInfoLocal())
        CreateLinksMetaInfo();

    LoadLinksMetaInfo(); // re-read FIXME check if needed

            // check if link name exists  
    if (linkTypes.contains(linkName))
    {
        qDebug()  << "Link name alredy exists: " << linkName << FN;
        return 1;
    }

    if (! dataNodeTypes.contains(firstDataNodeType))
    {
        qDebug()  << "Data node type doesn't exists: " << firstDataNodeType << FN;
        return 1;
    }

    if (! dataNodeTypes.contains(secondDataNodeType))
    {
        qDebug()  << "Data node type doesn't exists: " << secondDataNodeType << FN;
        return 1;
    }

        // add new link type
    EgNodeTypeSettings typeSettings;

    CreateNodeType(linkName + EgDataNodesLinkNamespace::egLinkFileNamePostfix, typeSettings);

    AddDataField("from_node_id", isIndexed);
    AddDataField("to_node_id");                 // FIXME check if index required

    CommitNodeType();

    if (linksMetaInfo.ConnectServiceNodeType(*this, EgDataNodesLinkNamespace::egLinkTypesFileName) < 0)
    {
        // qDebug()  << "EgDB connect error: Links meta info doesn't exist: " << EgDataNodesLinkNamespace::egLinkTypesFileName << ".dat" << FN;

        return -1;
    }

    addValues << linkName << firstDataNodeType << secondDataNodeType;

    linksMetaInfo.AddDataNode(addValues);
    linksMetaInfo.StoreData();

    EgDataNodesLinkType* newLink = new EgDataNodesLinkType(this);

    newLink-> linkName = linkName;
    newLink-> firstTypeName = firstDataNodeType;
    newLink-> secondTypeName = secondDataNodeType;

    linkTypes.insert(newLink-> linkName, *newLink);

    return 0;

}

int  EgGraphDatabase::AddDataNodeTypeMetaInfo(const QString& typeName)
{
    QList<QVariant> addValues;
    EgDataNodesType typesMetaInfoDatabase;

    if (typeName == EgDataNodesLinkNamespace::egLinkTypesFileName) // skip links metainfo
        return 1;

    dir = QDir::current();

    if(! CheckNodeTypesMetaInfoLocal())
        CreateNodeTypesMetaInfo();

    if (typesMetaInfoDatabase.ConnectServiceNodeType(*this, EgDataNodesNamespace::egDataNodesTypesFileName))
    {
        qDebug()  << "Can't connect nodes types meta-info, aborting" << FN;
        return -1;
    }

    typesMetaInfoDatabase.LoadAllDataNodes();

            // check if node type name exists
    for (QMap<EgDataNodeIdType, EgDataNode>::iterator nodesIter = typesMetaInfoDatabase.dataNodes.begin(); nodesIter != typesMetaInfoDatabase.dataNodes.end(); ++nodesIter)
    {
        if(nodesIter.value()["nodeTypeName"].toString() == typeName)
        {
            qDebug()  << "Node type name alredy exists: " << typeName << FN;
            return 1;
        }
    }

    addValues << typeName;

    typesMetaInfoDatabase.AddDataNode(addValues);
    typesMetaInfoDatabase.StoreData();

    return 0;

}

/*
int EgGraphDatabase::Connect()
{
    int res = 0;    // TODO could be error id reference

    if (! isConnected)
    {
        res = LoadLinksMetaInfo();   // 1 means empty meta, negative is error

        LoadNodeTypesMetaInfo();

        if (! res)
            isConnected = true;
    }

    return res;
}
*/

int EgGraphDatabase::AttachNodesType(EgDataNodesType* nType)
{
    // int res = 0;

    if (! isConnected)
    {
        // qDebug()  << "egGraphDatabase is not connected, connect it prior data nodes types" << FN;

        LoadNodeTypesMetaInfo();

        if (! LoadLinksMetaInfo())
            isConnected = true;
    }

    if (nType)
    {
        // qDebug() << "trying to attach: " << nType-> metaInfo.typeName << FN;

        if (! attachedNodeTypes.contains(nType-> extraInfo.typeName)) // FIXME check multi instances
            attachedNodeTypes.insert(nType-> extraInfo.typeName, nType);
    }

    return 0;
}

int EgGraphDatabase::LoadLinksMetaInfo()
{
    EgDataNodesLinkType* newLink;
    EgDataNodesType linksMetaInfo;

    if (serverAddress.isEmpty())
    {

        dir = QDir::current();

        if (! dir.exists("egdb/" + QString(EgDataNodesLinkNamespace::egLinkTypesFileName) + ".dat"))
        {
            qDebug()  << "EgDB connect error: Links meta info doesn't exist: " << EgDataNodesLinkNamespace::egLinkTypesFileName << ".dat" << FN;

            return -1;
        }
    }

    if (linksMetaInfo.ConnectServiceNodeType(*this, EgDataNodesLinkNamespace::egLinkTypesFileName) < 0)
    {
        // qDebug()  << "EgDB connect error: Links meta info doesn't exist: " << EgDataNodesLinkNamespace::egLinkTypesFileName << ".dat" << FN;

        return -1;
    }

    linksMetaInfo.LoadAllDataNodes();

    linkTypes.clear();

        // QMap<EgDataNodeIDtype, EgDataNode>::iterator
    for (auto nodesIter = linksMetaInfo.dataNodes.begin(); nodesIter != linksMetaInfo.dataNodes.end(); ++nodesIter)
    {
        // qDebug()  << "nodesIter.value().dataFields = " << nodesIter.value().dataFields << FN;

        newLink = new EgDataNodesLinkType(this);

        newLink-> linkName = nodesIter.value()["linkTypeName"].toString();
        newLink-> firstTypeName = nodesIter.value()["firstNodeType"].toString();
        newLink-> secondTypeName = nodesIter.value()["secondNodeType"].toString();

        if (newLink-> linkName != QString(EgDataNodesNamespace::egDummyLinkType))
        {
            linkTypes.insert(newLink-> linkName, *newLink);

            // qDebug()  << "Link added to EgGraphDatabase: " << newLink-> linkName << FN;
        }
    }

    return 0;
}


int EgGraphDatabase::LoadNodeTypesMetaInfo()
{
    EgDataNodesType typesMetaInfoDatabase;

    if (typesMetaInfoDatabase.ConnectServiceNodeType(*this, EgDataNodesNamespace::egDataNodesTypesFileName))
    {
        qDebug()  << "Can't connect nodes types meta-info, aborting" << FN;
        return -1;
    }

    typesMetaInfoDatabase.LoadAllDataNodes();

    dataNodeTypes.clear();

            // check if link name exists
    for (auto nodesIter = typesMetaInfoDatabase.dataNodes.begin(); nodesIter != typesMetaInfoDatabase.dataNodes.end(); ++nodesIter)
        dataNodeTypes.insert(nodesIter.value()["nodeTypeName"].toString(), nodesIter.value()["nodeTypeName"].toString());

    // qDebug()  << "dataNodeTypes : " << dataNodeTypes.keys() << FN;

    return 0;
}


/*
int  EgGraphDatabase::StoreAllLinks()
{
    for (QMap<QString, EgDataNodesLinkType>::iterator linksIter = linkTypes.begin(); linksIter != linkTypes.end(); ++linksIter)
            linksIter.value().StoreLinks();

    return 0;
}

int  EgGraphDatabase::LoadAllLinks()
{
    for (QMap<QString, EgDataNodesLinkType>::iterator linksIter = linkTypes.begin(); linksIter != linkTypes.end(); ++linksIter)
            linksIter.value().LoadLinks();

    return 0;
}
*/
