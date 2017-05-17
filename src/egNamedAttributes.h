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
    QMap <QString, QVariant>  namedAttributesOfNode;   // temporary for GetNamedAttributes

    QMultiMap <EgDataNodeIDtype, EgDataNode*>  attributesById;

    int AddNamedAttribute(EgDataNodeIDtype nodeID, const QString &name, const QVariant &value);
    int UpdateNamedAttribute(EgDataNodeIDtype nodeID, const QString &name, QVariant &value);
    int DeleteNamedAttribute(EgDataNodeIDtype nodeID, const QString &name);

    int LoadNamedAttributes();

    int ResolveNamedAttributes();                       // fill attributesById

    int GetNamedAttributes(EgDataNodeIDtype nodeID);    // fill namedAttributesOfNode

    EgNamedAttributes() = delete;
    EgNamedAttributes(EgDataNodesType* thePrimaryNodesType);

    ~EgNamedAttributes();

};


#endif // NAMED_ATTRIBUTES_H
