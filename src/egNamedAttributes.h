/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef NAMED_ATTRIBUTES_H
#define NAMED_ATTRIBUTES_H

#include "egCore.h"

class EgDataNodesType;

class EgNamedAttributes
{
public:

    // FIXME TODO check if all functionality implemented, write coverage tests

    EgDataNodesType* primaryNodesType;                  // backlink to my type
    EgDataNodesType* namedAttributesStorage;            // data

    // EgDataNodeIDtype currentNodeId = 0;              // last for GetNamedAttributes
    QMap <QString, QVariant>  namedAttributesOfNode;    // temporary for GetNamedAttributes
    QMap <QString, EgDataNodeIDtype>  namedAttributesOfNodeIDs;   // IDs in the namedAttributesStorage

    QMultiMap <EgDataNodeIDtype, EgDataNode*>  attributesById;

    int AddNamedAttribute(EgDataNodeIDtype nodeID, const QString &name, const QVariant &value);
    int UpdateNamedAttribute(EgDataNodeIDtype nodeID, const QString &name, QVariant &value);
    int DeleteNamedAttribute(EgDataNodeIDtype nodeID, const QString &name);

    int LoadNamedAttributes();

    int ResolveNamedAttributes();                       // fill attributesById

    int GetNamedAttributes(EgDataNodeIDtype nodeID);    // fill namedAttributesOfNode
    int GetNamedAttributesIDs(EgDataNodeIDtype nodeID);

    EgNamedAttributes() = delete;
    EgNamedAttributes(EgDataNodesType* thePrimaryNodesType);

    ~EgNamedAttributes();

};


#endif // NAMED_ATTRIBUTES_H
