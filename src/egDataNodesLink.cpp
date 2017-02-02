/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
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

    linksStorage-> LoadAllData();

    return 0;
}

int EgDataNodesLinkType::ResolveLinks(EgDataNodesType& firstType, EgDataNodesType& secondType)
{
    EgDataNodeIDtype fromNode, toNode;
    EgExtendedLinkType fwdLink, backLink;
    QList<EgExtendedLinkType> newLinks;

        // iterate loaded links
    for (auto Iter = linksStorage-> dataNodes.begin(); Iter != linksStorage-> dataNodes.end(); ++Iter)
    {
        // find first node

        fromNode = Iter.value()["from_node_id"].toInt();
        toNode = Iter.value()["to_node_id"].toInt();

        // qDebug() << "From Node ID = " << fromNode << " To Node ID = " << toNode << FN;

        if (firstType.dataNodes.contains(fromNode) && secondType.dataNodes.contains(toNode))
        {
            // fill new links info
            fwdLink.dataNodeID = toNode;
            fwdLink.dataNodePtr = &(secondType.dataNodes[toNode]);

            backLink.dataNodeID = fromNode;
            backLink.dataNodePtr = &(firstType.dataNodes[fromNode]);

            // check/create links
            if (! firstType.dataNodes[fromNode].nodeLinks)
                firstType.dataNodes[fromNode].nodeLinks = new EgDataNodeLinks();

            if (! secondType.dataNodes[toNode].nodeLinks)
                secondType.dataNodes[toNode].nodeLinks = new EgDataNodeLinks();

            // write fwd link to outLinks
            if (! firstType.dataNodes[fromNode].nodeLinks-> outLinks.contains(linkName))
                firstType.dataNodes[fromNode].nodeLinks-> outLinks[linkName].append(fwdLink);
            else
            {
                newLinks.clear();
                newLinks.append(fwdLink);

                firstType.dataNodes[fromNode].nodeLinks-> outLinks.insert(linkName, newLinks);
            }

            // write back link to inLinks
            if (secondType.dataNodes[toNode].nodeLinks-> inLinks.contains(linkName))
                secondType.dataNodes[toNode].nodeLinks-> inLinks[linkName].append(backLink);
            else
            {
                newLinks.clear();
                newLinks.append(backLink);

                secondType.dataNodes[toNode].nodeLinks-> inLinks.insert(linkName, newLinks);
            }

            // qDebug() << "Link " << linkName << " added " << firstType.metaInfo.typeName << " " << fromNode << " to "
            //         << secondType.metaInfo.typeName << " " <<  toNode << FN;
        }
        else
        {
            // qDebug() << "Link " << linkName << " of " << firstType.metaInfo.typeName << " link NOT added for ID = " << fromNode << " to "
            //         << secondType.metaInfo.typeName << " " << toNode << FN;
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

    linksStorage-> LocalFiles-> indexFiles["from_node_id"]-> Load_EQ(linksStorage->IndexOffsets, fromNodeID);


    if (! linksStorage-> IndexOffsets.isEmpty())
        res = linksStorage-> LocalFiles-> LocalLoadData(linksStorage-> IndexOffsets, linksStorage-> dataNodes);


    return res;
}



