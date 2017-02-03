/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "egGraphDatabase.h"
#include "egDataNodesLink.h"

int EgGraphDatabase::CreateEgDb()
{
        // check if metainfo exists
    if (dir.exists(QString("egdb/") + EgDataNodesLinkNamespace::egLinkTypesFileName + ".ddt"))
    {
        return 1;
    }

    CreateNodeType(EgDataNodesLinkNamespace::egLinkTypesFileName);

    AddDataField("name");
    AddDataField("firstNodeType");
    AddDataField("secondNodeType");

    CommitNodeType();

    return 0;
}

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

    metaInfo-> useLocationsNodes = addLocation;
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

int EgGraphDatabase::AddDataField(QString fieldName, bool indexed)
{
    if (! metaInfo)
    {
        qDebug()  << "CreateNodeType() wasn't called, field not added " << fieldName << FN;
        return -1;
    }

    metaInfo-> AddDataField(fieldName, indexed);

    return 0;
}

int EgGraphDatabase::AddLocationField(QString fieldName, bool indexed)
{
    if (! locationMetaInfo)
    {
        qDebug()  << "CreateNodeType() wasn't called for node location field, it was not added " << fieldName << FN;
        return -1;
    }

    locationMetaInfo-> AddDataField(fieldName, indexed);

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

    if (linksMetaInfo.Connect(*this, EgDataNodesLinkNamespace::egLinkTypesFileName))
    {
        qDebug()  << "CreateEgDb wasn't called, aborting" << FN;
        return -1;
    }

        // FIXME - check if link name exists

    linksMetaInfo.LoadAllData();

    linkTypes.clear();

    for (QMap<EgDataNodeIDtype, EgDataNode>::iterator nodesIter = linksMetaInfo.dataNodes.begin(); nodesIter != linksMetaInfo.dataNodes.end(); ++nodesIter)
    {
        if(nodesIter.value()["name"].toString() == linkName)
        {
            qDebug()  << "Link name alredy exists: " << linkName << FN;
            return 1;
        }
    }

    CreateNodeType(linkName + EgDataNodesLinkNamespace::egLinkFileNamePostfix);

    AddDataField("from_node_id", isIndexed);
    AddDataField("to_node_id");                 // FIXME check if index required

    CommitNodeType();

    addValues << linkName << firstDataNodeType << secondDataNodeType;

    linksMetaInfo.AddDataNode(addValues);
    linksMetaInfo.StoreData();

    return 0;

}

int EgGraphDatabase::Connect()
{
    if (! isConnected)
    {
       LoadLinksMetaInfo();
    }

    isConnected = true;

    return 0;
}

int EgGraphDatabase::Attach(EgDataNodesType* nType)
{
    /*
    if (! isConnected)
    {
        qDebug()  << "egGraphDatabase is not connected, connect it prior data nodes types" << FN;

        return -1;
    }
    */

    if (! connNodeTypes.contains(nType-> metaInfo.typeName)) // FIXME check multi instances
        connNodeTypes.insert(nType-> metaInfo.typeName, nType);

    return 0;
}

int EgGraphDatabase::LoadLinksMetaInfo()
{
    EgDataNodesLinkType* newLink;
    EgDataNodesType linksMetaInfo;

    linksMetaInfo.Connect(*this, EgDataNodesLinkNamespace::egLinkTypesFileName); // FIXME check

    linksMetaInfo.LoadAllData();

    linkTypes.clear();

    for (QMap<EgDataNodeIDtype, EgDataNode>::iterator nodesIter = linksMetaInfo.dataNodes.begin(); nodesIter != linksMetaInfo.dataNodes.end(); ++nodesIter)
    {
        // qDebug()  << "nodesIter.value().dataFields = " << nodesIter.value().dataFields << FN;

        newLink = new EgDataNodesLinkType(this);

        newLink-> linkName = nodesIter.value()["name"].toString();
        newLink-> firstTypeName = nodesIter.value()["firstNodeType"].toString();
        newLink-> secondTypeName = nodesIter.value()["secondNodeType"].toString();

        linkTypes.insert(newLink-> linkName, *newLink);

        // qDebug()  << "Link added to EgGraphDatabase: " << newLink.linkName << FN;
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
