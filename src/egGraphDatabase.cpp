/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egGraphDatabase.h"
#include "egDataNodesLink.h"

EgGraphDatabase::~EgGraphDatabase()
{
    if (metaInfo)
        delete metaInfo;
    if (locationMetaInfo)
        delete locationMetaInfo;
    if (attributesMetaInfo)
        delete attributesMetaInfo;
    if (GUIcontrolsMetaInfo)
        delete GUIcontrolsMetaInfo;
    if (entryNodesMetaInfo)
        delete entryNodesMetaInfo;
}


bool EgGraphDatabase::CheckLinksMetaInfo()
{
    // EgDataNodesType linksMetaInfo;

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


int EgGraphDatabase::CreateLinksMetaInfo()
{
    // EgNodeTypeSettings typeSettings;

        // init links meta-info
    if (metaInfo)
    {
        delete metaInfo;
    }

    fieldsCreated = 0;
    locationFieldsCreated = 0;

    metaInfo = new EgDataNodeTypeMetaInfo(EgDataNodesLinkNamespace::egLinkTypesFileName);

        // init settings
    metaInfo-> useEntryNodes = false;
    metaInfo-> useLocation = false;
    metaInfo-> useNamedAttributes = false;
    metaInfo-> useLinks = false;
    metaInfo-> useGUIsettings = false;

    ClearMetaInfo(GUIcontrolsMetaInfo);
    ClearMetaInfo(entryNodesMetaInfo);
    ClearMetaInfo(locationMetaInfo);
    ClearMetaInfo(attributesMetaInfo);

        // add fields
    AddDataField("name");
    AddDataField("firstNodeType");
    AddDataField("secondNodeType");

    CommitNodeType();

    QList<QVariant> addValues;
    EgDataNodesType linksMetaInfo;

    linksMetaInfo.Connect(*this, EgDataNodesLinkNamespace::egLinkTypesFileName);

    addValues << QString(EgDataNodesNamespace::egDummyLinkType)
              << QString(EgDataNodesNamespace::egDummyNodesType)
              << QString(EgDataNodesNamespace::egDummyNodesType);

    linksMetaInfo.AddDataNode(addValues);
    linksMetaInfo.StoreData();

    return 0;
}

inline void EgGraphDatabase::ClearMetaInfo(EgDataNodeTypeMetaInfo* metaInfo)
{
    if (metaInfo)
    {
        delete metaInfo;
        metaInfo = nullptr;
    }
}

int EgGraphDatabase::CreateNodeType(QString typeName,  EgNodeTypeSettings& typeSettings)
{
    // check/create DB metainfo

    if(! CheckLinksMetaInfo())
        CreateLinksMetaInfo();

        // check if node type exists
    if (dir.exists("egdb/" + typeName + ".ddt"))
    {
        qDebug()  << "Data node type already exists: " << typeName << FN;

        return 1;
    }

        // init all
    if (metaInfo)
    {
        qDebug()  << "Warning: node type metainfo exists, CommitNodeType() wasn't called properly " << typeName << FN;
        delete metaInfo;
    }

    fieldsCreated = 0;
    locationFieldsCreated = 0;

    metaInfo = new EgDataNodeTypeMetaInfo(typeName);

        // store settings
    metaInfo-> useEntryNodes = typeSettings.useEntryNodes;
    metaInfo-> useLocation = typeSettings.useLocation;
    metaInfo-> useNamedAttributes = typeSettings.useNamedAttributes;
    metaInfo-> useLinks = typeSettings.useLinks;
    metaInfo-> useGUIsettings = typeSettings.useGUIsettings;

    ClearMetaInfo(GUIcontrolsMetaInfo);
    ClearMetaInfo(entryNodesMetaInfo);
    ClearMetaInfo(locationMetaInfo);
    ClearMetaInfo(attributesMetaInfo);

    if (typeSettings.useGUIsettings)
        GUIcontrolsMetaInfo = new EgDataNodeTypeMetaInfo(typeName + EgDataNodesNamespace::egGUIfileName);

    if (typeSettings.useEntryNodes)
        entryNodesMetaInfo = new EgDataNodeTypeMetaInfo(typeName + EgDataNodesNamespace::egEntryNodesFileName);

    if (typeSettings.useLocation)
        locationMetaInfo = new EgDataNodeTypeMetaInfo(typeName + EgDataNodesNamespace::egLocationFileName);

    if (typeSettings.useNamedAttributes)
        attributesMetaInfo = new EgDataNodeTypeMetaInfo(typeName + EgDataNodesNamespace::egAttributesFileName);

    return 0;
}

/*
int EgGraphDatabase::CreateNodeType(QString typeName, bool addLocation, bool addAttributes)
{
        // check if node type exists
    if (dir.exists("egdb/" + typeName + ".ddt"))
    {
        qDebug()  << "Data node type already exists: " << typeName << FN;

        return 1;
    }

        // init all
    if (metaInfo)
    {
        qDebug()  << "Warning: node type metainfo exists, CommitNodeType() wasn't called properly " << typeName << FN;
        delete metaInfo;
    }

    fieldsCreated = 0;
    locationFieldsCreated = 0;

    metaInfo = new EgDataNodeTypeMetaInfo(typeName);

    metaInfo-> useLocation        = addLocation;
    metaInfo-> useNamedAttributes = addAttributes;

    if (locationMetaInfo)
    {
        delete locationMetaInfo;
        locationMetaInfo = nullptr;
    }

    if (attributesMetaInfo)
    {
        delete attributesMetaInfo;
        attributesMetaInfo = nullptr;
    }

    if (addLocation)
        locationMetaInfo = new EgDataNodeTypeMetaInfo(typeName + EgDataNodesNamespace::egLocationFileName);

    if (addAttributes)
        attributesMetaInfo = new EgDataNodeTypeMetaInfo(typeName + EgDataNodesNamespace::egAttributesFileName);

    return 0;
}
*/

int EgGraphDatabase::AddDataField(QString fieldName, bool uint32index)
{
    if (! metaInfo)
    {
        qDebug()  << "CreateNodeType() wasn't called, field not added " << fieldName << FN;
        return -1;
    }

    metaInfo-> AddDataField(fieldName, uint32index);

    return 0;
}

int EgGraphDatabase::AddLocationField(QString fieldName, bool uint32index)
{
    if (! locationMetaInfo)
    {
        qDebug()  << "CreateNodeType() wasn't called for node location field, it was not added " << fieldName << FN;
        return -1;
    }

    locationMetaInfo-> AddDataField(fieldName, uint32index);

    return 0;
}


int EgGraphDatabase::AddDataField(QString fieldName, EgIndexSettings indexSettings)
{
    if (! metaInfo)
    {
        qDebug()  << "CreateNodeType() wasn't called, field not added " << fieldName << FN;
        return -1;
    }

    metaInfo-> AddDataField(fieldName, indexSettings);

    return 0;
}

int EgGraphDatabase::AddLocationField(QString fieldName, EgIndexSettings indexSettings)
{
    if (! locationMetaInfo)
    {
        qDebug()  << "CreateNodeType() wasn't called for node location field, it was not added " << fieldName << FN;
        return -1;
    }

    locationMetaInfo-> AddDataField(fieldName, indexSettings);

    return 0;
}

int EgGraphDatabase::CommitNodeType()
{
    if (! metaInfo)
    {
        qDebug()  << "CreateNodeType wasn't called" << FN;
        return -1;
    }

    fieldsCreated = metaInfo->dataFields.count();

    metaInfo-> LocalStoreMetaInfo();

    if (GUIcontrolsMetaInfo)
    {
        GUIcontrolsMetaInfo-> AddDataField("name");
        GUIcontrolsMetaInfo-> AddDataField("label");
        GUIcontrolsMetaInfo-> AddDataField("width");

        GUIcontrolsMetaInfo-> LocalStoreMetaInfo();

        delete GUIcontrolsMetaInfo;
        GUIcontrolsMetaInfo = nullptr;
    }

    if (entryNodesMetaInfo)
    {
        entryNodesMetaInfo-> AddDataField("subgraphid");    // hard linked by node id, field is optional

        entryNodesMetaInfo-> LocalStoreMetaInfo();

        delete entryNodesMetaInfo;
        entryNodesMetaInfo = nullptr;
    }

    if (locationMetaInfo)
    {
            // add default fields: x,y
        locationMetaInfo-> AddDataField("x");
        locationMetaInfo-> AddDataField("y");

        locationFieldsCreated = locationMetaInfo->dataFields.count();

        locationMetaInfo-> LocalStoreMetaInfo();

        delete locationMetaInfo;
        locationMetaInfo = nullptr;
    }

    if (attributesMetaInfo)
    {
            // add named attributes fields
        attributesMetaInfo-> AddDataField("nodeid", isIndexed);
        attributesMetaInfo-> AddDataField("key");
        attributesMetaInfo-> AddDataField("value");

        attributesMetaInfo-> LocalStoreMetaInfo();

        delete attributesMetaInfo;
        attributesMetaInfo = nullptr;
    }

    if (metaInfo)
    {
        delete metaInfo;
        metaInfo = nullptr;
    }

    return 0;
}

int  EgGraphDatabase::AddLinkType(QString linkName, QString firstDataNodeType, QString secondDataNodeType)
{
    QList<QVariant> addValues;
    EgDataNodesType linksMetaInfo; // == LinkTypes

    if(! CheckLinksMetaInfo())
        CreateLinksMetaInfo();

    if (linksMetaInfo.Connect(*this, EgDataNodesLinkNamespace::egLinkTypesFileName))
    {
        qDebug()  << "Can't connect links meta-info, aborting" << FN;
        return -1;
    }

    // linkTypes.clear();

    linksMetaInfo.LoadAllNodes();

            // check if link name exists
    for (QMap<EgDataNodeIDtype, EgDataNode>::iterator nodesIter = linksMetaInfo.dataNodes.begin(); nodesIter != linksMetaInfo.dataNodes.end(); ++nodesIter)
    {
        if(nodesIter.value()["name"].toString() == linkName)
        {
            qDebug()  << "Link name alredy exists: " << linkName << FN;
            return 1;
        }
    }

        // add new link type
    EgNodeTypeSettings typeSettings;

    CreateNodeType(linkName + EgDataNodesLinkNamespace::egLinkFileNamePostfix, typeSettings);

    AddDataField("from_node_id", isIndexed);
    AddDataField("to_node_id");                 // FIXME check if index required

    CommitNodeType();

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

int EgGraphDatabase::Connect()
{
    int res = 0;

    if (! isConnected)
    {
        res = LoadLinksMetaInfo();   // 1 means empty meta, negative is error


        // res could be error id reference, so no simple bool conversion
        if (! res)
            isConnected = true;
    }

    return res;
}

    /*

int EgGraphDatabase::CreateEgDb()
{

        // check if metainfo exists
    if (dir.exists(QString("egdb/") + EgDataNodesLinkNamespace::egLinkTypesFileName + ".ddt"))
    {
        return 1;
    }

    EgNodeTypeSettings typeSettings;

        // links meta-info
    CreateNodeType(EgDataNodesLinkNamespace::egLinkTypesFileName, typeSettings);

    AddDataField("name");
    AddDataField("firstNodeType");
    AddDataField("secondNodeType");

    CommitNodeType();


    return 0;
}

    */

// int EgGraphDatabase::Attach(EgDataNodesType* nType)
// {
    /*
    if (! isConnected)
    {
        qDebug()  << "egGraphDatabase is not connected, connect it prior data nodes types" << FN;

        return -1;
    }
    */

//    if (! connNodeTypes.contains(nType-> metaInfo.typeName)) // FIXME check multi instances
//        connNodeTypes.insert(nType-> metaInfo.typeName, nType);

//     return 0;
// }

int EgGraphDatabase::LoadLinksMetaInfo()
{
    EgDataNodesLinkType* newLink;
    EgDataNodesType linksMetaInfo;

    if (! dir.exists("egdb/" + QString(EgDataNodesLinkNamespace::egLinkTypesFileName) + ".dat"))
    {
        qDebug()  << "EgDB connect error: Links meta info doesn't exist: " << EgDataNodesLinkNamespace::egLinkTypesFileName << ".dat" << FN;

        return -1;
    }

    if (linksMetaInfo.Connect(*this, EgDataNodesLinkNamespace::egLinkTypesFileName) < 0)
    {
        // qDebug()  << "EgDB connect error: Links meta info doesn't exist: " << EgDataNodesLinkNamespace::egLinkTypesFileName << ".dat" << FN;

        return -1;
    }

    linksMetaInfo.LoadAllNodes();

    linkTypes.clear();

        // QMap<EgDataNodeIDtype, EgDataNode>::iterator
    for (auto nodesIter = linksMetaInfo.dataNodes.begin(); nodesIter != linksMetaInfo.dataNodes.end(); ++nodesIter)
    {
        // qDebug()  << "nodesIter.value().dataFields = " << nodesIter.value().dataFields << FN;

        newLink = new EgDataNodesLinkType(this);

        newLink-> linkName = nodesIter.value()["name"].toString();
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
