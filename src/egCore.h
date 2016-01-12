/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2014 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef EG_CORE_H
#define EG_CORE_H

// egCore - basic types and constants

#include <QtDebug>
#include <QString>

    // debug info definition
#define FN " [" << __FUNCTION__  << ", " << __FILE__ << "] "
#define FNS " [" + QString(__FUNCTION__) + ", " + QString(__FILE__) + "] "
#define LOCAL NULL          // alias : use local files, not server link

    // local files : index size
const quint64 IndexRecSize2 = 4 + 8; // bytes : ID (4) + pos (8)

    // indexed flag alias
const bool IsIndexed = true;
const qint16 primIndex = -1; // primary index alias for indexes proc

    // ID and other types
typedef quint32 EgDataNodesTypeID;    // data object class ID type
typedef qint32  EgDataNodeIDtype;     // data object ID type !!! signed to mark deleted !!!
typedef quint16 EgFieldIDtype;      // data field ID type

class EgDataNode;

// filter callback type (remote filter could be set via Qt plugins)
typedef int (*FilterCallbackType) (QList<QVariant>& obj_fields_values, QList<QVariant>& class_filter_values, QHash<QString, int>& obj_field_indexes);
// sample: int FilterTest (QList<QVariant>& obj_fields_values, QList<QVariant>& class_filter_values)

// filter callback type (remote filter could be set via Qt plugins)
typedef int (*FilterCallbackType2) (EgDataNode& data_object, QList<QVariant>& filter_values);
// sample: int FilterTest (DataObj& data_object, QList<QVariant>& filter_values)

enum DataFieldType  // supported Data Field types
{
    d_var,      // QVariant
    d_int2,      // int
    d_float,    // float (32 bit)
    d_date,     // converted to qint32
    d_string,   // QString
    d_link,     // reference OBJ_ID
    d_double,   // double (64 bit)
    d_time,     // converted to qint32
    d_datetime, // converted to qint64 as superposition of d_date and d_time
    d_attribute // named atttribute - struct NamedAttribute below
};
    // fixed field sizes and names directly related to DataFieldType (!!!) (0-variable size) CHECKME obsolete ?
// const rec_size_type d_type_sizes[9] = {0,4,4,4,0,sizeof(obj_id_type),8,4,8};
const char* const dtype_names[9] = {"d_var", "d_int32", "d_float", "d_date", "d_string", "d_link", "d_double", "d_time", "d_datetime"};

    // index-based filter type

enum FilterType // filter type
{
    EQ, // ==
    GE, // >=
    LE, // <=
    GT, // >
    LT, // <
    NE  // !=
};

enum LOType // logical operation type
{
    AND,
    OR
};

    // internal record status (similar to model data_status below)
enum DataStatusType // data status type
{
    is_unchanged,
    is_added,
    is_modified,
    is_deleted
};

    // Qt data model related constants - service fields location

const int data_status = Qt::UserRole + 1;   // data_status_type
const int data_id     = Qt::UserRole + 2;   // ObjDb OBJ_ID

    // forward decl
// class DFieldDescriptors;
// class DataObjects;


struct NamedAttribute
{
    QString  name;
    QVariant value;
};







// =============================================================================================
//                              JUNKYARD
// =============================================================================================

// const char* const stype_names[4] = {"is_unchanged", "is_added", "is_modified", "is_deleted"};

/*
enum CtrlType    // supported Qt controls
{
    ct_none,
    ct_line_edit,   // QLineEdit
    ct_date_edit,   // QDateEdit
    ct_combo_box,   // QComboBox
    ct_model        // QStandardItemModel : QTableView, QTreeView
};
*/

// const char* const ctrltype_names[6] = {"ct_none","ct_line_edit","ct_date_edit","ct_combo_box","ct_table_view","ct_tree_view"};

/*
const data_status_type is_unchanged  = 0;
const data_status_type is_added      = 1;
const data_status_type is_modified   = 2;
const data_status_type is_deleted    = 4;
*/

#endif // EG_CORE_H
