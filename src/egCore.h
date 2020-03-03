/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2020 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

// /* Almost all compilers support "pragma once"                                     */
// /* If you got an ugly one someway, uncomment and double-check the define guards manually */

// #ifndef EG_CORE_H
// #define EG_CORE_H

// #define EG_LIB_BUILD // or use it in compiler options for the lib setup

// egCore - basic types and constants

#pragma once

// #include <QString>
// #include <QVariant>

        // debug info definition
#include <QtDebug>
#include "egLogging.h"

// The custom async singleton logger to log file would be better for release version, qDebug used for development stage

// #define EG_LOG_STUB *egGlobalLoggerPtr

#define EG_LOG_STUB qDebug()

#ifdef EG_LOG
    static egAsyncLogger* egGlobalLoggerPtr {nullptr};
    static egLogFlush eglogflush;
        // debug info definition
    #define FN "[" << __FUNCTION__  << ", " << __FILE__ << "]" << eglogflush
    #define FNS QString("[") + __FUNCTION__ + ", " + __FILE__ + "]"
#else
    #define FN "[" << __FUNCTION__  << ", " << __FILE__ << "]"
    #define FNS QString("[") + __FUNCTION__ + ", " + __FILE__ + "]"
#endif

    // bool flags aliases
const bool isIndexed = true;
const bool useLocations = true;

    // ID types
// typedef uint32_t EgDataNodesClassIDType;     // data object class ID type

typedef int32_t  EgDataNodeIdType;      // data object ID type
typedef uint16_t EgFieldIDtype;         // data field ID type
typedef uint32_t EgIndexNodeIDtype;     // index node ID type

// sample: bool FilterTest (EgDataNode& dataNode, QList<QVariant>& filterValues)

    // index-based filter type (fortran-style logical acronims)
enum FilterType
{
    EQ, // ==
    GE, // >=
    LE, // <=
    GT, // >
    LT, // <
    NE  // !=
};

    // indexes tree logical operations
enum LOType
{
    AND,
    OR
};

    // internal data node status (similar to Qt model data_status below)
enum DataStatusType // data status type
{
    isUnchanged,
    isAdded,
    isModified
    // isDeleted
};

    // data node type settings
struct EgNodeTypeSettings
{
    bool useEntryNodes  { false };         // start points for graph operations
    bool useLocation    { false };         // locations info in separate nodes type, hardlinked by ID
    bool useNamedAttributes { false };     // flexible named attributes/properties stored in separate subtype
    bool useLinks       { false };
    bool useGUIsettings { false };

    // EgRemoteConnect*  connection = nullptr;     // connection data (nullptr means local files)
};

    // data node type settings
struct EgIndexSettings
{
    uint16_t fieldNum;       // field # in data node fields list
    uint16_t indexSize;      // bits - 32, 64 etc.
    uint16_t isSigned;       // uniform bool representation
    uint16_t functionID;     // transformation function

};

// #endif // EG_CORE_H
