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

EgDataNodesLinkType::EgDataNodesLinkType():
  linksStorage(new EgDataNodesType())
{

}

EgDataNodesLinkType::EgDataNodesLinkType(EgGraphDatabase *theDatabase):
    egDatabase(theDatabase)
  , linksStorage(new EgDataNodesType())
{

}



EgDataNodesLinkType::~EgDataNodesLinkType()
{
    if (linksStorage)
        delete linksStorage;
}

int EgDataNodesLinkType::AddLink (EgDataNodeIdType fromNodeID, EgDataNodeIdType toNodeID)
{

    QList<QVariant> myData;

    myData << fromNodeID << toNodeID;

    linksStorage-> AddDataNode(myData);

    /*
    EgExtendedLinkType fwdLink, backLink;
    QList<EgExtendedLinkType> newLinks;

        // check if types connected
    if (!firstType || !secondType)
    {
        qDebug() << linkName << " : data type(s) not connected to link type " << FN;

        return -1;
    }


    if (firstType-> dataNodes.contains(fromNodeID) && secondType-> dataNodes.contains(toNodeID))
    {
            // fill new links info
        fwdLink.dataNodeID = toNodeID;
        fwdLink.dataNodePtr = &(secondType-> dataNodes[toNodeID]);

        backLink.dataNodeID = fromNodeID;
        backLink.dataNodePtr = &(firstType-> dataNodes[fromNodeID]);

            // check/create links
        if (! firstType-> dataNodes[fromNodeID].nodeLinks)
          firstType-> dataNodes[fromNodeID].nodeLinks = new EgDataNodeLinks();

        if (! secondType-> dataNodes[toNodeID].nodeLinks)
          secondType-> dataNodes[toNodeID].nodeLinks = new EgDataNodeLinks();

            // write fwd link
        if (firstType-> dataNodes[fromNodeID].nodeLinks-> outLinks.contains(linkName))
            firstType-> dataNodes[fromNodeID].nodeLinks-> outLinks[linkName].append(fwdLink);
        else
        {
            newLinks.clear();
            newLinks.append(fwdLink);

            firstType-> dataNodes[fromNodeID].nodeLinks-> outLinks.insert(linkName, newLinks);
        }

            // FIXME RUNTIME SEGFAULT write back link
        if (secondType-> dataNodes[toNodeID].nodeLinks-> outLinks.contains(linkName))
            secondType-> dataNodes[toNodeID].nodeLinks-> outLinks[linkName].append(backLink);
        else
        {
            newLinks.clear();
            newLinks.append(backLink);

            secondType-> dataNodes[toNodeID].nodeLinks-> outLinks.insert(linkName, newLinks);
        }
    }
    else
    {
        qDebug() << "Link " << linkName << " of " << firstType->extraInfo.typeName << " link NOT added in memory for ID = " << fromNodeID << " to "
                 << secondType->extraInfo.typeName << " " << toNodeID << FN;

        // return -1;
    }
    */

    // qDebug() << linkName << ": link added " << fromNodeID << "to" <<  toNodeID << FN;

    // qDebug() << allLinkNames.linkName << " Next link node ID " << linksStorage->metaInfo.nextNodeID << FN;

    return 0;
}

int EgDataNodesLinkType::Connect(EgGraphDatabase& myDB, const QString& linkTypeName)
{
        // check if already connected FIXME implement reconnect
    if (isConnected)
    {
        // qDebug()  << "Warning: attempt to connect again data link type: " << linkTypeName << FN;

        return 1;
    }
/*
        // get nodes types from database
    if (! myDB.getNodesTypesOfLink(linkTypeName))
    {
        // qDebug()  << "Error: can't find data nodes type of link type: " << linkTypeName << FN;

        return -1;
    }
*/

    allLinkNames.linkName = linkTypeName;

        // connect links storage
    int res = linksStorage-> ConnectServiceNodeType(myDB,
                                     QString(linkTypeName + EgDataNodesLinkNamespace::egLinkFileNamePostfix));

    if (! res)
      isConnected = true;

    return res;
}

int EgDataNodesLinkType::DeleteLink (EgDataNodeIdType linkNodeID)
{
        // FIXME check

    linksStorage-> DeleteDataNode(linkNodeID);

    return 0;
}

int EgDataNodesLinkType::StoreLinks()
{
    if (! isConnected)
    {
        linksStorage-> ConnectServiceNodeType(*egDatabase, allLinkNames.linkName + egLinkFileNamePostfix);

        isConnected = true;
    }

    // qDebug() << allLinkNames.linkName << " Added links count: " << linksStorage->addedDataNodes.count() << FN;

    linksStorage-> StoreData();

    return 0;
}

int EgDataNodesLinkType::LoadLinks()
{
    if (! isConnected)
    {
        linksStorage-> ConnectServiceNodeType(*egDatabase, allLinkNames.linkName + egLinkFileNamePostfix);

        isConnected = true;
    }

    linksStorage-> LoadAllDataNodes();

    // qDebug() << allLinkNames.linkName << " Links count: " << linksStorage-> dataNodes.count() << FN;

    // linksStorage-> PrintObjData();

    return 0;
}

int EgDataNodesLinkType::ResolveNodeTypes()
{
    auto iterFirst = egDatabase-> attachedNodeTypes.find(allLinkNames.firstTypeName);
    auto iterSecond = egDatabase-> attachedNodeTypes.find(allLinkNames.secondTypeName);

    if (iterFirst != egDatabase-> attachedNodeTypes.end() && iterSecond != egDatabase-> attachedNodeTypes.end())
    {
        firstType = iterFirst.value();
        secondType = iterSecond.value();

        return 0;
    }

    return 1;
}

int EgDataNodesLinkType::ResolveLinksToPointers()
{
    EgDataNodeIdType fromNode, toNode;
    EgExtendedLinkType fwdLink, backLink;

    QList<EgExtendedLinkType> newLinks;

    if (ResolveNodeTypes())
    {
        qDebug() << "EgDataNodesType not found: " << allLinkNames.firstTypeName << " or " << allLinkNames.secondTypeName << FN;

        return -1;
    }

        // iterate loaded links
    for (auto Iter = linksStorage-> dataNodes.begin(); Iter != linksStorage-> dataNodes.end(); ++Iter)
    {
            // find first node
        fromNode = Iter.value()["from_node_id"].toInt();
        toNode = Iter.value()["to_node_id"].toInt();

        // qDebug() << "LinkName: " << allLinkNames.linkName << " From Node ID = " << fromNode << " To Node ID = " << toNode << FN;

        if (firstType-> dataNodes.contains(fromNode) && secondType-> dataNodes.contains(toNode))
        {
                // fill new links info
            fwdLink.dataNodeID = toNode;
            fwdLink.dataNodePtr = &(secondType-> dataNodes[toNode]);

            if (! fwdLink.dataNodePtr-> nodeLinks)
                fwdLink.dataNodePtr-> nodeLinks = new EgDataNodeLinks();

            backLink.dataNodeID = fromNode;
            backLink.dataNodePtr = &(firstType-> dataNodes[fromNode]);

            if (! backLink.dataNodePtr-> nodeLinks)
                backLink.dataNodePtr-> nodeLinks = new EgDataNodeLinks();

                // write fwd link to outLinks
            if (backLink.dataNodePtr->nodeLinks-> outLinks.contains(allLinkNames.linkName))
            {
                backLink.dataNodePtr->nodeLinks-> outLinks[allLinkNames.linkName].append(fwdLink);

                // qDebug() << "Out links added " << backLink.dataNodePtr->nodeLinks-> outLinks[allLinkNames.linkName].count() << FN;
            }
            else
            {
                newLinks.clear();
                newLinks.append(fwdLink);

                backLink.dataNodePtr->nodeLinks-> outLinks.insert(allLinkNames.linkName, newLinks);

                // qDebug() << "Out links created " << backLink.dataNodePtr->nodeLinks-> outLinks[allLinkNames.linkName].count() << FN;
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

            // qDebug() << "Link " << allLinkNames.linkName << " added " << firstType-> metaInfo.typeName << " " << fromNode << " to "
            //          << secondType-> metaInfo.typeName << " " <<  toNode << FN;

            // qDebug() << "Nodes Type " << firstType-> metaInfo.typeName << " out links count = " << backLink.dataNodePtr->nodeLinks-> outLinks[allLinkNames.linkName].count() << FN;

        }
        else
        {
            // qDebug() << "Link " << allLinkNames.linkName << " of " << firstType-> metaInfo.typeName << " link NOT added for ID = " << fromNode << " to "
            //         << secondType-> metaInfo.typeName << " " << toNode << FN;
        }


    }
    return 0;
}

int EgDataNodesLinkType::LoadLinkedNodes(EgDataNodeIdType fromNodeID)
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

    // qDebug() << allLinkNames.linkName << " Links count: " << linksStorage-> dataNodes.count() << FN;

    // qDebug() << allLinkNames.linkName << " Next node ID:" << linksStorage-> metaInfo.nextNodeID << FN;

    // linksStorage-> PrintObjData();

    return res;
}



