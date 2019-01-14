/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_DATA_NODES_GUI_CONNECT_H
#define EG_DATA_NODES_GUI_CONNECT_H

#include <QStandardItemModel>
#include <QTableView>
#include <QTreeView>
#include <QComboBox>

#include "egDataNode.h"

class EgDataNodesType;

struct TreeBuildNodeType
{
    EgDataNode* dataNode;
    QStandardItem* modelItem;

    TreeBuildNodeType(): dataNode(NULL), modelItem(NULL) {}
};

class EgBasicControlDesc
{
public:
    QString             controlLabel;           // GUI label
    int                 controlDefWidth;        // GUI control default width
    EgFieldIDtype       fieldIndex;             // index of corresponding field at

    EgDataNodesType*    AutoSubstClass = nullptr;         // auto/substitute link class
    EgFieldIDtype       AutoSubstFieldIndex;    // auto/substitute link index

    EgBasicControlDesc() {}

    EgBasicControlDesc(EgDataNode &dataNode);
    ~EgBasicControlDesc() {}
};


#ifdef EG_LIB_BUILD   // library build switch, define it in project or egCore.h

#include "ecographdb_global.h"

class ECOGRAPHDBSHARED_EXPORT EgDataNodesGUIsupport

#else   // no library build

class EgDataNodesGUIsupport

#endif
              // Data Nodes GUI functionality
{
public:

  EgDataNodesType* primaryNodesType = nullptr;  // peer nodes type
  EgDataNodesType* controlDescs = nullptr;      // simple control descriptors storage

  QStandardItem* model_current_item = nullptr;  // model item ptr
  qint32 model_current_row = 0;                 // current row of model savepoint

  QList <EgBasicControlDesc> basicControlDescs;
  QMap <QString, EgBasicControlDesc*> basicControlDescsOrder;

  EgDataNodesGUIsupport() = delete;             // peer type required

  EgDataNodesGUIsupport(EgDataNodesType *thePrimaryNodesType);
  ~EgDataNodesGUIsupport();

  // bool CheckLocalGUIFile();
  // int CreateDataNodesForControlDescs();

  int AddSimpleControlDesc(QString fieldName, QString fieldLabel, int fieldWidth);   // add default control to field descriptor
  // int DeleteControlDesc(QString fieldName);  // FIXME TODO
  // int UpdateControlDesc(QString fieldName, QString fieldLabel, int fieldWidth);

  int LoadSimpleControlDesc();

    // data model transfers
  void SetModelHeaders(QStandardItemModel* model);  // set table headers names as in the control descriptors (DControlDesc)
  void SetViewWidths(QTableView* tableView);        // adjust column sizes
  void SetViewWidths(QTreeView* treeView);

  int DataFromModel(QStandardItemModel* model);     // get data from model
  void DataToModel(QStandardItemModel* model);       // pump data to model

  inline QStandardItem *AddNodeToModelTree(QStandardItem* parentItem, EgDataNode* dataNode);
  int DataToModelTree(QStandardItemModel* model, QString linkName);   // pump to model with tree support

    // mark data in model
  int AddRowOfModel(QStandardItemModel* model, QList<QStandardItem *> & items); // add new row
  int DeleteRowOfModel(QStandardItemModel* model);                              // mark row as deleted
  int ModifyRowOfModel(QStandardItemModel* model);

    // get selected ID
  EgDataNodeIDtype GetIDByModel(QStandardItemModel* model); // get selected object's ID

    // combo box operations
  void FillComboBox(QComboBox* my_box); // move all data to combo box
  void SetComboBox(QComboBox* my_box, QVariant dobj_id);    // set box value by object ID
  QVariant GetComboBoxID(QComboBox* my_box);    // get selected object ID of the box

    // substitute operations
  int AddAutoSubstitute(const char* my_field, EgDataNodesType& ref_class, const char* ref_field);

};


#endif // EG_DATA_NODES_GUI_CONNECT_H
