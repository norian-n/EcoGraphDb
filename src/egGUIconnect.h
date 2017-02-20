/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
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

    EgDataNodesType*    AutoSubstClass;         // auto/substitute link class
    EgFieldIDtype       AutoSubstFieldIndex;    // auto/substitute link index

    EgBasicControlDesc(): AutoSubstClass(NULL) {}

    EgBasicControlDesc(EgDataNode &dataNode);
    ~EgBasicControlDesc() {}
};

class EgDataNodesGUIsupport   // Data Nodes GUI functionality
{
public:

  EgDataNodesType* dataNodesType = nullptr;   // external

  EgDataNodesType* controlDescs = nullptr;    // my, new-delete

  QStandardItem* model_current_item = nullptr;
  qint32 model_current_row = 0;         // current row of model savepoint

  QList <EgBasicControlDesc> basicControlDescs;
  QHash <QString, EgBasicControlDesc*> basicControlDescsOrder;

  EgDataNodesGUIsupport();
  ~EgDataNodesGUIsupport();

  // void Init();


  bool CheckLocalGUIFile();
  int CreateDataNodesForControlDescs();

  int AddSimpleControlDesc(QString fieldName, QString fieldLabel, int fieldWidth);   // add default control to field descriptor
  // int DeleteControlDesc(QString fieldName, QString fieldLabel, int fieldWidth);
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
