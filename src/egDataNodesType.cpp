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
        // qDebug()  << "Delete location info " << metaInfo.typeName + EgDataNodesNamespace::egLocationFileName << FN;

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
        // qDebug()  << "Warning: attempt to connect again data nodes type: " << nodeTypeName << FN;

        return 1;
    }

    extraInfo.myECoGraphDB = &myDB;
    extraInfo.typeName = nodeTypeName;

    if (myDB.serverAddress.isEmpty())
    {
        if (extraInfo.LocalLoadExtraInfo())
        {
            // if (! nodeTypeName.contains(EgDataNodesNamespace::egGUIfileName))
                qDebug()  << "Can't load meta info of data nodes type " << nodeTypeName << FN;

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
                qDebug()  << "Can't load meta info of data nodes type " << nodeTypeName << FN;

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
            // qDebug()  << "Connect location info " << nodeTypeName + EgDataNodesNamespace::egLocationFileName << FN;

            locations = new EgDataNodesLocation(this);

            int locres = locations->locationStorage-> ConnectServiceNodeType(myDB, nodeTypeName + EgDataNodesNamespace::egLocationFileName);

            if (locres)
                qDebug()  << "Can't connect location info " << nodeTypeName + EgDataNodesNamespace::egLocationFileName << FN;
        }

        if (extraInfo.typeSettings.useNamedAttributes)
        {
            // qDebug()  << "Connect location info " << nodeTypeName + EgDataNodesNamespace::egLocationFileName << FN;

            namedAttributes = new EgNamedAttributes(this);

            int attrres = namedAttributes->namedAttributesStorage-> ConnectServiceNodeType(myDB, nodeTypeName + EgDataNodesNamespace::egAttributesFileName);

            if (attrres)
                qDebug()  << "Can't connect named attributes info " << nodeTypeName + EgDataNodesNamespace::egAttributesFileName << FN;
        }

        if (extraInfo.typeSettings.useEntryNodes)
        {
            // qDebug()  << "Connect entry nodes " << nodeTypeName + EgDataNodesNamespace::egEntryNodesFileName << FN;

            entryNodes = new EgEntryNodes(this);

            int entres = entryNodes->entryStorage-> ConnectServiceNodeType(myDB, nodeTypeName + EgDataNodesNamespace::egEntryNodesFileName);

            if (entres)
                qDebug()  << "Can't connect entry nodes " << nodeTypeName + EgDataNodesNamespace::egEntryNodesFileName << FN;
        }

        GUI = new EgDataNodesGUIsupport(this); // needed for basic UI interaction

        if (extraInfo.typeSettings.useGUIsettings) // FIXME server
        {
            int guires = GUI-> LoadSimpleControlDesc();

            if (guires)
                qDebug()  << "Can't load GUI settings of " << nodeTypeName << FN;
        }
    }

    initNotFoundVirtualNode(); // init special data node

    if (! res)
        res = getMyLinkTypes(); // extract nodetype-specific link types from all link types

    if (! res)
        isConnected = true;

    return res;
}

int EgDataNodesType::ConnectLinkType(const QString& linkTypeName)
{
    if (! isConnected)
    {
        qDebug()  << "Error: data nodes type " << extraInfo.typeName << " should be connected before link type: " << linkTypeName << FN;

        return -1;
    }

        // find link
    if (! myLinkTypes.contains(linkTypeName))
    {
        qDebug() << extraInfo.typeName << " : bad link type name: " << linkTypeName << FN;
        return -1;
    }

    return myLinkTypes[linkTypeName]->linksStorage-> ConnectServiceNodeType(*(extraInfo.myECoGraphDB),
                                     QString(linkTypeName + EgDataNodesLinkNamespace::egLinkFileNamePostfix));
}

int EgDataNodesType::ConnectServiceNodeType(EgGraphDatabase& myDB, const QString& nodeTypeName) // connect to server
{
    int res = 0;

        // check if already connected FIXME implement reconnect
    if (isConnected)
    {
        // qDebug()  << "Warning: attempt to connect again data nodes type: " << nodeTypeName << FN;

        return 1;
    }

    extraInfo.myECoGraphDB = &myDB;
    extraInfo.typeName = nodeTypeName;

    if (myDB.serverAddress.isEmpty())
    {
        if (extraInfo.LocalLoadExtraInfo())
        {
            // if (! nodeTypeName.contains(EgDataNodesNamespace::egGUIfileName))
                qDebug()  << "Can't load meta info of data nodes type " << nodeTypeName << FN;

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
                qDebug()  << "Can't load meta info of data nodes type " << nodeTypeName << FN;

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


int EgDataNodesType::getMyLinkTypes()
{
    if (extraInfo.myECoGraphDB)
            // QMap<QString, EgDataNodesLinkType>::iterator
        for (auto linksIter = extraInfo.myECoGraphDB-> linkTypes.begin(); linksIter != extraInfo.myECoGraphDB-> linkTypes.end(); ++linksIter)
        {
             // qDebug() << "linksIter.key() = " << linksIter.key()
             //        << "linksIter.value().firstTypeName = " << linksIter.value().firstTypeName
             //        << "linksIter.value().secondTypeName = " << linksIter.value().secondTypeName << FN;

            if ((linksIter.value().firstTypeName == extraInfo.typeName) || (linksIter.value().secondTypeName == extraInfo.typeName))
            {
                myLinkTypes.insert(linksIter.key(), &(linksIter.value()));

                // qDebug() << "Node type : " << metaInfo.typeName << "added link " << linksIter.key() << FN;
            }
        }

    return 0;
}


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
        qDebug() << extraInfo.typeName << "- not found data node for ID = " << objID << FN;
        return notFound;
    }
}

int EgDataNodesType::AddArrowLink(const QString& linkName, EgDataNodeIdType fromNode, EgDataNodesType &toType, EgDataNodeIdType toNode)
{

    EgExtendedLinkType fwdLink, backLink;
    QList<EgExtendedLinkType> newLinks;

    if ( ! myLinkTypes.contains(linkName))
    {
        qDebug() << extraInfo.typeName << " : bad link name: " << linkName << FN;
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

        myLinkTypes[linkName] -> AddLink(fromNode, toNode);

        // qDebug() << "Link " << linkName << " added " << metaInfo.typeName << " " << fromNode << " to "
        //         << toType.metaInfo.typeName << " " <<  toNode << FN;

        return 0;
    }
    else
    {
        // qDebug() << "Link " << linkName << " of " << metaInfo.typeName << " link NOT added for ID = " << fromNode << " to "
        //         << toType.metaInfo.typeName << " " << toNode << FN;

        return -1;
    }
}




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

    // qDebug() << "IndexOffsets = " << IndexOffsets << FN;

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

        // qDebug() << "IndexOffsets = " << IndexOffsets << FN;

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
    if (! myLinkTypes.contains(linkName))
    {
        qDebug() << extraInfo.typeName << " : bad link name: " << linkName << FN;
        return -1;
    }

        // load linked offsets
    // res = myLinkTypes[linkName]-> LoadLinkedNodes(IndexOffsets, fromNodeID); // FIXME

    res = myLinkTypes[linkName]-> LoadLinkedNodes(fromNodeID);

    // if (res)
    //    return res;

    IndexOffsets.clear();

        // iterate loaded links
    for (auto Iter = myLinkTypes[linkName]->linksStorage-> dataNodes.begin(); Iter != myLinkTypes[linkName]->linksStorage-> dataNodes.end(); ++Iter)
        LocalFiles->primIndexFiles-> Load_EQ(IndexOffsets, Iter.value()["to_node_id"].toInt()); //

    // qDebug() << "IndexOffsets.count() = " << IndexOffsets.count() << FN;

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

    // qDebug() << "metaInfo.nextObjID = " << metaInfo.nextObjID << FN;

        // set next available ID FIXME : thread safe
    tmpObj.dataNodeID = extraInfo.nextNodeID++;
    tmpObj.isAdded = true;
    tmpObj.extraInfo = &extraInfo;

        // copy to map and then add to pointers list
    dataNodesIter = dataNodes.insert(tmpObj.dataNodeID, tmpObj);
    addedDataNodes.insert(tmpObj.dataNodeID, &(dataNodesIter.value()));

    // qDebug() << "tmpObj.dataNodeID = " << tmpObj.dataNodeID << FN;

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
    dataNodesIter = dataNodes.insert(tmpObj.dataNodeID, tmpObj);
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
    dataNodesIter = dataNodes.insert(tmpObj.dataNodeID, tmpObj);
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
    dataNodesIter = dataNodes.insert(tmpObj.dataNodeID, tmpObj);
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


/*
int EgDataNodesType::AddLocationOfNode(QList<QVariant>& myData, EgDataNodeIDtype nodeID)
{
    if (locationNodesType)
    {
        return locationNodesType->AddHardLinked(myData, nodeID);
    }
    else
    {
        qDebug()  << "Can't add location, location type does not exist for " << metaInfo.typeName << FN;

        return -1;
    }
}
*/
int EgDataNodesType::DeleteDataNode(EgDataNodeIdType nodeID)
{
    QMap<EgDataNodeIdType, EgDataNode>::iterator dataNodesIter;
    QMap<EgDataNodeIdType, EgDataNode*>::iterator auxIter;

    dataNodesIter = dataNodes.find(nodeID);
    if (dataNodesIter == dataNodes.end())
    {
        qDebug()  << "Not found data node by ID to modify: " << nodeID << FN;
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

        deletedDataNodes.insert(dataNodesIter.key(), dataNodesIter.value());
        dataNodes.erase(dataNodesIter);

        // qDebug()  << "deletedDataNodes count " << deletedDataNodes.count() << FN;
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
        qDebug() << FN << "Not found Node ID to modify: " << nodeID;
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
        qDebug() << FN << "Not found Node ID to modify: " << nodeID;
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

        // qDebug()  << "Nodes Type: " << metaInfo.typeName << " nextNodeID: " << metaInfo.nextNodeID << FN;

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
        qDebug()  << "ERROR: got non-zero error code from subfunction, changes not saved " << FN;

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

        // qDebug() << FN << "IndexOffsets.count() = " << IndexOffsets.count();

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
        // qDebug() << "Sending node " << (int) addIter.value()-> dataNodeID << FN ;

        qDebug() << sendIter.key() ; // <<  sendIter.value();

        for (auto fieldsIter = sendIter.value().dataFields.begin(); fieldsIter != sendIter.value().dataFields.end(); ++fieldsIter)
        {
            qDebug() << *fieldsIter; // <<  sendIter.value();
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
        qDebug() << FN << "Field # " << i << " " << (*cur_desc).field_name;
        cur_desc++;
        i++;
    }
    qDebug() << FN << "ObjectsCount() = " << ObjectsCount();

    qDebug() << FN << "obj_count = " << obj_count;
    qDebug() << FN << "next_obj_id = " << next_obj_id;
        // print data
    cur_obj = dobj_map.begin();
    while (cur_obj != dobj_map.end())
    {
        qDebug() << FN << "OBJ_ID = " << (*cur_obj).OBJ_ID;
        // qDebug() << FN << "data_obj_size = " << (*cur_obj).data_obj_size;
        cur_field = cur_obj.value().data_fields.begin();
        while (cur_field != cur_obj.value().data_fields.end())
        {
            qDebug() << FN << (*cur_field).toString();// str;
            cur_field++;
        }
        cur_obj++;
    }
    */
    return 0;
}

int EgDataNodesType::StoreAllLinks()
{
    for (QMap <QString, EgDataNodesLinkType*>::iterator linksIter = myLinkTypes.begin(); linksIter != myLinkTypes.end(); ++linksIter)
        linksIter.value()-> StoreLinks();

    return 0;
}

int EgDataNodesType::LoadAllLinks()
{
    for (QMap <QString, EgDataNodesLinkType*>::iterator linksIter = myLinkTypes.begin(); linksIter != myLinkTypes.end(); ++linksIter)
        linksIter.value()-> LoadLinks();

    return 0;
}

int EgDataNodesType::StoreLinkType(QString linkName)
{
    if (! myLinkTypes.contains(linkName))
    {
        qDebug() << extraInfo.typeName << " - bad link name: " << linkName << FN;
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
        qDebug() << extraInfo.typeName << " - bad link name: " << linkName << FN;
        return -1;
    }

    myLinkTypes[linkName]-> LoadLinks();

    return 0;
}

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
        qDebug() << "Cant find data node ID of " << extraInfo.typeName << " " << hex << entryNodeID << FN;
        qDebug() << dataNodes.keys() << FN;
        return -1;
    }
}
