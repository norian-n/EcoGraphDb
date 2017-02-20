/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef NAMED_ATTRIBUTES_H
#define NAMED_ATTRIBUTES_H

#include "egCore.h"

class EgDataNodesType;

class EgNamedAttributes
{
public:

    // FIXME TODO not implemented some functionality

    EgDataNodesType* primaryNodesType;      // backlink
    EgDataNodesType* namedAttributesStorage;

    EgDataNodeIDtype currentNodeId = 0;                     // last for GetNamedAttributes
    QMultiMap <QString, QVariant>  namedAttributesOfNode;   // temporary for GetNamedAttributes

    QMultiMap <EgDataNodeIDtype, EgDataNode*>  attributesById;

    int AddNamedAttribute(EgDataNodeIDtype nodeID, QString& name, QVariant& value);
    int UpdateNamedAttribute(EgDataNodeIDtype nodeID, QString &name, QVariant &value);
    int DeleteNamedAttribute(EgDataNodeIDtype nodeID);

    int LoadNamedAttributes();

    int ResolveNamedAttributes();                       // TODO fill attributesById

    int GetNamedAttributes(EgDataNodeIDtype nodeID);    // TODO fill namedAttributesOfNode

    EgNamedAttributes() = delete;
    EgNamedAttributes(EgDataNodesType* thePrimaryNodesType);

    ~EgNamedAttributes();

};


#endif // NAMED_ATTRIBUTES_H
