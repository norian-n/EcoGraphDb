/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egDataNodesType.h"
#include "egDataClient.h"
#include "egDataNodesLink.h"
#include "egGraphDatabase.h"

EgDataNodesType::EgDataNodesType():
    LocalFiles (new EgDataFiles())
{
    // GUI.primaryNodesType = this;
}

EgDataNodesType::~EgDataNodesType()
{
    // index_tree.clear();

    if (LocalFiles)
        delete LocalFiles;

    if (locations)
        delete locations;
    /*
    if (locationNodesType)
    {
        // EG_LOG_STUB  << "Delete location info " << metaInfo.typeName + EgDataNodesNamespace::egLocationFileName << FN;

        delete locationNodesType;
    }
    */

    /*
    if (namedAttributesType)
    {

        delete namedAttributesType;
    }
    */

    if (index_tree)
        delete index_tree;
}

int EgDataNodesType::Connect(EgGraphDatabase& myDB, const QString& nodeTypeName) // connect to server
{
    int res = 0;

        // check if already connected FIXME implement reconnect
    if (isConnected)
    {
        // EG_LOG_STUB  << "Warning: attempt to connect again data nodes type: " << nodeTypeName << FN;

        return 1;
    }

    extraInfo.myECoGraphDB = &myDB;
    extraInfo.typeName = nodeTypeName;

    if (myDB.serverAddress.isEmpty())
    {
        if (extraInfo.LocalLoadExtraInfo())
        {
            // if (! nodeTypeName.contains(EgDataNodesNamespace::egGUIfileName))
                EG_LOG_STUB  << "Can't load meta info of data nodes type " << nodeTypeName << FN;

            res = -1;
        }

        LocalFiles-> Init(extraInfo);
        index_tree = new EgIndexConditionsTree();

    }
    else
    {
        if (extraInfo.ServerLoadExtraInfo())
        {
            // if (! nodeTypeName.contains(EgDataNodesNamespace::egGUIfileName))
                EG_LOG_STUB  << "Can't load meta info of data nodes type " << nodeTypeName << FN;

            res = -1;
        }
    }

                // connect to peer database controller
    if (! res)
        res = myDB.AttachNodesType(this);  // TODO FIXME implement double-check files

        // connect support node types
    if (! res)
    {
        if (extraInfo.typeSettings.useLocation)
        {
            // EG_LOG_STUB  << "Connect location info " << nodeTypeName + EgDataNodesNamespace::egLocationFileName << FN;

            locations = new EgDataNodesLocation(this);

            int locres = locations->locationStorage-> ConnectServiceNodeType(myDB, nodeTypeName + EgDataNodesNamespace::egLocationFileName);

            if (locres)
                EG_LOG_STUB  << "Can't connect location info " << nodeTypeName + EgDataNodesNamespace::egLocationFileName << FN;
        }

        if (extraInfo.typeSettings.useNamedAttributes)
        {
            // EG_LOG_STUB  << "Connect location info " << nodeTypeName + EgDataNodesNamespace::egLocationFileName << FN;

            namedAttributes = new EgNamedAttributes(this);

            int attrres = namedAttributes->namedAttributesStorage-> ConnectServiceNodeType(myDB, nodeTypeName + EgDataNodesNamespace::egAttributesFileName);

            if (attrres)
                EG_LOG_STUB  << "Can't connect named attributes info " << nodeTypeName + EgDataNodesNamespace::egAttributesFileName << FN;
        }

        if (extraInfo.typeSettings.useEntryNodes)
        {
            // EG_LOG_STUB  << "Connect entry nodes " << nodeTypeName + EgDataNodesNamespace::egEntryNodesFileName << FN;

            entryNodes = new EgEntryNodes(this);

            int entres = entryNodes->entryStorage-> ConnectServiceNodeType(myDB, nodeTypeName + EgDataNodesNamespace::egEntryNodesFileName);

            if (entres)
                EG_LOG_STUB  << "Can't connect entry nodes " << nodeTypeName + EgDataNodesNamespace::egEntryNodesFileName << FN;
        }

        GUI = new EgDataNodesGUIsupport(this); // needed for basic UI interaction

        if (extraInfo.typeSettings.useGUIsettings) // FIXME server
        {
            int guires = GUI-> LoadSimpleControlDesc();

            if (guires)
                EG_LOG_STUB  << "Can't load GUI settings of " << nodeTypeName << FN;
        }
    }

    initNotFoundVirtualNode(); // init special data node

    // if (! res)
    //    res = getMyLinkTypes(); // extract nodetype-specific link types from all link types

    if (! res)
        isConnected = true;

    return res;
}

/*
int EgDataNodesType::ConnectLinkType(const QString& linkTypeName)
{
    if (! isConnected)
    {
        EG_LOG_STUB  << "Error: data nodes type " << extraInfo.typeName << " should be connected before link type: " << linkTypeName << FN;

        return -1;
    }

        // find link
    if (! myLinkTypes.contains(linkTypeName))
    {
        EG_LOG_STUB << extraInfo.typeName << " : bad link type name: " << linkTypeName << FN;
        return -1;
    }

    return myLinkTypes[linkTypeName]->linksStorage-> ConnectServiceNodeType(*(extraInfo.myECoGraphDB),
                                     QString(linkTypeName + EgDataNodesLinkNamespace::egLinkFileNamePostfix));
}
*/

int EgDataNodesType::ConnectServiceNodeType(EgGraphDatabase& myDB, const QString& nodeTypeName) // connect to server
{
    int res = 0;

        // check if already connected FIXME implement reconnect
    if (isConnected)
    {
        // EG_LOG_STUB  << "Warning: attempt to connect again data nodes type: " << nodeTypeName << FN;

        return 1;
    }

    extraInfo.myECoGraphDB = &myDB;
    extraInfo.typeName = nodeTypeName;

    if (myDB.serverAddress.isEmpty())
    {
        if (extraInfo.LocalLoadExtraInfo())
        {
            // if (! nodeTypeName.contains(EgDataNodesNamespace::egGUIfileName))
                EG_LOG_STUB  << "Can't load meta info of data nodes type " << nodeTypeName << FN;

            res = -1;
        }

        LocalFiles-> Init(extraInfo);
        index_tree = new EgIndexConditionsTree();

    }
    else
    {
        if (extraInfo.ServerLoadExtraInfo())
        {
            // if (! nodeTypeName.contains(EgDataNodesNamespace::egGUIfileName))
                EG_LOG_STUB  << "Can't load meta info of data nodes type " << nodeTypeName << FN;

            res = -1;
        }
    }

    initNotFoundVirtualNode();

    if (! res)
        isConnected = true;

    return res;
}

inline void EgDataNodesType::initNotFoundVirtualNode()
{
        // init special data node
    notFound.extraInfo = &extraInfo;
    notFound.dataFields.clear();

    for (int i = 0; i < extraInfo.dataFields.count(); i++)
        notFound.dataFields << QVariant("<Not found>");
}

/*
int EgDataNodesType::getMyLinkTypes()
{
    if (extraInfo.myECoGraphDB)
            // QMap<QString, EgDataNodesLinkType>::iterator
        for (auto linksIter = extraInfo.myECoGraphDB-> linkTypes.begin(); linksIter != extraInfo.myECoGraphDB-> linkTypes.end(); ++linksIter)
        {
             // EG_LOG_STUB << "linksIter.key() = " << linksIter.key()
             //        << "linksIter.value().firstTypeName = " << linksIter.value().firstTypeName
             //        << "linksIter.value().secondTypeName = " << linksIter.value().secondTypeName << FN;

            if ((linksIter.value().allLinkNames.firstTypeName == extraInfo.typeName) || (linksIter.value().allLinkNames.secondTypeName == extraInfo.typeName))
            {
                myLinkTypes.insert(linksIter.key(), &(linksIter.value()));

                // EG_LOG_STUB << "Node type : " << metaInfo.typeName << "added link " << linksIter.key() << FN;
            }
        }

    return 0;
}
*/


void EgDataNodesType::SetLocalFilter(FilterFunctionType theFunction) // set filter callback - local data only
{
    LocalFiles-> FilterCallback = theFunction;
}

void EgDataNodesType::SetFilterParams(QList<QVariant>& values)
{
    LocalFiles-> filterValues.clear();
    LocalFiles-> filterValues = values;
}


EgDataNode& EgDataNodesType::operator [](EgDataNodeIdType objID)
{
    if (dataNodes.contains(objID))
    {
         return dataNodes[objID];
    }
    else
    {
        EG_LOG_STUB << extraInfo.typeName << "- not found data node for ID = " << objID << FN;
        return notFound;
    }
}


/*
int EgDataNodesType::AddArrowLink(const QString& linkName, EgDataNodeIdType fromNode, EgDataNodesType &toType, EgDataNodeIdType toNode)
{

    EgExtendedLinkType fwdLink, backLink;
    QList<EgExtendedLinkType> newLinks;

    */

    /*
    if ( ! myLinkTypes.contains(linkName))
    {
        EG_LOG_STUB << extraInfo.typeName << " : bad link name: " << linkName << FN;
        return -1;
    }
    */

 /*   if (! extraInfo.myECoGraphDB-> attachedLinkTypes.contains(linkName))
    {
        EG_LOG_STUB << extraInfo.typeName << " : link type was not connected : " << linkName << FN;
        return -1;
    }

    if (dataNodes.contains(fromNode) && toType.dataNodes.contains(toNode))
    {
            // fill new links info
        fwdLink.dataNodeID = toNode;
        fwdLink.dataNodePtr = &(toType.dataNodes[toNode]);

        backLink.dataNodeID = fromNode;
        backLink.dataNodePtr = &(dataNodes[fromNode]);

            // check/create links
        if (! dataNodes[fromNode].nodeLinks)
          dataNodes[fromNode].nodeLinks = new EgDataNodeLinks();

        if (! toType.dataNodes[toNode].nodeLinks)
          toType.dataNodes[toNode].nodeLinks = new EgDataNodeLinks();

            // write fwd link
        if (dataNodes[fromNode].nodeLinks-> outLinks.contains(linkName))
            dataNodes[fromNode].nodeLinks-> outLinks[linkName].append(fwdLink);
        else
        {
            newLinks.clear();
            newLinks.append(fwdLink);

            dataNodes[fromNode].nodeLinks-> outLinks.insert(linkName, newLinks);
        }

            // FIXME RUNTIME SEGFAULT write back link
        if (toType.dataNodes[toNode].nodeLinks-> outLinks.contains(linkName))
            toType.dataNodes[toNode].nodeLinks-> outLinks[linkName].append(backLink);
        else
        {
            newLinks.clear();
            newLinks.append(backLink);

            toType.dataNodes[toNode].nodeLinks-> outLinks.insert(linkName, newLinks);
        }
            // add record to store links

        extraInfo.myECoGraphDB-> attachedLinkTypes[linkName] -> AddLinkToStorageOnly(fromNode, toNode);

        // EG_LOG_STUB << "Link " << linkName << " added " << metaInfo.typeName << " " << fromNode << " to "
        //         << toType.metaInfo.typeName << " " <<  toNode << FN;

        return 0;
    }
    else
    {
        // EG_LOG_STUB << "Link " << linkName << " of " << metaInfo.typeName << " link NOT added for ID = " << fromNode << " to "
        //         << toType.metaInfo.typeName << " " << toNode << FN;

        return -1;
    }
}
*/



/*
EgDataNodesType& EgDataNodesType::operator << (EgDataNode& d_obj)
{

    // check field descriptors

    // look in the map

    // update

    // add

    return this;
}
*/


int EgDataNodesType::RemoveLocalFiles()
{
    // FIXME check dir
    return LocalFiles->RemoveLocalFiles();
}

void EgDataNodesType::ClearData()
{
        // clear changes lists
    deletedDataNodes.clear();
    addedDataNodes.clear();
    updatedDataNodes.clear();

        // clear data
    dataNodes.clear();

        // FIXME - clear all support data

    if (entryNodes)
    {
        entryNodes->entryStorage->ClearData();
        entryNodes->entryNodesList.clear();
    }

    if (locations)
        locations->locationStorage-> ClearData();

    // if (namedAttributes)
    //    namedAttributes->namedAttributesStorage-> ClearData();
}

int EgDataNodesType::LoadAllDataNodes()
{
    int res = 0;

    ClearData();

    IndexOffsets.clear();

    // EG_LOG_STUB << "IndexOffsets = " << IndexOffsets << FN;

    if (extraInfo.myECoGraphDB-> serverConnection)
    {
        res = extraInfo.myECoGraphDB-> serverConnection-> OpenLoadStream(opcode_load_all_data, extraInfo.myECoGraphDB-> serverStream, extraInfo.typeName);

        if (! res)
        {
            res = extraInfo.myECoGraphDB-> serverConnection-> WaitForSending(); // command

            // qDebug << serverConnection-> tcpSocket.state() << "" << ; WaitForReadyRead

            if (! res)
                res = extraInfo.myECoGraphDB-> serverConnection-> WaitForReadyRead();

            if(! res)
                LocalFiles-> ReceiveDataNodes(dataNodes, *(extraInfo.myECoGraphDB-> serverStream));

            extraInfo.myECoGraphDB-> serverConnection-> Disconnect();
        }

    }
    else    // local
    {
        LocalFiles-> primIndexFiles -> LoadAllDataOffsets(IndexOffsets);

        if (! IndexOffsets.isEmpty())
            res = LocalFiles-> LocalLoadData(IndexOffsets, dataNodes);
    }

    return res;
}

int EgDataNodesType::AutoLoadAllData()
{
    int res = 0;

    ClearData();

    if (locations)
        locations->locationStorage-> ClearData();

    if (entryNodes)
        entryNodes->entryStorage-> ClearData();

    if (namedAttributes)
        namedAttributes->namedAttributesStorage-> ClearData();

    IndexOffsets.clear();

    if (extraInfo.myECoGraphDB-> serverConnection)
    {
        res = extraInfo.myECoGraphDB-> serverConnection-> OpenLoadStream(opcode_load_all_data, extraInfo.myECoGraphDB-> serverStream, extraInfo.typeName);

        if (! res)
        {
            res = extraInfo.myECoGraphDB-> serverConnection-> WaitForSending(); // command

            // qDebug << serverConnection-> tcpSocket.state() << "" << ; WaitForReadyRead

            if (! res)
                res = extraInfo.myECoGraphDB-> serverConnection-> WaitForReadyRead();

            if(! res)
                LocalFiles-> ReceiveDataNodes(dataNodes, *(extraInfo.myECoGraphDB-> serverStream));

            extraInfo.myECoGraphDB-> serverConnection-> Disconnect();
        }

    }
    else    // local
    {
        LocalFiles-> primIndexFiles -> LoadAllDataOffsets(IndexOffsets);

        // EG_LOG_STUB << "IndexOffsets = " << IndexOffsets << FN;

        if (! IndexOffsets.isEmpty())
            res = LocalFiles-> LocalLoadData(IndexOffsets, dataNodes);
    }


    // load secondary info
    if (locations && ! res)
        locations->locationStorage-> LoadAllDataNodes();

    if (entryNodes && ! res)
        entryNodes->entryStorage-> LoadAllDataNodes();

    if (namedAttributes && ! res)
        namedAttributes->namedAttributesStorage-> LoadAllDataNodes();

    return res;
}

int EgDataNodesType::LoadLinkedData(QString linkName, EgDataNodeIdType fromNodeID)
{
    int res = 0;

    ClearData();

        // find link
    /*
    if (! myLinkTypes.contains(linkName))
    {
        EG_LOG_STUB << extraInfo.typeName << " : bad link name: " << linkName << FN;
        return -1;
    }
    */

        // load linked offsets
    // res = myLinkTypes[linkName]-> LoadLinkedNodes(IndexOffsets, fromNodeID); // FIXME

    if (! extraInfo.myECoGraphDB-> attachedLinkTypes.contains(linkName))
    {
        EG_LOG_STUB << extraInfo.typeName << " : link type was not connected : " << linkName << FN;
        return -1;
    }

    res = extraInfo.myECoGraphDB-> attachedLinkTypes[linkName]-> LoadLinkedNodesFrom(fromNodeID);

    // res = myLinkTypes[linkName]-> LoadLinkedNodes(fromNodeID);

    // if (res)
    //    return res;

    IndexOffsets.clear();

        // iterate loaded links
    for (auto Iter = extraInfo.myECoGraphDB-> attachedLinkTypes[linkName]->linksStorage-> dataNodes.begin(); Iter != extraInfo.myECoGraphDB-> attachedLinkTypes[linkName]->linksStorage-> dataNodes.end(); ++Iter)
        LocalFiles->primIndexFiles-> Load_EQ(IndexOffsets, Iter.value()["to_node_id"].toInt()); //

    // EG_LOG_STUB << "IndexOffsets.count() = " << IndexOffsets.count() << FN;

    if (! IndexOffsets.isEmpty())
        res = LocalFiles-> LocalLoadData(IndexOffsets, dataNodes);

    if (! res && entryNodes)
        res = entryNodes->LoadEntryNodes();
        // res = entryNodesInst.LoadEntryNodes(*this);

    return res;
}

/*
int EgDataNodesType::LoadLocationsData()
{
    int res = 0;

    // EgDataNodeIDtype dataNodeID;

    if (locationNodesType)
    {
        locationNodesType-> ClearData();

        locationNodesType-> IndexOffsets.clear();

        for (QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodeIter = dataNodes.begin(); dataNodeIter != dataNodes.end(); ++dataNodeIter)
            locationNodesType-> LocalFiles-> primIndexFiles-> Load_EQ(locationNodesType->IndexOffsets, dataNodeIter.key());


        if (! locationNodesType-> IndexOffsets.isEmpty())
            res = locationNodesType-> LocalFiles-> LocalLoadData(locationNodesType-> IndexOffsets, locationNodesType-> dataNodes);

    }

    return res;
}
*/

int EgDataNodesType::CompressData()
{
    if (extraInfo.myECoGraphDB-> serverConnection)
       return -1; // return ConnectonClient->RemoteCompressData();  FIXME
    else
        return LocalFiles->LocalCompressData();
}

int EgDataNodesType::AddDataNode(EgDataNode& tmpObj)
{
    QMap<EgDataNodeIdType, EgDataNode>::iterator dataNodesIter;

    // EG_LOG_STUB << "metaInfo.nextObjID = " << metaInfo.nextObjID << FN;

        // set next available ID FIXME : thread safe
    tmpObj.dataNodeID = extraInfo.nextNodeID++;
    tmpObj.isAdded = true;
    tmpObj.extraInfo = &extraInfo;

        // copy to map and then add to pointers list
    dataNodesIter = dataNodes.insert(tmpObj.dataNodeID, std::move(tmpObj));
    addedDataNodes.insert(tmpObj.dataNodeID, &(dataNodesIter.value()));

    // EG_LOG_STUB << "tmpObj.dataNodeID = " << tmpObj.dataNodeID << FN;

    return 0;
}

int EgDataNodesType::AddDataNode(QList<QVariant>& myData)
{
    QMap<EgDataNodeIdType, EgDataNode>::iterator dataNodesIter;
    EgDataNode tmpObj;

        // set next available ID FIXME : thread safe
    tmpObj.dataNodeID = extraInfo.nextNodeID++;
    tmpObj.dataFields = myData;
    tmpObj.isAdded = true;
    tmpObj.extraInfo = &extraInfo;

        // copy to map and then add to pointers list
    dataNodesIter = dataNodes.insert(tmpObj.dataNodeID, std::move(tmpObj));
    addedDataNodes.insert(tmpObj.dataNodeID, &(dataNodesIter.value()));

    return 0;
}

int EgDataNodesType::AddDataNode(QList<QVariant>& myData, EgDataNodeIdType& newID)
{
    QMap<EgDataNodeIdType, EgDataNode>::iterator dataNodesIter;
    EgDataNode tmpObj;

        // set next available ID FIXME : thread safe
    tmpObj.dataNodeID = extraInfo.nextNodeID++;
    tmpObj.dataFields = myData;
    tmpObj.isAdded = true;
    tmpObj.extraInfo = &extraInfo;

        // copy to map and then add to pointers list
    dataNodesIter = dataNodes.insert(tmpObj.dataNodeID, std::move(tmpObj));
    addedDataNodes.insert(tmpObj.dataNodeID, &(dataNodesIter.value()));

    newID = tmpObj.dataNodeID;

    return 0;
}


int EgDataNodesType::AddHardLinked(QList<QVariant>& myData, EgDataNodeIdType nodeID)
{
    QMap<EgDataNodeIdType, EgDataNode>::iterator dataNodesIter;
    EgDataNode tmpObj;

        // set next available ID FIXME : thread safe
    tmpObj.dataNodeID = nodeID;
    extraInfo.nextNodeID = (extraInfo.nextNodeID >= nodeID) ?  extraInfo.nextNodeID+1 : nodeID;

    tmpObj.dataFields = myData;
    tmpObj.isAdded = true;
    tmpObj.extraInfo = &extraInfo;

        // copy to map and then add to pointers list
    dataNodesIter = dataNodes.insert(tmpObj.dataNodeID, std::move(tmpObj));
    addedDataNodes.insert(tmpObj.dataNodeID, &(dataNodesIter.value()));

    return 0;
}

int EgDataNodesType::AddLocation(QList<QVariant>& locationData, EgDataNodeIdType nodeID)
{
    return locations-> AddLocation(locationData, nodeID);
}


int EgDataNodesType::GetLocation(QList<QVariant>& locationData, EgDataNodeIdType nodeID)
{
    return locations-> GetLocation(locationData, nodeID);
}

int EgDataNodesType::UpdateLocation(QList<QVariant>& locationData, EgDataNodeIdType nodeID)
{
    return locations-> UpdateLocation(locationData, nodeID);
}


/*
int EgDataNodesType::AddLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID)
{
    if (locationNodesType)
    {
        return locationNodesType->AddHardLinked(myData, nodeID);
    }
    else
    {
        EG_LOG_STUB  << "Can't add location, location type does not exist for " << metaInfo.typeName << FN;

        return -1;
    }
}
*/


int EgDataNodesType::DeleteLinksOfNode(EgDataNodeIdType nodeID)
{
    auto const& dataNodeIter = dataNodes.find(nodeID);

    if (dataNodeIter != dataNodes.end())    // node found
    {
        if (dataNodeIter-> nodeLinks)
        {
                // remove outlinks
            for (auto linksIter = dataNodeIter->nodeLinks-> outLinks.begin(); linksIter !=  dataNodeIter->nodeLinks-> outLinks.end(); linksIter++)
            {
                for (auto linkIter : linksIter.value())
                {

                        // delete corresponding node link
                    auto findIter = linkIter.dataNodePtr ->nodeLinks->inLinks.find(linksIter.key());
                    if (findIter != linkIter.dataNodePtr ->nodeLinks->inLinks.end()) // link type found
                        for (auto listIter = findIter.value().begin(); listIter != findIter.value().end(); listIter++)
                        {
                            if (listIter-> dataNodeID == nodeID) // got it
                            {
                              findIter.value().erase(listIter);
                              break;
                            }
                        }

                        // delete link in database
                    if (! extraInfo.myECoGraphDB-> attachedLinkTypes.contains(linksIter.key()))
                    {
                        EG_LOG_STUB << extraInfo.typeName << " : link type was not connected : " << linksIter.key() << FN;
                    }

                    extraInfo.myECoGraphDB-> attachedLinkTypes[linksIter.key()]-> DeleteLink(linkIter.linkID);

                    qDebug() << " Delete out link of nodeID " << nodeID << " by linkID " << linkIter.linkID << FN;
                }
            }
                // remove inlinks
            for (auto linksIter = dataNodeIter->nodeLinks-> inLinks.begin(); linksIter !=  dataNodeIter->nodeLinks-> inLinks.end(); linksIter++)
            {
                for (auto linkIter : linksIter.value())
                {

                        // delete corresponding node link
                    auto findIter = linkIter.dataNodePtr ->nodeLinks->outLinks.find(linksIter.key());
                    if (findIter != linkIter.dataNodePtr ->nodeLinks->outLinks.end()) // link type found
                        for (auto listIter = findIter.value().begin(); listIter != findIter.value().end(); listIter++)
                        {
                            if (listIter-> dataNodeID == nodeID) // got it
                            {
                                findIter.value().erase(listIter);
                                break;
                            }
                        }

                        // delete link in database
                    if (! extraInfo.myECoGraphDB-> attachedLinkTypes.contains(linksIter.key()))
                    {
                        EG_LOG_STUB << extraInfo.typeName << " : link type was not connected : " << linksIter.key() << FN;
                    }

                    extraInfo.myECoGraphDB-> attachedLinkTypes[linksIter.key()]-> DeleteLink(linkIter.linkID);

                    qDebug() << " Delete in link of nodeID " << nodeID << " by linkID " << linkIter.linkID << FN;
                }
            }
        }
    }
    return 0;
}

int EgDataNodesType::DeleteDataNode(EgDataNodeIdType nodeID)
{
    QMap<EgDataNodeIdType, EgDataNode>::iterator dataNodesIter;
    QMap<EgDataNodeIdType, EgDataNode*>::iterator auxIter;

    dataNodesIter = dataNodes.find(nodeID);
    if (dataNodesIter == dataNodes.end())
    {
        EG_LOG_STUB  << "Not found data node by ID to modify: " << nodeID << FN;
        return -1;
    }

        // check if just added
    auxIter = addedDataNodes.find(nodeID);
    if (auxIter != addedDataNodes.end())
    {
        addedDataNodes.erase(auxIter);
        dataNodes.erase(dataNodesIter);
    }
    else
    {
            // check if node was updated
        auxIter = updatedDataNodes.find(nodeID);
        if (auxIter != updatedDataNodes.end())
            updatedDataNodes.erase(auxIter);

            // FIXME move semantics required for internal lists
        deletedDataNodes.insert(dataNodesIter.key(), dataNodesIter.value());
        dataNodes.erase(dataNodesIter);

        // EG_LOG_STUB  << "deletedDataNodes count " << deletedDataNodes.count() << FN;
    }

    return 0;
}

int EgDataNodesType::UpdateDataNode(QList<QVariant>& myData, EgDataNodeIdType nodeID)
{
    QMap<EgDataNodeIdType, EgDataNode>::iterator dataNodesIter;
    QMap<EgDataNodeIdType, EgDataNode*>::iterator auxIter;

    dataNodesIter = dataNodes.find(nodeID);
    if (dataNodesIter == dataNodes.end())
    {
        EG_LOG_STUB << FN << "Not found Node ID to modify: " << nodeID;
        return -1;
    }

    dataNodesIter.value().dataFields.clear();
    dataNodesIter.value().dataFields = myData;

            // check if node just added
    auxIter = addedDataNodes.find(nodeID);
    if (auxIter != addedDataNodes.end())
        return 0;

        // check if node updated first time
    auxIter = updatedDataNodes.find(nodeID);
    if (auxIter == updatedDataNodes.end())
        updatedDataNodes.insert(dataNodesIter.key(), &(dataNodesIter.value()));

    return 0;
}

int EgDataNodesType::UpdateDataNode(EgDataNodeIdType nodeID)
{
    QMap<EgDataNodeIdType, EgDataNode>::iterator dataNodesIter;
    QMap<EgDataNodeIdType, EgDataNode*>::iterator auxIter;

    dataNodesIter = dataNodes.find(nodeID);
    if (dataNodesIter == dataNodes.end())
    {
        EG_LOG_STUB << FN << "Not found Node ID to modify: " << nodeID;
        return -1;
    }

            // check if node just added
    auxIter = addedDataNodes.find(nodeID);
    if (auxIter != addedDataNodes.end())
        return 0;

        // check if node updated first time
    auxIter = updatedDataNodes.find(nodeID);
    if (auxIter == updatedDataNodes.end())
        updatedDataNodes.insert(dataNodesIter.key(), &(dataNodesIter.value()));

    return 0;
}



int EgDataNodesType::StoreData()
{
    int ret_val = 0;

    // check empty lists
    if (! (addedDataNodes.isEmpty() && deletedDataNodes.isEmpty() && updatedDataNodes.isEmpty())) // not all are empty
    {
            // update metainfo
        extraInfo.nodesCount += addedDataNodes.count() - deletedDataNodes.count();

        // EG_LOG_STUB  << "Nodes Type: " << metaInfo.typeName << " nextNodeID: " << metaInfo.nextNodeID << FN;

        // pack data and update index fields
        if (extraInfo.myECoGraphDB-> serverConnection)
        {
            ret_val = extraInfo.ServerStoreExtraInfo();

            if (! ret_val)
                ret_val = extraInfo.myECoGraphDB-> serverConnection-> OpenStoreStream(opcode_store_data, extraInfo.myECoGraphDB-> serverStream, extraInfo.typeName);

                // send added nodes
            if (! ret_val)
            {
                LocalFiles-> SendNodesToStream(addedDataNodes, *(extraInfo.myECoGraphDB-> serverStream));

                extraInfo.myECoGraphDB-> serverConnection-> WaitForSending();

                LocalFiles-> SendNodesToStream(deletedDataNodes, *(extraInfo.myECoGraphDB-> serverStream));

                extraInfo.myECoGraphDB-> serverConnection-> WaitForSending();

                LocalFiles-> SendNodesToStream(updatedDataNodes, *(extraInfo.myECoGraphDB-> serverStream));

                extraInfo.myECoGraphDB-> serverConnection-> WaitForSending();
            }


            extraInfo.myECoGraphDB-> serverConnection-> Disconnect();
        }
        else
        {
            ret_val = extraInfo.LocalStoreExtraInfo();

            if(! ret_val)
                ret_val = LocalFiles-> LocalStoreData(addedDataNodes, deletedDataNodes, updatedDataNodes);
        }

        if (! ret_val) // FIXME check
        {
            deletedDataNodes.clear();
            addedDataNodes.clear();
            updatedDataNodes.clear();
        }
    }

    if (!ret_val)
    {
        // if (entryNodes)
        //    entryNodes->entryStorage-> StoreData();

        if (locations)
            locations->locationStorage-> StoreData();

        if (namedAttributes)
            namedAttributes->namedAttributesStorage-> StoreData();
    }
    else
        EG_LOG_STUB  << "ERROR: got non-zero error code from subfunction, changes not saved " << FN;

    return  ret_val;
}

int EgDataNodesType::LoadDataByIndexes(QString a_FieldName, int an_oper, QVariant a_value)
{
    EgIndexCondition indexCondition(a_FieldName, an_oper, a_value);

    return LoadDataByIndexes(indexCondition);
}


int EgDataNodesType::LoadDataByIndexes(const EgIndexCondition &indexCondition)
{
    int res = 0;

    ClearData();

    if (extraInfo.myECoGraphDB-> serverConnection)
    {
        res = extraInfo.myECoGraphDB-> serverConnection-> OpenStoreStream(opcode_load_selected_data, extraInfo.myECoGraphDB-> serverStream, extraInfo.typeName);

        if (! res)
        {
            index_tree-> TransferTreeSet(indexCondition.iTreeNode, *(extraInfo.myECoGraphDB-> serverStream));

            res = extraInfo.myECoGraphDB-> serverConnection-> WaitForSending();

            // qDebug << serverConnection-> tcpSocket.state() << "" << ; WaitForReadyRead

            if (! res)
                res = extraInfo.myECoGraphDB-> serverConnection-> WaitForReadyRead();

            if(! res)
                LocalFiles-> ReceiveDataNodes(dataNodes, extraInfo.myECoGraphDB-> serverConnection-> in);


            extraInfo.myECoGraphDB-> serverConnection-> Disconnect();
        }
    }
    else
    {

        index_tree-> CalcTreeSet(indexCondition.iTreeNode, IndexOffsets, LocalFiles);

        // EG_LOG_STUB << "IndexOffsets.count() = " << IndexOffsets.count() << FN;

        if (! IndexOffsets.isEmpty())
        {
            res = LocalFiles-> LocalLoadData(IndexOffsets, dataNodes);

            // FIXME special auto load

            if (! res && locations)
                res = locations-> LoadLocationsData();

            if (! res && namedAttributes)
                res = namedAttributes-> LoadNamedAttributes();

            if (! res && entryNodes)
                res = entryNodes->LoadEntryNodes();

            // entryNodesInst.LoadEntryNodes(*this);
        }

        index_tree-> RecursiveClear(indexCondition.iTreeNode);
    }

    return res;
}

int EgDataNodesType::PrintObjData() // debug print
{
    for (auto sendIter = dataNodes.begin(); sendIter != dataNodes.end(); ++sendIter)
    {
        // EG_LOG_STUB << "Sending node " << (int) addIter.value()-> dataNodeID << FN ;

        EG_LOG_STUB << sendIter.key() ; // <<  sendIter.value();

        for (auto fieldsIter = sendIter.value().dataFields.begin(); fieldsIter != sendIter.value().dataFields.end(); ++fieldsIter)
        {
            EG_LOG_STUB << *fieldsIter; // <<  sendIter.value();
        }
    }

    /*
    QMap<EgDataNodeIDtype, EgDataNode>::iterator cur_obj;
    QList<QVariant>::iterator cur_field;
    QList<EgFieldDesc>::iterator cur_desc;

    int i = 0;
        // print headers

    cur_desc = FD.field_desc_list.begin();
    while (cur_desc != FD.field_desc_list.end())
    {
        EG_LOG_STUB << FN << "Field # " << i << " " << (*cur_desc).field_name;
        cur_desc++;
        i++;
    }
    EG_LOG_STUB << FN << "ObjectsCount() = " << ObjectsCount();

    EG_LOG_STUB << FN << "obj_count = " << obj_count;
    EG_LOG_STUB << FN << "next_obj_id = " << next_obj_id;
        // print data
    cur_obj = dobj_map.begin();
    while (cur_obj != dobj_map.end())
    {
        EG_LOG_STUB << FN << "OBJ_ID = " << (*cur_obj).OBJ_ID;
        // EG_LOG_STUB << FN << "data_obj_size = " << (*cur_obj).data_obj_size;
        cur_field = cur_obj.value().data_fields.begin();
        while (cur_field != cur_obj.value().data_fields.end())
        {
            EG_LOG_STUB << FN << (*cur_field).toString();// str;
            cur_field++;
        }
        cur_obj++;
    }
    */
    return 0;
}

/*
int EgDataNodesType::StoreAllLinks()
{
    for (QMap <QString, EgLinkType*>::iterator linksIter = myLinkTypes.begin(); linksIter != myLinkTypes.end(); ++linksIter)
        linksIter.value()-> StoreLinks();

    return 0;
}

int EgDataNodesType::LoadAllLinks()
{
    for (QMap <QString, EgLinkType*>::iterator linksIter = myLinkTypes.begin(); linksIter != myLinkTypes.end(); ++linksIter)
        linksIter.value()-> LoadLinks();

    return 0;
}
*/

/*
int EgDataNodesType::StoreLinkType(QString linkName)
{
    if (! myLinkTypes.contains(linkName))
    {
        EG_LOG_STUB << extraInfo.typeName << " - bad link name: " << linkName << FN;
        return -1;
    }

    myLinkTypes[linkName]-> StoreLinks();

    return 0;
}

// int EgDataNodesType::LoadLinkedIDsOnly(QString linkName)
int EgDataNodesType::LoadLinkType(QString linkName)
{
    if (! myLinkTypes.contains(linkName))
    {
        EG_LOG_STUB << extraInfo.typeName << " - bad link name: " << linkName << FN;
        return -1;
    }

    myLinkTypes[linkName]-> LoadLinks();

    return 0;
}
*/

int EgDataNodesType::AddEntryNode(EgDataNodeIdType entryNodeID)
{
    if (dataNodes.contains(entryNodeID))
    {
        // entryNodesInst.AddEntryNode(*this, dataNodes[entryNodeID]);
        // entryNodesInst.StoreEntryNodes(*this);

        entryNodes->AddEntryNode(entryNodeID);

        return 0;
    }
    else
    {
        EG_LOG_STUB << "Cant find data node ID of " << extraInfo.typeName << " " << hex << entryNodeID << FN;
        EG_LOG_STUB << dataNodes.keys() << FN;
        return -1;
    }
}
