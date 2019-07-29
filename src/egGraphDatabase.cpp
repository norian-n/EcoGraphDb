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
        EG_LOG_STUB << "error: unexpected folder: " << dir.path() << FN;

        return false;
    }

        // check folder and ddt file
    if (! dir.exists(QString("egdb/") + EgDataNodesLinkNamespace::egLinkTypesFileName + ".ddt"))
    {
        // EG_LOG_STUB  << "No egDb meta info found: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

        return false;   // no db/metainfo
    }

        // check folder and dat file
    if (! dir.exists(QString("egdb/") + EgDataNodesLinkNamespace::egLinkTypesFileName + ".dat"))
    {
        // EG_LOG_STUB  << "No egDb meta info found: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

        return false;   // no db/metainfo
    }

    /*
        // try to connect
    if (linksMetaInfo.Connect(*this, EgDataNodesLinkNamespace::egLinkTypesFileName))
    {
        // EG_LOG_STUB  << "No egDb meta info connected: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

        return false;   // no db/metainfo
    }
    */

    return true;
}

bool EgGraphDatabase::CheckNodeTypesMetaInfoLocal()
{
    // EgDataNodesType linksMetaInfo;

    dir = QDir::current();

    if (dir.dirName() == "egdb")
    {
        EG_LOG_STUB << "error: unexpected folder: " << dir.path() << FN;

        return false;
    }

        // check folder and ddt file
    if (! dir.exists(QString("egdb/") + EgDataNodesNamespace::egDataNodesTypesFileName + ".ddt"))
    {
        // EG_LOG_STUB  << "No egDb meta info found: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

        return false;   // no db/metainfo
    }

        // check folder and dat file
    if (! dir.exists(QString("egdb/") + EgDataNodesNamespace::egDataNodesTypesFileName + ".dat"))
    {
        // EG_LOG_STUB  << "No egDb meta info found: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

        return false;   // no db/metainfo
    }

    /*
        // try to connect
    if (linksMetaInfo.Connect(*this, EgDataNodesLinkNamespace::egLinkTypesFileName))
    {
        // EG_LOG_STUB  << "No egDb meta info connected: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;

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
        EG_LOG_STUB << "error: unexpected folder: " << dir.path() << FN;

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
        EG_LOG_STUB << "error: unexpected folder: " << dir.path() << FN;

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

int EgGraphDatabase::CreateNodeType(const QString& typeName,  EgNodeTypeSettings& typeSettings)
{
    // check/create DB metainfo

    if (serverAddress.isEmpty())
    {

        dir = QDir::current();

        if(! CheckLinksMetaInfoLocal())
            CreateLinksMetaInfo();

        if(! CheckNodeTypesMetaInfoLocal())
            CreateNodeTypesMetaInfo();

        // check if node type exists
        if (dir.exists("egdb/" + typeName + ".ddt"))
        {
            EG_LOG_STUB  << "Data node type already exists: " << typeName << FN;

            return 1;
        }
    }
    else
    {
        // FIXME implement server metainfo update
    }

        // init all
    if (extraInfo)
    {
        EG_LOG_STUB  << "Warning: node type extra info exists, CommitNodeType() wasn't called properly before " << typeName << FN;
        ClearExtraInfo(extraInfo);
    }
    else
        extraInfo = new EgDataNodeTypeExtraInfo(typeName);

    fieldsCreated = 0;
    locationFieldsCreated = 0;

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

    return 0;
}


int EgGraphDatabase::AddDataField(const QString& fieldName, bool uint32index)
{
    if (! extraInfo)
    {
        EG_LOG_STUB  << "CreateNodeType() wasn't called, field not added " << fieldName << FN;
        return -1;
    }

    extraInfo-> AddDataField(fieldName, uint32index);

    return 0;
}

int EgGraphDatabase::AddLocationField(const QString& fieldName, bool uint32index)
{
    if (! locationExtraInfo)
    {
        EG_LOG_STUB  << "CreateNodeType() wasn't called for node location field, it was not added " << fieldName << FN;
        return -1;
    }

    locationExtraInfo-> AddDataField(fieldName, uint32index);

    return 0;
}


int EgGraphDatabase::AddDataField(const QString& fieldName, EgIndexSettings indexSettings)
{
    if (! extraInfo)
    {
        EG_LOG_STUB  << "CreateNodeType() wasn't called, field not added " << fieldName << FN;
        return -1;
    }

    extraInfo-> AddDataField(fieldName, indexSettings);

    return 0;
}

int EgGraphDatabase::AddLocationField(const QString& fieldName, EgIndexSettings indexSettings)
{
    if (! locationExtraInfo)
    {
        EG_LOG_STUB  << "CreateNodeType() wasn't called for node location field, it was not added " << fieldName << FN;
        return -1;
    }

    locationExtraInfo-> AddDataField(fieldName, indexSettings);

    return 0;
}

int EgGraphDatabase::CommitNodeType()
{
    if (! extraInfo)
    {
        EG_LOG_STUB  << "CreateNodeType wasn't called" << FN;
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

int  EgGraphDatabase::AddLinkType(const QString& theLinkName, const QString& firstDataNodeType, const QString& secondDataNodeType)
{
    QList<QVariant> addValues;
    EgDataNodesType linksMetaInfo; // == LinkTypes

    dir = QDir::current();

    if(! CheckLinksMetaInfoLocal())
        CreateLinksMetaInfo();

    LoadLinksMetaInfo(); // re-read FIXME check if needed

            // check if link name exists  
    if (linkTypes.contains(theLinkName))
    {
        EG_LOG_STUB  << "Link name alredy exists: " << theLinkName << FN;
        return 1;
    }

    if (! dataNodeTypes.contains(firstDataNodeType))
    {
        EG_LOG_STUB  << "Data node type doesn't exists: " << firstDataNodeType << FN;
        return 1;
    }

    if (! dataNodeTypes.contains(secondDataNodeType))
    {
        EG_LOG_STUB  << "Data node type doesn't exists: " << secondDataNodeType << FN;
        return 1;
    }

        // add new link type
    EgNodeTypeSettings typeSettings;

    CreateNodeType(theLinkName + EgDataNodesLinkNamespace::egLinkFileNamePostfix, typeSettings);

    AddDataField("from_node_id", isIndexed);
    AddDataField("to_node_id");                 // FIXME check if index required

    CommitNodeType();

    if (linksMetaInfo.ConnectServiceNodeType(*this, EgDataNodesLinkNamespace::egLinkTypesFileName) < 0)
    {
        // EG_LOG_STUB  << "EgDB connect error: Links meta info doesn't exist: " << EgDataNodesLinkNamespace::egLinkTypesFileName << ".dat" << FN;

        return -1;
    }

    addValues << theLinkName << firstDataNodeType << secondDataNodeType;

    linksMetaInfo.AddDataNode(addValues);
    linksMetaInfo.StoreData();

    EgLinkNames newLinkNames;

    newLinkNames.linkName = theLinkName;
    newLinkNames.firstTypeName = firstDataNodeType;
    newLinkNames.secondTypeName = secondDataNodeType;

    linkTypes.insert(theLinkName, newLinkNames);

    return 0;

}

int  EgGraphDatabase::AddDataNodeTypeMetaInfo(const QString& typeName)
{
    QList<QVariant> addValues;
    EgDataNodesType typesMetaInfoDatabase;

    if (typeName == EgDataNodesLinkNamespace::egLinkTypesFileName) // skip links metainfo
        return 1;

    if (serverAddress.isEmpty()) // local files
    {
        if(! CheckNodeTypesMetaInfoLocal())
            CreateNodeTypesMetaInfo();
    }

    if (typesMetaInfoDatabase.ConnectServiceNodeType(*this, EgDataNodesNamespace::egDataNodesTypesFileName))
    {
        EG_LOG_STUB  << "Can't connect nodes types meta-info, aborting" << FN;
        return -1;
    }

    typesMetaInfoDatabase.LoadAllDataNodes();

            // check if node type name exists
    for (QMap<EgDataNodeIdType, EgDataNode>::iterator nodesIter = typesMetaInfoDatabase.dataNodes.begin(); nodesIter != typesMetaInfoDatabase.dataNodes.end(); ++nodesIter)
    {
        if(nodesIter.value()["nodeTypeName"].toString() == typeName)
        {
            EG_LOG_STUB  << "Node type name alredy exists: " << typeName << FN;
            return 1;
        }
    }

    addValues << typeName;

    typesMetaInfoDatabase.AddDataNode(addValues);
    typesMetaInfoDatabase.StoreData();

    return 0;

}

/*
int EgGraphDatabase::CheckServerConnection()
{
    int res = 0;    // TODO could be error id reference

    if (! serverAddress.isEmpty())
    {
        // check if server online

        // get metainfo

    }

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
    bool res {false};

    if (! isConnected)
    {
        // EG_LOG_STUB  << "egGraphDatabase is not connected, connect it prior data nodes types" << FN;

        res = (bool) LoadNodeTypesMetaInfo(); // 0 (i.e. false) is ok

        res = res || (bool) LoadLinksMetaInfo();

        if (! res)
            isConnected = true;
    }

    if (nType)
    {
        if (! dataNodeTypes.contains(nType-> extraInfo.typeName))
            EG_LOG_STUB << "Integrity WARNING: dataNodeTypes doesn't contain " << nType-> extraInfo.typeName << FN;
        // else
        //    EG_LOG_STUB << "dataNodeTypes contains " << nType-> extraInfo.typeName << FN;

        if (! attachedNodeTypes.contains(nType-> extraInfo.typeName)) // FIXME check multi instances
            attachedNodeTypes.insert(nType-> extraInfo.typeName, nType);
    }

    return 0;
}

int EgGraphDatabase::AttachLinksType(EgLinkType* linkType)
{
    bool res {false};

    if (! isConnected)
    {
        // EG_LOG_STUB  << "egGraphDatabase is not connected, connect it prior data nodes types" << FN;

        res = (bool) LoadNodeTypesMetaInfo(); // 0 (i.e. false) is ok

        res = res || (bool) LoadLinksMetaInfo();

        if (! res)
            isConnected = true;
    }

    if (linkType)
    {
        if (! linkTypes.contains(linkType-> allLinkNames.linkName))
            EG_LOG_STUB << "Integrity WARNING: linkTypes doesn't contain " << linkType-> allLinkNames.linkName << FN;
        else
        {
            if (linkType-> fromType -> extraInfo.typeName != linkTypes[linkType-> allLinkNames.linkName].firstTypeName)
                EG_LOG_STUB << "Integrity WARNING: linksType " << linkType-> allLinkNames.linkName
                         << " has bad data node type name: " << linkType-> fromType-> extraInfo.typeName
                         << " instead of: " << linkTypes[linkType-> allLinkNames.linkName].firstTypeName << FN;

            if (linkType-> toType-> extraInfo.typeName != linkTypes[linkType-> allLinkNames.linkName].secondTypeName)
                EG_LOG_STUB << "Integrity WARNING: linksType " << linkType-> allLinkNames.linkName
                         << " has bad data node type name: " << linkType-> toType->extraInfo.typeName
                         << " instead of: " << linkTypes[linkType-> allLinkNames.linkName].secondTypeName << FN;

        }
        //    EG_LOG_STUB << "linkTypes contains " << linkType-> allLinkNames.linkName << FN;

        if (! attachedLinkTypes.contains(linkType-> allLinkNames.linkName)) // FIXME check multi instances
            attachedLinkTypes.insert(linkType-> allLinkNames.linkName, linkType);
    }

    return 0;
}


int EgGraphDatabase::LoadLinksMetaInfo()
{
    EgLinkType* newLink;
    EgDataNodesType linksMetaInfo;

    if (serverAddress.isEmpty())
    {

        dir = QDir::current();

        if (! dir.exists("egdb/" + QString(EgDataNodesLinkNamespace::egLinkTypesFileName) + ".dat"))
        {
            EG_LOG_STUB  << "eGraphDb connect error: Link types meta info doesn't exist: " << EgDataNodesLinkNamespace::egLinkTypesFileName << ".dat" << FN;

            return -1;
        }
    }

    if (linksMetaInfo.ConnectServiceNodeType(*this, EgDataNodesLinkNamespace::egLinkTypesFileName) < 0)
    {
        // EG_LOG_STUB  << "EgDB connect error: Links meta info doesn't exist: " << EgDataNodesLinkNamespace::egLinkTypesFileName << ".dat" << FN;

        return -1;
    }

    linksMetaInfo.LoadAllDataNodes();

    linkTypes.clear();

        // QMap<EgDataNodeIDtype, EgDataNode>::iterator
    for (auto nodesIter = linksMetaInfo.dataNodes.begin(); nodesIter != linksMetaInfo.dataNodes.end(); ++nodesIter)
    {
        // EG_LOG_STUB  << "nodesIter.value().dataFields = " << nodesIter.value().dataFields << FN;

        newLink = new EgLinkType(this);

        newLink-> allLinkNames.linkName = nodesIter.value()["linkTypeName"].toString();
        newLink-> allLinkNames.firstTypeName = nodesIter.value()["firstNodeType"].toString();
        newLink-> allLinkNames.secondTypeName = nodesIter.value()["secondNodeType"].toString();




        if (nodesIter.value()["linkTypeName"].toString() != QString(EgDataNodesNamespace::egDummyLinkType))
        {
            EgLinkNames newLinkNames;

            newLinkNames.linkName = nodesIter.value()["linkTypeName"].toString();
            newLinkNames.firstTypeName = nodesIter.value()["firstNodeType"].toString();
            newLinkNames.secondTypeName = nodesIter.value()["secondNodeType"].toString();

            linkTypes.insert(nodesIter.value()["linkTypeName"].toString(), newLinkNames);

            // EG_LOG_STUB  << "Link added to EgGraphDatabase: " << newLink-> linkName << FN;
        }
    }

    return 0;
}


int EgGraphDatabase::LoadNodeTypesMetaInfo()
{
    EgDataNodesType typesMetaInfoDatabase;

    if (typesMetaInfoDatabase.ConnectServiceNodeType(*this, EgDataNodesNamespace::egDataNodesTypesFileName))
    {
        EG_LOG_STUB  << "Can't connect nodes types meta-info, aborting" << FN;
        return -1;
    }

    typesMetaInfoDatabase.LoadAllDataNodes();

    dataNodeTypes.clear();

            // check if link name exists
    for (auto nodesIter = typesMetaInfoDatabase.dataNodes.begin(); nodesIter != typesMetaInfoDatabase.dataNodes.end(); ++nodesIter)
        dataNodeTypes.insert(nodesIter.value()["nodeTypeName"].toString(), nodesIter.value()["nodeTypeName"].toString());

    // EG_LOG_STUB  << "dataNodeTypes : " << dataNodeTypes.keys() << FN;

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
