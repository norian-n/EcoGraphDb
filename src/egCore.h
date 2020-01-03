/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_CORE_H
#define EG_CORE_H

// #define EG_LIB_BUILD

// egCore - basic types and constants

#include <QtDebug>

#include <QString>
#include <QVariant>

#include "egLogging.h"

        // debug info definition
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

class EgDataNode;

    // custom filter function type (remote filter could be set via Qt plugins)
typedef bool (*FilterFunctionType) (EgDataNode& dataNode, QList<QVariant>& filterValues);

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

    // data node add-on for uncommon data
struct NamedAttribute
{
    QString  name;
    QVariant value;
};

    // Qt form data node operations
enum formMode
{
    formModeAdd,
    formModeEdit,
    formModeDelete
};

    // Qt data model related constants - service fields location
const int data_status = Qt::UserRole + 1;   // DataStatusType equivalent
const int data_id     = Qt::UserRole + 2;   // egDb data node ID


#endif // EG_CORE_H
