#include "egDataNodesType.h"
#include "egDataClient.h"
#include "egDataNodesLink.h"
#include "egGraphDatabase.h"

EgDataNodesType::EgDataNodesType():
    LocalFiles(new EgDataFiles()),
    ConnectonClient(new EgDataClient(this)), // FIXME
    connection(NULL),
    index_tree(NULL)
{
    GUI.dataNodesType = this;
}

EgDataNodesType::~EgDataNodesType()
{
    // index_tree.clear();

    if (LocalFiles)
        delete LocalFiles;

    if (ConnectonClient)
        delete ConnectonClient;

    if (connection)
        delete connection;

    if (index_tree)
        delete index_tree;
}

int EgDataNodesType::Connect(EgGraphDatabase& myDB, const QString& nodeTypeName, EgRemoteConnect* server) // connect to server
{
    int res = 0;

    metaInfo.myECoGraphDB = &myDB;
    metaInfo.typeName = nodeTypeName;
    connection = server;    

    if (! server)
    {
        if (metaInfo.LocalLoadMetaInfo())
        {
            if (! nodeTypeName.contains(EgDataNodesGUInamespace::egGUIfileName))
                qDebug()  << "Can't load meta info of data nodes type " << nodeTypeName << FN;
            res = -1;
        }
        else
            if (! nodeTypeName.contains(EgDataNodesGUInamespace::egGUIfileName))
                GUI.LoadSimpleControlDesc();

        LocalFiles-> Init(metaInfo);
        index_tree = new EgIndexConditions(nodeTypeName);
    }

        // init special data node
    notFound.metaInfo = &metaInfo;

    for (int i = 0; i < metaInfo.dataFields.count(); i++)
        notFound.dataFields << QVariant("<Not found>");

    if (! res && &myDB)
        myDB.Connect(this);

    getMyLinkTypes(); // extract nodetype-specific link types from all link types


        // FIXME process server based version

    return res;
}

int EgDataNodesType::getMyLinkTypes()
{
    if (metaInfo.myECoGraphDB)
        for (QMap<QString, EgDataNodesLinkType>::iterator linksIter = metaInfo.myECoGraphDB-> linkTypes.begin(); linksIter != metaInfo.myECoGraphDB-> linkTypes.end(); ++linksIter)
        {
            if ((linksIter.value().firstTypeName == metaInfo.typeName) || (linksIter.value().secondTypeName == metaInfo.typeName))
            {
                myLinkTypes.insert(linksIter.key(), &(linksIter.value()));

                // qDebug() << "added link " << linksIter.key() << FN;
            }
        }

    return 0;
}


void EgDataNodesType::SetLocalFilter(FilterCallbackType f_callback) // set filter callback - local data only
{
    LocalFiles -> FilterCallback = f_callback;
}


EgDataNode& EgDataNodesType::operator [](EgDataNodeIDtype objID)
{
    if (dataNodes.contains(objID))
    {
         return dataNodes[objID];
    }
    else
    {
        qDebug() << metaInfo.typeName << "- not found data node for ID = " << objID << FN;
        return notFound;
    }
}

int EgDataNodesType::AddArrowLink(QString linkName, EgDataNodeIDtype fromNode, EgDataNodesType &toType, EgDataNodeIDtype toNode)
{

    EgExtendedLinkType fwdLink, backLink;
    QList<EgExtendedLinkType> newLinks;

    if (! myLinkTypes.contains(linkName))
    {
        qDebug() << metaInfo.typeName << " : bad link name: " << linkName << FN;
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
        if (! dataNodes[fromNode].nodeLinks-> outLinks.contains(linkName))
        {
            newLinks.clear();
            newLinks.append(fwdLink);

            dataNodes[fromNode].nodeLinks-> outLinks.insert(linkName, newLinks);
        }
        else
            dataNodes[fromNode].nodeLinks-> outLinks[linkName].append(fwdLink);

            // write back link
        if (! toType.dataNodes[toNode].nodeLinks-> outLinks.contains(linkName))
        {
            newLinks.clear();
            newLinks.append(backLink);

            toType.dataNodes[toNode].nodeLinks-> outLinks.insert(linkName, newLinks);
        }
        else
            toType.dataNodes[toNode].nodeLinks-> outLinks[linkName].append(backLink);

            // add record to store links

        myLinkTypes[linkName] -> AddLink(fromNode, toNode);

        // qDebug() << metaInfo.typeName << "link added " << fromNode << "to" <<  toNode << FN;

        return 0;
    }
    else
    {
        qDebug() << metaInfo.typeName << " : not found data node for ID = " << fromNode << " or " << toType.metaInfo.typeName << " " <<  toNode << FN;
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
    return LocalFiles->RemoveLocalFiles();
}


int EgDataNodesType::LoadAllData()
{
    index_tree-> clear();
    index_tree-> AddNode(NULL, 1, 0, GE, "odb_pit", 1);

    return LoadData();
}


int EgDataNodesType::CompressData()
{
    if (connection)
       return -1; // return ConnectonClient->RemoteCompressData();
    else
        return LocalFiles->LocalCompressData();
}

int EgDataNodesType::AddNewData(EgDataNode& tmpObj)
{
    QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodesIter;

        // set next available ID FIXME : thread safe
    tmpObj.dataNodeID = metaInfo.nextObjID++;
    tmpObj.isAdded = true;
    tmpObj.metaInfo = &metaInfo;

        // copy to map and then add to pointers list
    dataNodesIter = dataNodes.insert(tmpObj.dataNodeID, tmpObj);
    addedDataNodes.insert(tmpObj.dataNodeID, &(dataNodesIter.value()));

    return 0;
}

int EgDataNodesType::AddNewData(QList<QVariant>& myData)
{
    QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodesIter;
    EgDataNode tmpObj;

        // set next available ID FIXME : thread safe
    tmpObj.dataNodeID = metaInfo.nextObjID++;
    tmpObj.dataFields = myData;
    tmpObj.isAdded = true;
    tmpObj.metaInfo = &metaInfo;

        // copy to map and then add to pointers list
    dataNodesIter = dataNodes.insert(tmpObj.dataNodeID, tmpObj);
    addedDataNodes.insert(tmpObj.dataNodeID, &(dataNodesIter.value()));

    return 0;
}

int EgDataNodesType::MarkDeletedData(EgDataNodeIDtype nodeID)
{
    QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodesIter;
    QMap<EgDataNodeIDtype, EgDataNode*>::iterator auxIter;

    dataNodesIter = dataNodes.find(nodeID);
    if (dataNodesIter == dataNodes.end())
    {
        qDebug() << FN << "Not found data node by ID to modify: " << nodeID;
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
    }

    return 0;
}

int EgDataNodesType::SetModifiedData(QList<QVariant>& myData, EgDataNodeIDtype nodeID)
{
    QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodesIter;
    QMap<EgDataNodeIDtype, EgDataNode*>::iterator auxIter;

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

int EgDataNodesType::SetModifiedData(EgDataNodeIDtype nodeID)
{
    QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodesIter;
    QMap<EgDataNodeIDtype, EgDataNode*>::iterator auxIter;

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

        // pack data and update index fields

    /*

    if (connection)
        ret_val = ConnectonClient-> StoreData(packed_add_list, packed_del_list, packed_mod_list);
    else
        */

    ret_val = LocalFiles-> LocalStoreData(addedDataNodes, deletedDataNodes, updatedDataNodes);

    deletedDataNodes.clear();
    addedDataNodes.clear();
    updatedDataNodes.clear();

    if (ret_val)
       qDebug() << FN << "ERROR: got non-zero error code from callee" ;

    return  ret_val;
}

int EgDataNodesType::LoadData()
{
    int res = 0;
       // clear lists
    deletedDataNodes.clear();
    addedDataNodes.clear();
    updatedDataNodes.clear();

        // clear objects data
    dataNodes.clear();

    index_tree-> CalcTreeSet(IndexOffsets, LocalFiles);

    // qDebug() << FN << "IndexOffsets.count() = " << IndexOffsets.count();

    if (! IndexOffsets.isEmpty())
        res = LocalFiles-> LocalLoadData(IndexOffsets, dataNodes);

    index_tree-> clear();

    return res;
}



int EgDataNodesType::PrintObjData() // debug print
{
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

int EgDataNodesType::StoreLinks()
{
    for (QMap <QString, EgDataNodesLinkType*>::iterator linksIter = myLinkTypes.begin(); linksIter != myLinkTypes.end(); ++linksIter)
        linksIter.value()-> StoreLinks();

    return 0;
}

int EgDataNodesType::LoadLinks()
{
    for (QMap <QString, EgDataNodesLinkType*>::iterator linksIter = myLinkTypes.begin(); linksIter != myLinkTypes.end(); ++linksIter)
        linksIter.value()-> LoadLinks();

    return 0;
}

int EgDataNodesType::StoreLink(QString linkName)
{
    if (! myLinkTypes.contains(linkName))
    {
        qDebug() << metaInfo.typeName << " - bad link name: " << linkName << FN;
        return -1;
    }

    myLinkTypes[linkName]-> StoreLinks();

    return 0;
}


int EgDataNodesType::LoadLink(QString linkName)
{
    if (! myLinkTypes.contains(linkName))
    {
        qDebug() << metaInfo.typeName << " - bad link name: " << linkName << FN;
        return -1;
    }

    myLinkTypes[linkName]-> LoadLinks();

    return 0;
}
