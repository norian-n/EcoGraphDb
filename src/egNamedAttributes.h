/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2020 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

// #ifndef NAMED_ATTRIBUTES_H
// #define NAMED_ATTRIBUTES_H

#pragma once

#include "egDataNode.h"

class EgDataNodesType;

class EgNamedAttributes
{
public:

    // FIXME TODO check if all functionality implemented, write coverage tests

    EgDataNodesType* primaryNodesType;                  // backlink to my master type
    EgDataNodesType* namedAttributesStorage;            // data pile

    // EgDataNodeIDtype currentNodeId = 0;              // last for GetNamedAttributes
    QMap <QString, QVariant>  namedAttributesOfNode;    // temporary for GetNamedAttributes
    QMap <QString, EgDataNodeIdType>  namedAttributesOfNodeIDs;   // IDs in the namedAttributesStorage

    QMultiMap <EgDataNodeIdType, EgDataNode*>  attributesById;

    int AddNamedAttribute(EgDataNodeIdType nodeID, const QString &name, const QVariant &value);
    int UpdateNamedAttribute(EgDataNodeIdType nodeID, const QString &name, QVariant &value);
    int DeleteNamedAttribute(EgDataNodeIdType nodeID, const QString &name);

    int LoadNamedAttributes();

    int ResolveNamedAttributes();                       // fill attributesById

    int GetNamedAttributes(EgDataNodeIdType nodeID);    // fill namedAttributesOfNode
    int GetNamedAttributesIDs(EgDataNodeIdType nodeID);

    EgNamedAttributes() = delete;
    EgNamedAttributes(EgDataNodesType* thePrimaryNodesType);

    ~EgNamedAttributes();

};


// #endif // NAMED_ATTRIBUTES_H
