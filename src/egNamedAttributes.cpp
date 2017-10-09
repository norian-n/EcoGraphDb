/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egNamedAttributes.h"
#include "egDataNodesType.h"

#include <QtDebug>

EgNamedAttributes::EgNamedAttributes(EgDataNodesType* thePrimaryNodesType):
    primaryNodesType(thePrimaryNodesType)
  , namedAttributesStorage(new EgDataNodesType())
{}


EgNamedAttributes::~EgNamedAttributes()
{
    if (namedAttributesStorage)
        delete namedAttributesStorage;
}

int EgNamedAttributes::AddNamedAttribute(EgDataNodeIDtype nodeID, const QString &name, const QVariant &value)
{
    QList<QVariant> myData;

    myData << nodeID << name << value;

    namedAttributesStorage-> AddDataNode(myData); // AddDataNode(EgDataNode& tmpObj)

    return 0;
}


int EgNamedAttributes::UpdateNamedAttribute(EgDataNodeIDtype nodeID, const QString &name, QVariant &value)
{   
        // find attribute
    GetNamedAttributesIDs(nodeID);

    auto attributeIter = namedAttributesOfNodeIDs.find(name);

    if (attributeIter != namedAttributesOfNodeIDs.end())
    {
        QList<QVariant> myData;

        myData << nodeID << name << value;

        namedAttributesStorage-> UpdateDataNode(myData, nodeID);
        return 0;
    }

    return -1; // not found
}

int EgNamedAttributes::DeleteNamedAttribute(EgDataNodeIDtype nodeID, const QString &name)
{
        // find attribute
    GetNamedAttributesIDs(nodeID);

    auto attributeIter = namedAttributesOfNodeIDs.find(name);

    if (attributeIter != namedAttributesOfNodeIDs.end())
    {
        namedAttributesStorage->DeleteDataNode(nodeID);

        return 0;
    }

    return -1; // not found
}

int EgNamedAttributes::LoadNamedAttributes()
{
    int res = 0;

    namedAttributesStorage-> ClearData();

    namedAttributesStorage-> IndexOffsets.clear();

    // for (QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
    for (auto dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
        // namedAttributesType-> LocalFiles-> primIndexFiles-> Load_EQ(namedAttributesType->IndexOffsets, dataNodeIter.key()); // FIXME wrong index
    {
        QVariant tmpID = dataNodeIter.key();
        namedAttributesStorage-> LocalFiles-> indexFiles["nodeid"]-> Load_EQ(namedAttributesStorage->IndexOffsets, tmpID);
    }


    if (! namedAttributesStorage-> IndexOffsets.isEmpty())
        res = namedAttributesStorage-> LocalFiles-> LocalLoadData(namedAttributesStorage-> IndexOffsets, namedAttributesStorage-> dataNodes);


    return res;
}


/*
    attributesMetaInfo-> AddDataField("nodeid", isIndexed);
    attributesMetaInfo-> AddDataField("key");
    attributesMetaInfo-> AddDataField("value");
*/

int EgNamedAttributes::ResolveNamedAttributes()
{
    attributesById.clear();

        // QMap <EgDataNodeIDtype, EgDataNode>
    for (auto dataNodeIter = namedAttributesStorage-> dataNodes.begin(); dataNodeIter != namedAttributesStorage-> dataNodes.end(); ++dataNodeIter)
        attributesById.insert(dataNodeIter.key(), &(dataNodeIter.value()));

    return 0;
}

int EgNamedAttributes::GetNamedAttributes(EgDataNodeIDtype nodeID)
{
    namedAttributesOfNode.clear();

        // QMultiMap <EgDataNodeIDtype, EgDataNode*>
    auto attributeIter = attributesById.find(nodeID);
    while (attributeIter != attributesById.end() && attributeIter.key() == nodeID)
    {
        namedAttributesOfNode.insert((*(attributeIter.value()))["key"].toString(), (*(attributeIter.value()))["value"]);
        ++attributeIter;
    }

    return 0;
}

int EgNamedAttributes::GetNamedAttributesIDs(EgDataNodeIDtype nodeID)
{
    namedAttributesOfNodeIDs.clear();

        // QMultiMap <EgDataNodeIDtype, EgDataNode*>
    auto attributeIter = attributesById.find(nodeID);
    while (attributeIter != attributesById.end() && attributeIter.key() == nodeID)
    {
        namedAttributesOfNodeIDs.insert((*(attributeIter.value()))["key"].toString(), attributeIter.value()-> dataNodeID);
        ++attributeIter;
    }

    return 0;
}
