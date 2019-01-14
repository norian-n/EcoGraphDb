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

int EgDataNodesLinkType::AddLink (EgDataNodeIDtype leftNodeID, EgDataNodeIDtype rightNodeID)
{
    QList<QVariant> myData;

    myData << leftNodeID << rightNodeID;

    linksStorage-> AddDataNode(myData); // AddDataNode(EgDataNode& tmpObj)

    // qDebug() << linkName << ": link added " << leftNodeID << "to" <<  rightNodeID << FN;

    return 0;
}

int EgDataNodesLinkType::DeleteLink (EgDataNodeIDtype linkNodeID)
{
        // FIXME check

    linksStorage-> DeleteDataNode(linkNodeID);

    return 0;
}

int EgDataNodesLinkType::StoreLinks()
{
    if (! isConnected)
    {
        linksStorage-> Connect(*egDatabase, linkName + egLinkFileNamePostfix);

        isConnected = true;
    }

    // qDebug() << linkName << " Links count: " << linksStorage-> dataNodes.count() << FN;

    linksStorage-> StoreData();

    return 0;
}

int EgDataNodesLinkType::LoadLinks()
{
    if (! isConnected)
    {
        linksStorage-> Connect(*egDatabase, linkName + egLinkFileNamePostfix);

        isConnected = true;
    }

    linksStorage-> LoadAllNodes();

    // qDebug() << linkName << " Links count: " << linksStorage-> dataNodes.count() << FN;

    return 0;
}

int EgDataNodesLinkType::ResolveNodeTypes()
{
    auto iterFirst = egDatabase-> connectedNodeTypes.find(firstTypeName);
    auto iterSecond = egDatabase-> connectedNodeTypes.find(secondTypeName);

    if (iterFirst != egDatabase-> connectedNodeTypes.end() && iterSecond != egDatabase-> connectedNodeTypes.end())
    {
        firstType = iterFirst.value();
        secondType = iterSecond.value();

        return 0;
    }

    return 1;
}

int EgDataNodesLinkType::ResolveLinksToPointers()
{
    EgDataNodeIDtype fromNode, toNode;
    EgExtendedLinkType fwdLink, backLink;

    QList<EgExtendedLinkType> newLinks;

    if (ResolveNodeTypes())
    {
        qDebug() << "EgDataNodesType not found: " << firstTypeName << " or " << secondTypeName << FN;

        return -1;
    }

        // iterate loaded links
    for (auto Iter = linksStorage-> dataNodes.begin(); Iter != linksStorage-> dataNodes.end(); ++Iter)
    {
            // find first node
        fromNode = Iter.value()["from_node_id"].toInt();
        toNode = Iter.value()["to_node_id"].toInt();

        // qDebug() << "From Node ID = " << fromNode << " To Node ID = " << toNode << FN;

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
            if (backLink.dataNodePtr->nodeLinks-> outLinks.contains(linkName))
            {
                backLink.dataNodePtr->nodeLinks-> outLinks[linkName].append(fwdLink);

                // qDebug() << "Out links added " << backLink.dataNodePtr->nodeLinks-> outLinks[linkName].count() << FN;
            }
            else
            {
                newLinks.clear();
                newLinks.append(fwdLink);

                backLink.dataNodePtr->nodeLinks-> outLinks.insert(linkName, newLinks);

                // qDebug() << "Out links created " << backLink.dataNodePtr->nodeLinks-> outLinks[linkName].count() << FN;
            }

                // write back link to inLinks
            if (fwdLink.dataNodePtr->nodeLinks-> inLinks.contains(linkName))
            {
                fwdLink.dataNodePtr->nodeLinks-> inLinks[linkName].append(backLink);
            }
            else
            {
                newLinks.clear();
                newLinks.append(backLink);

                fwdLink.dataNodePtr->nodeLinks-> inLinks.insert(linkName, newLinks);
            }

            // qDebug() << "Link " << linkName << " added " << firstType-> metaInfo.typeName << " " << fromNode << " to "
            //          << secondType-> metaInfo.typeName << " " <<  toNode << FN;

            // qDebug() << "Nodes Type " << firstType-> metaInfo.typeName << " out links count = " << backLink.dataNodePtr->nodeLinks-> outLinks[linkName].count() << FN;

        }
        else
        {
            // qDebug() << "Link " << linkName << " of " << firstType-> metaInfo.typeName << " link NOT added for ID = " << fromNode << " to "
            //         << secondType-> metaInfo.typeName << " " << toNode << FN;
        }


    }
    return 0;
}

int EgDataNodesLinkType::LoadLinkedNodes(EgDataNodeIDtype fromNodeID)
{

    int res = 0;

    if (! isConnected)
    {
        linksStorage-> Connect(*egDatabase, linkName + egLinkFileNamePostfix);

        isConnected = true;
    }

    linksStorage-> ClearData();

    linksStorage-> IndexOffsets.clear();

    QVariant variantID (fromNodeID);

    linksStorage-> LocalFiles-> indexFiles["from_node_id"]-> Load_EQ(linksStorage->IndexOffsets, variantID);


    if (! linksStorage-> IndexOffsets.isEmpty())
        res = linksStorage-> LocalFiles-> LocalLoadData(linksStorage-> IndexOffsets, linksStorage-> dataNodes);


    return res;
}



