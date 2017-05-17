/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EG_CORE_H
#define EG_CORE_H

// #define EG_LIB_BUILD

// egCore - basic types and constants

#include <QtDebug>
#include <QString>

    // debug info definition
#define FN "[" << __FUNCTION__  << ", " << __FILE__ << "]"
#define FNS QString("[") + __FUNCTION__ + ", " + __FILE__ + "]"
#define LOCAL NULL          // alias : use local files, not server link

    // flags aliases
const bool isIndexed = true;
const bool useLocations = true;

    // ID and other types
typedef quint32 EgDataNodesTypeID;    // data object class ID type
typedef qint32  EgDataNodeIDtype;     // data object ID type !!! signed to mark deleted !!!
typedef quint16 EgFieldIDtype;      // data field ID type

class EgDataNode;

// filter callback type (remote filter could be set via Qt plugins)
typedef bool (*FilterFunctionType) (EgDataNode& data_object, QList<QVariant>& filter_values);
// sample: int FilterTest (DataObj& data_object, QList<QVariant>& filter_values)

    // index-based filter type

enum FilterType     // fortran-style indexes logical abbreviations
{
    EQ, // ==
    GE, // >=
    LE, // <=
    GT, // >
    LT, // <
    NE  // !=
};

enum LOType         // indexes logical operations
{
    AND,
    OR
};

    // internal data node status (similar to Qt model data_status below)
enum DataStatusType // data status type
{
    is_unchanged,
    is_added,
    is_modified,
    is_deleted
};

enum formMode   // Qt form for data node operations mode
{
    formModeAdd,
    formModeEdit,
    formModeDelete
};

    // Qt data model related constants - service fields location

const int data_status = Qt::UserRole + 1;   // DataStatusType equivalent
const int data_id     = Qt::UserRole + 2;   // egDb data node ID

struct NamedAttribute   // data node add-on
{
    QString  name;
    QVariant value;
};

struct EgRemoteConnect    // server connection info
{
    QString server_address;
};


struct EgNodeTypeSettings
{
    bool useEntryNodes = false;         // start points for graph operations
    bool useLocation = false;           // locations info in separate nodes type, hardlinked by ID
    bool useNamedAttributes = false;    // named attributes/properties in separate nodes type
    bool useLinks = false;
    bool useGUIsettings = false;

    EgRemoteConnect*  connection = nullptr;     // connection data (nullptr means local files)
};


#endif // EG_CORE_H
