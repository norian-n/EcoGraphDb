/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
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
    QList<QVariant> myData;

    myData << nodeID << name << value;

        // FIXME find attribute

    namedAttributesStorage-> UpdateDataNode(myData, nodeID);

    return 0;
}

int EgNamedAttributes::DeleteNamedAttribute(EgDataNodeIDtype nodeID, const QString &name)
{
    // FIXME find attribute

    // namedAttributesStorage->DeleteDataNode(nodeID);

    return 0;
}

int EgNamedAttributes::LoadNamedAttributes()
{
    int res = 0;

    namedAttributesStorage-> ClearData();

    namedAttributesStorage-> IndexOffsets.clear();

    // for (QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
    for (auto dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
        // namedAttributesType-> LocalFiles-> primIndexFiles-> Load_EQ(namedAttributesType->IndexOffsets, dataNodeIter.key()); // FIXME wrong index

        namedAttributesStorage-> LocalFiles-> indexFiles["nodeid"]-> Load_EQ(namedAttributesStorage->IndexOffsets, dataNodeIter.key());


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
