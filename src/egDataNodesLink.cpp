#include "egDataNodesLink.h"
#include "egDataNodesType.h"
#include "egGraphDatabase.h"

/*
EgDataNodesLinkType::EgDataNodesLinkType():
    LinkFileName(left_dc->DClassName+"_"+right_dc->DClassName+".dln")
{
    // constructor
    // qDebug() << "egDataNodesLink(): LinkFileName = " << LinkFileName;
}

*/

int EgDataNodesLinkType::AddLink (EgDataNodeIDtype leftNodeID, EgDataNodeIDtype rightNodeID)
{
    addedLinks.insert(leftNodeID, rightNodeID);

    // qDebug() << linkName << ": link added " << leftNodeID << "to" <<  rightNodeID << FN;

    return 0;
}


int EgDataNodesLinkType::StoreLinks()
{
    QFile dat_file;             // data file
    QDataStream dat;

    if (addedLinks.isEmpty())
        return 1;

    dat_file.setFileName(linkName + ".dln");

    if (!dat_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "can't open file " << linkName + ".dln" << FN;
        return -1;
    }

    dat.setDevice(&dat_file);

    dat.device()-> seek(dat.device()-> size());

        // walk add list
    for (QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>::iterator addIter = addedLinks.begin(); addIter != addedLinks.end(); ++addIter)
    {
        // qDebug() << "Adding link" << (int) addIter.key() << (int) addIter.value() << " on offset" << hex << (int) dat.device()-> pos() << FN;

        // primIndexFiles.data_offset = dat.device()-> pos();

        dat << addIter.key();
        dat << addIter.value();

            // add primary index
        // primIndexFiles.the_index = addIter.key();
        // primIndexFiles.AddObjToIndex();
    }

    addedLinks.clear();

    dat_file.close();

    // primIndexFiles.CloseIndexFiles();

    return 0;
}


int EgDataNodesLinkType::LoadLinks()
{
     QFile dat_file;             // data file
     QDataStream dat;

     EgDataNodeIDtype leftNodeID;
     EgDataNodeIDtype rightNodeID;

     loadedLinks.clear();

     dat_file.setFileName(linkName + ".dln");

     if (!dat_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
     {
         // qDebug() << "can't open file " << linkName + ".dln" << FN;
         return 1;
     }

     dat.setDevice(&dat_file);

     while(! dat.atEnd())
     {
         dat >> leftNodeID;
         dat >> rightNodeID;

         loadedLinks.insert(leftNodeID, rightNodeID);
     }

     dat_file.close();

     ResolveLinks(); // FIXME STUB

     return 0;
}

int EgDataNodesLinkType::ResolveLinks()
{
    EgDataNodesType* firstType, * secondType;
    EgDataNodeIDtype fromNode, toNode;
    EgExtendedLinkType fwdLink, backLink;
    QList<EgExtendedLinkType> newLinks;

        // find node types to resolve
    if (! egDatabase-> connNodeTypes.contains(firstTypeName))
    {
        qDebug() << "NodeType not found: " << firstTypeName << FN;

        return -1;
    }

    firstType = egDatabase-> connNodeTypes[firstTypeName];

    if (! egDatabase-> connNodeTypes.contains(secondTypeName))
    {
        qDebug() << "NodeType not found: " << secondTypeName << FN;

        return -1;
    }

    secondType = egDatabase-> connNodeTypes[secondTypeName];

        // iterate loaded links
    for (QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>::iterator Iter = loadedLinks.begin(); Iter != loadedLinks.end(); ++Iter)
    {
        // find first node

        fromNode = Iter.key();
        toNode = Iter.value();

        /*if (firstType-> dataNodes.contains(Iter.key()))
            qDebug() << "Node ID found: " << (int) Iter.key() << FN;

        // find second node
        if (secondType-> dataNodes.contains(Iter.value()))
            qDebug() << "Node ID found: " << (int) Iter.value() << FN;*/

                    // firstType-> dataNodes[Iter.key()]-> nodeLinks.outLinks.insert(linkName, newLinks);

        if (firstType-> dataNodes.contains(fromNode) && secondType-> dataNodes.contains(toNode))
        {
                // fill new links info
            fwdLink.dataNodeID = toNode;
            fwdLink.dataNodePtr = &(secondType-> dataNodes[toNode]);

            backLink.dataNodeID = fromNode;
            backLink.dataNodePtr = &(firstType-> dataNodes[fromNode]);

                // check/create links
            if (! firstType-> dataNodes[fromNode].nodeLinks)
              firstType-> dataNodes[fromNode].nodeLinks = new EgDataNodeLinks();

            if (! secondType-> dataNodes[toNode].nodeLinks)
              secondType-> dataNodes[toNode].nodeLinks = new EgDataNodeLinks();

                // write fwd link
            if (! firstType-> dataNodes[fromNode].nodeLinks-> outLinks.contains(linkName))
            {
                newLinks.clear();
                newLinks.append(fwdLink);

                firstType-> dataNodes[fromNode].nodeLinks-> outLinks.insert(linkName, newLinks);
            }
            else
                firstType-> dataNodes[fromNode].nodeLinks-> outLinks[linkName].append(fwdLink);

                // write back link
            if (! secondType-> dataNodes[toNode].nodeLinks-> outLinks.contains(linkName))
            {
                newLinks.clear();
                newLinks.append(backLink);

                secondType-> dataNodes[toNode].nodeLinks-> outLinks.insert(linkName, newLinks);
            }
            else
                secondType-> dataNodes[toNode].nodeLinks-> outLinks[linkName].append(backLink);

            // qDebug() << firstType->  metaInfo.typeName << "link added " << fromNode << "to" <<  toNode << FN;
        }
        else
        {
            qDebug() << firstType-> metaInfo.typeName << " : not found data node for ID = " << fromNode << " or " << secondType-> metaInfo.typeName << " " <<  toNode << FN;
        }


    }
    return 0;
}
