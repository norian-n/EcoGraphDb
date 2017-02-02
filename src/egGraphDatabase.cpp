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

    if (! connNodeTypes.contains(nType-> metaInfo.typeName)) // FIXME multi instances
        connNodeTypes.insert(nType-> metaInfo.typeName, nType);

    return 0;
}

int EgGraphDatabase::LoadLinksMetaInfo()
{
    EgDataNodesLinkType* newLink;

/*
        // check if GUI descriptors exists - FIXME for server
    QFile dat_file;
    dat_file.setFileName(dataNodesType-> metaInfo.typeName + egGUIfileName + ".dat");

    if (!dat_file.exists())
    {
        // qDebug() << FN << "file  doesn't exist" << dataNodesType-> metaInfo.typeName + egGUIfileName + ".dat";
        return -1;
    }
*/

    EgDataNodesType linksMetaInfo;

    /*
    CreateNodeType(EgDataNodesLinkNamespace::egLinkTypesFileName);

    AddDataField("name");
    AddDataField("firstNodeType");
    AddDataField("secondNodeType");

    linksMetaInfo.metaInfo = *metaInfo;

    if (! linksMetaInfo.LocalFiles-> CheckMetaInfoFile(*metaInfo))  // FIXME server
        return -1; // no data found

    linksMetaInfo.LocalFiles-> Init(*metaInfo);

    linksMetaInfo.index_tree = new EgIndexConditions(EgDataNodesLinkNamespace::egLinkTypesFileName);



    if (linksMetaInfo.Connect(*this, EgDataNodesLinkNamespace::egLinkTypesFileName))
    {
        qDebug()  << "No links found " << linksMetaInfo.metaInfo.typeName << FN;
        return 1;
    }

        */

    // int res = 0;

    /*
    linksMetaInfo.metaInfo.myECoGraphDB = this;
    linksMetaInfo.metaInfo.typeName = EgDataNodesLinkNamespace::egLinkTypesFileName;
    // connection = server;

    if (! linksMetaInfo.LocalFiles-> CheckMetaInfoFile(linksMetaInfo.metaInfo))  // FIXME server
    {
        qDebug()  << "Can't load linksMetaInfo " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;
        return -1;
    }

    if (linksMetaInfo.metaInfo.LocalLoadMetaInfo())
    {
        qDebug()  << "Can't load linksMetaInfo " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;
        return -1;
    }

    linksMetaInfo.LocalFiles-> Init(linksMetaInfo.metaInfo);
    // linksMetaInfo.index_tree = new EgIndexConditions(EgDataNodesLinkNamespace::egLinkTypesFileName);
    */

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

int EgGraphDatabase::CreateNodeType(QString typeName, bool addLocation, bool addAttributes)
{
    // FIXME check if node type exists

    if (metaInfo)
        delete metaInfo;

    metaInfo = new EgDataNodeTypeMetaInfo(typeName);

    metaInfo-> useLocationsNodes = addLocation;

    if (locationMetaInfo)
    {
        delete locationMetaInfo;
        locationMetaInfo = NULL;
    }

    if (attributesMetaInfo)
    {
        delete attributesMetaInfo;
        attributesMetaInfo = NULL;
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
        qDebug()  << "CreateNodeType wasn't called" << FN;
        return -1;
    }

    metaInfo-> AddDataField(fieldName, indexed);

    return 0;
}

int EgGraphDatabase::AddLocationField(QString fieldName, bool indexed)
{
    if (! locationMetaInfo)
    {
        qDebug()  << "CreateNodeType wasn't called for node location, call it properly" << FN;
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

    metaInfo-> LocalStoreMetaInfo();

    if (locationMetaInfo)
    {
            // add default fields: x,y
        locationMetaInfo-> AddDataField("x");
        locationMetaInfo-> AddDataField("y");

        locationMetaInfo-> LocalStoreMetaInfo();
    }

    if (attributesMetaInfo)
    {
            // add named attributes fields
        attributesMetaInfo-> AddDataField("nodeid", isIndexed);
        attributesMetaInfo-> AddDataField("key");
        attributesMetaInfo-> AddDataField("value");

        attributesMetaInfo-> LocalStoreMetaInfo();
    }

    return 0;
}

/*
int EgGraphDatabase::CreateControlDescs()
{
    if (! metaInfo)
    {
        qDebug()  << "CreateNodeType wasn't called" << FN;
        return -1;
    }

    EgDataNodeTypeMetaInfo controlDescMetaInfo(metaInfo-> typeName + EgDataNodesGUInamespace::egGUIfileName);

    controlDescMetaInfo.AddDataField("name");
    controlDescMetaInfo.AddDataField("label");
    controlDescMetaInfo.AddDataField("width");

        // store meta-info
    controlDescMetaInfo.LocalStoreMetaInfo();

    return 0;
}

int  EgGraphDatabase::AddSimpleControlDesc(QString fieldName, QString fieldLabel, int fieldWidth)
{
    QList<QVariant> addValues;

    if (! metaInfo)
    {
        qDebug()  << "CreateNodeType wasn't called, aborting" << FN;
        return -1;
    }

    if (controlDescs.Connect(*(metaInfo-> myECoGraphDB), metaInfo-> typeName + EgDataNodesGUInamespace::egGUIfileName))
    {
        qDebug()  << "CreateControlDescs wasn't called, aborting" << FN;
        return -2;
    }

    addValues << fieldName << fieldLabel << fieldWidth;

    controlDescs.AddDataNode(addValues);
    // controlDescs.StoreData();

    return 0;

}

int  EgGraphDatabase::CommitControlDesc()
{
    return controlDescs.StoreData();
}
*/

int EgGraphDatabase::CreateEgDb()
{
    // check if metainfo exists

    EgDataNodesType linksMetaInfo;

    linksMetaInfo.metaInfo.myECoGraphDB = this;
    linksMetaInfo.metaInfo.typeName = EgDataNodesLinkNamespace::egLinkTypesFileName;
    // connection = server;

    if (linksMetaInfo.LocalFiles-> CheckMetaInfoFile(linksMetaInfo.metaInfo))  // FIXME server
    {
        // qDebug()  << "linksMetaInfo already exists: " << EgDataNodesLinkNamespace::egLinkTypesFileName << FN;
        return 1;
    }

    CreateNodeType(EgDataNodesLinkNamespace::egLinkTypesFileName);

    AddDataField("name");
    AddDataField("firstNodeType");
    AddDataField("secondNodeType");

    CommitNodeType();

    return 0;
}

int  EgGraphDatabase::AddLinkType(QString linkName, QString firstDataNodeType, QString secondDataNodeType)
{
    QList<QVariant> addValues;
    EgDataNodesType linksTypes;

    /*
    if (! metaInfo)
    {
        qDebug()  << "CreateLinksMetaInfo wasn't called, aborting" << FN;
        return -1;
    }
    */

    if (linksTypes.Connect(*this, EgDataNodesLinkNamespace::egLinkTypesFileName))
    {
        qDebug()  << "CreateLinksMetaInfo wasn't called, aborting" << FN;
        return -1;
    }

        // FIXME - check if link name exists

    CreateNodeType(linkName + EgDataNodesLinkNamespace::egLinkFileNamePostfix);

    AddDataField("from_node_id", isIndexed);
    AddDataField("to_node_id");                 // FIXME check if index required

    CommitNodeType();

    addValues << linkName << firstDataNodeType << secondDataNodeType;

    linksTypes.AddDataNode(addValues);
    linksTypes.StoreData();

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
