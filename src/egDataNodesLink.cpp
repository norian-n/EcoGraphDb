/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egDataNodesLink.h"
#include "egDataNodesType.h"
#include "egGraphDatabase.h"

using namespace EgDataNodesLinkNamespace;

EgLinkType::EgLinkType():
  linksStorage(new EgDataNodesType())
{

}

EgLinkType::EgLinkType(EgGraphDatabase *theDatabase):
    egDatabase(theDatabase)
  , linksStorage(new EgDataNodesType())
{

}



EgLinkType::~EgLinkType()
{
    if (linksStorage)
        delete linksStorage;
}


int EgLinkType::AddLinkToStorageOnly (EgDataNodeIdType fromNodeID, EgDataNodeIdType toNodeID)
{

    QList<QVariant> myData;

    myData << fromNodeID << toNodeID;

    return linksStorage-> AddDataNode(myData);
}

int EgLinkType::AddArrowLink (EgDataNodeIdType fromNodeID, EgDataNodeIdType toNodeID)
{

    QList<QVariant> myData;

    myData << fromNodeID << toNodeID;

    linksStorage-> AddDataNode(myData);

    EgExtendedLinkType fwdLink, backLink;
    QList<EgExtendedLinkType> newLinks;

        // check if types connected
    if (!fromType || !toType)
    {
        EG_LOG_STUB << allLinkNames.linkName << " : data type(s) not connected to link type " << FN;

        return -1;
    }


    if (fromType-> dataNodes.contains(fromNodeID) && toType-> dataNodes.contains(toNodeID))
    {
            // fill new links info
        fwdLink.dataNodeID = toNodeID;
        fwdLink.dataNodePtr = &(toType-> dataNodes[toNodeID]);

        backLink.dataNodeID = fromNodeID;
        backLink.dataNodePtr = &(fromType-> dataNodes[fromNodeID]);

            // check/create links
        if (! fromType-> dataNodes[fromNodeID].nodeLinks)
          fromType-> dataNodes[fromNodeID].nodeLinks = new EgDataNodeLinks();

        if (! toType-> dataNodes[toNodeID].nodeLinks)
          toType-> dataNodes[toNodeID].nodeLinks = new EgDataNodeLinks();

            // write fwd link
        if (fromType-> dataNodes[fromNodeID].nodeLinks-> outLinks.contains(allLinkNames.linkName))
            fromType-> dataNodes[fromNodeID].nodeLinks-> outLinks[allLinkNames.linkName].append(fwdLink);
        else
        {
            newLinks.clear();
            newLinks.append(fwdLink);

            fromType-> dataNodes[fromNodeID].nodeLinks-> outLinks.insert(allLinkNames.linkName, newLinks);
        }

            // FIXME RUNTIME SEGFAULT write back link
        if (toType-> dataNodes[toNodeID].nodeLinks-> outLinks.contains(allLinkNames.linkName))
            toType-> dataNodes[toNodeID].nodeLinks-> outLinks[allLinkNames.linkName].append(backLink);
        else
        {
            newLinks.clear();
            newLinks.append(backLink);

            toType-> dataNodes[toNodeID].nodeLinks-> outLinks.insert(allLinkNames.linkName, newLinks);
        }
    }
    else
    {
        EG_LOG_STUB << "Link " << allLinkNames.linkName << " of " << fromType->extraInfo.typeName << " link NOT added in memory for ID = " << fromNodeID << " to "
                 << toType->extraInfo.typeName << " " << toNodeID << FN;

        // return -1;
    }

    // EG_LOG_STUB << linkName << ": link added " << fromNodeID << "to" <<  toNodeID << FN;

    // EG_LOG_STUB << allLinkNames.linkName << " Next link node ID " << linksStorage->metaInfo.nextNodeID << FN;

    return 0;
}

int EgLinkType::Connect(EgGraphDatabase& myDB, const QString& linkTypeName, EgDataNodesType& aFromType, EgDataNodesType& aToType)
{
        // check if already connected FIXME implement reconnect
    if (isConnected)
    {
        EG_LOG_STUB  << "Warning: attempt to connect again data link type: " << allLinkNames.linkName << " as " << linkTypeName << FN;

        return -1;
    }
/*
        // get nodes types from database
    if (! myDB.getNodesTypesOfLink(linkTypeName))
    {
        // EG_LOG_STUB  << "Error: can't find data nodes type of link type: " << linkTypeName << FN;

        return -1;
    }
*/

    allLinkNames.linkName = linkTypeName;
    egDatabase = &myDB;

    fromType = &aFromType;
    toType = &aToType;

    allLinkNames.firstTypeName  = aFromType.extraInfo.typeName;
    allLinkNames.secondTypeName = aToType.extraInfo.typeName;

    myDB.AttachLinksType(this); // FIXME aFromType, aToType check

        // connect links storage
    int res = linksStorage-> ConnectServiceNodeType(myDB,
                                     QString(linkTypeName + EgDataNodesLinkNamespace::egLinkFileNamePostfix));

    if (! res)
      isConnected = true;

    return res;
}

int EgLinkType::DeleteLink (EgDataNodeIdType linkNodeID)
{
        // FIXME check

    linksStorage-> DeleteDataNode(linkNodeID);

    return 0;
}

int EgLinkType::StoreLinks()
{
    if (! isConnected)
    {
        linksStorage-> ConnectServiceNodeType(*egDatabase, allLinkNames.linkName + egLinkFileNamePostfix);

        isConnected = true;
    }

    // EG_LOG_STUB << allLinkNames.linkName << " Added links count: " << linksStorage->addedDataNodes.count() << FN;

    linksStorage-> StoreData();

    return 0;
}

int EgLinkType::LoadLinks()
{
    if (! isConnected)
    {
        linksStorage-> ConnectServiceNodeType(*egDatabase, allLinkNames.linkName + egLinkFileNamePostfix);

        isConnected = true;
    }

    linksStorage-> LoadAllDataNodes(); // FIXME check if links already loaded

    // EG_LOG_STUB << allLinkNames.linkName << " Links count: " << linksStorage-> dataNodes.count() << FN;

    // linksStorage-> PrintObjData();

    return 0;
}

int EgLinkType::ResolveNodeTypes()
{
    auto iterFirst = egDatabase-> attachedNodeTypes.find(allLinkNames.firstTypeName);
    auto iterSecond = egDatabase-> attachedNodeTypes.find(allLinkNames.secondTypeName);

    if (iterFirst != egDatabase-> attachedNodeTypes.end() && iterSecond != egDatabase-> attachedNodeTypes.end())
    {
        fromType = iterFirst.value();
        toType = iterSecond.value();

        return 0;
    }

    return 1;
}

int EgLinkType::ResolveLinksToPointers()
{
    EgDataNodeIdType fromNode, toNode;
    EgExtendedLinkType fwdLink, backLink;

    QList<EgExtendedLinkType> newLinks;

    if (! fromType || ! toType)
        if (ResolveNodeTypes()) // bad ret code
        {
            EG_LOG_STUB << "EgDataNodesType not found: " << allLinkNames.firstTypeName << " or " << allLinkNames.secondTypeName << FN;

            return -1;
        }

        // iterate loaded links
    for (auto Iter = linksStorage-> dataNodes.begin(); Iter != linksStorage-> dataNodes.end(); ++Iter)
    {
            // find first node
        fromNode = Iter.value()["from_node_id"].toInt();
        toNode = Iter.value()["to_node_id"].toInt();

        // EG_LOG_STUB << "LinkName: " << allLinkNames.linkName << " From Node ID = " << fromNode << " To Node ID = " << toNode << FN;

        if (fromType-> dataNodes.contains(fromNode) && toType-> dataNodes.contains(toNode))
        {
                // fill new links info
            fwdLink.dataNodeID = toNode;
            fwdLink.dataNodePtr = &(toType-> dataNodes[toNode]);

            if (! fwdLink.dataNodePtr-> nodeLinks)
                fwdLink.dataNodePtr-> nodeLinks = new EgDataNodeLinks();

            backLink.dataNodeID = fromNode;
            backLink.dataNodePtr = &(fromType-> dataNodes[fromNode]);

            if (! backLink.dataNodePtr-> nodeLinks)
                backLink.dataNodePtr-> nodeLinks = new EgDataNodeLinks();

                // write fwd link to outLinks
            if (backLink.dataNodePtr->nodeLinks-> outLinks.contains(allLinkNames.linkName))
            {
                backLink.dataNodePtr->nodeLinks-> outLinks[allLinkNames.linkName].append(fwdLink);

                // EG_LOG_STUB << "Out links added " << backLink.dataNodePtr->nodeLinks-> outLinks[allLinkNames.linkName].count() << FN;
            }
            else
            {
                newLinks.clear();
                newLinks.append(fwdLink);

                backLink.dataNodePtr->nodeLinks-> outLinks.insert(allLinkNames.linkName, newLinks);

                // EG_LOG_STUB << "Out links created " << backLink.dataNodePtr->nodeLinks-> outLinks[allLinkNames.linkName].count() << FN;
            }

                // write back link to inLinks
            if (fwdLink.dataNodePtr->nodeLinks-> inLinks.contains(allLinkNames.linkName))
            {
                fwdLink.dataNodePtr->nodeLinks-> inLinks[allLinkNames.linkName].append(backLink);
            }
            else
            {
                newLinks.clear();
                newLinks.append(backLink);

                fwdLink.dataNodePtr->nodeLinks-> inLinks.insert(allLinkNames.linkName, newLinks);
            }

            // EG_LOG_STUB << "Link " << allLinkNames.linkName << " added " << firstType-> metaInfo.typeName << " " << fromNode << " to "
            //          << secondType-> metaInfo.typeName << " " <<  toNode << FN;

            // EG_LOG_STUB << "Nodes Type " << firstType-> metaInfo.typeName << " out links count = " << backLink.dataNodePtr->nodeLinks-> outLinks[allLinkNames.linkName].count() << FN;

        }
        else
        {
            // EG_LOG_STUB << "Link " << allLinkNames.linkName << " of " << firstType-> metaInfo.typeName << " link NOT added for ID = " << fromNode << " to "
            //         << secondType-> metaInfo.typeName << " " << toNode << FN;
        }


    }
    return 0;
}

int EgLinkType::LoadLinkedNodesFrom(EgDataNodeIdType fromNodeID)
{

    int res = 0;

    if (! isConnected)
    {
        linksStorage-> ConnectServiceNodeType(*egDatabase, allLinkNames.linkName + egLinkFileNamePostfix); // FIXME server

        isConnected = true;
    }

    linksStorage-> ClearData();

    linksStorage-> IndexOffsets.clear();

    QVariant variantID (fromNodeID);

    linksStorage-> LocalFiles-> indexFiles["from_node_id"]-> Load_EQ(linksStorage->IndexOffsets, variantID);


    if (! linksStorage-> IndexOffsets.isEmpty())
        res = linksStorage-> LocalFiles-> LocalLoadData(linksStorage-> IndexOffsets, linksStorage-> dataNodes);

    // EG_LOG_STUB << allLinkNames.linkName << " Links count: " << linksStorage-> dataNodes.count() << FN;

    // EG_LOG_STUB << allLinkNames.linkName << " Next node ID:" << linksStorage-> metaInfo.nextNodeID << FN;

    // linksStorage-> PrintObjData();

    return res;
}



