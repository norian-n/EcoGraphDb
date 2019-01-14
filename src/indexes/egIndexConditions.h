/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#ifndef EG_INDEX_CONDITIONS_H
#define EG_INDEX_CONDITIONS_H

#include <QString>
#include <QSet>
#include <QVariant>

#include "egIndexesFiles.h"
#include "../egLocalFiles.h"

namespace egIndexConditionsNamespace
{

const bool RIGHT = false;
const bool LEFT = true;

const bool COMB = false;
const bool LEAF = true;

const char* const strEQ("==");
const char* const strGE(">=");
const char* const strLE("<=");
const char* const strGT(">");
const char* const strLT("<");
const char* const strNE("!=");

} // namespace

struct EgIndexNode // real node in the indexes tree
{
    bool is_leaf = false;    // simple condition (true) or logical combination (false)

    EgIndexNodeIDtype nodeID = 0;
    uint32_t subnodesCount = 0;

    QSet<quint64> my_set;

    QString FieldName;
    QVariant value;
    int16_t oper;       // logical operation type - enum LOType in egCore.h or enum FilterType if it is a leaf

    EgIndexNode* left = nullptr;
    EgIndexNode* right = nullptr;
    EgIndexNode* parent = nullptr;

    EgIndexNodeIDtype leftID = 0;
    EgIndexNodeIDtype rightID = 0;
    EgIndexNodeIDtype parentID = 0;

    // EgIndexNode(): is_leaf(false), left(NULL), right(NULL), parent(NULL) {}
};

QDataStream& operator << (QDataStream& d_stream, EgIndexNode& indexNode);    // transfer and file operations
QDataStream& operator >> (QDataStream& d_stream, EgIndexNode& indexNode);    // transfer and file operations

class EgIndexConditionsTree // conditional nodes tree operations
{
public:

    // EgIndexNode* root = nullptr;

    int CalcTreeSet(EgIndexNode *root, QSet<quint64>& final_set, EgDataFiles *LocalFiles);      // calc final set of tree
    void RecursiveCalc(EgIndexNode* branch, EgDataFiles *LocalFiles);

    // void clear(EgIndexNode *rootNode);      // clear tree
    void RecursiveClear(EgIndexNode* branch);

    int TransferTreeSet(EgIndexNode* rootNode, QDataStream& d_stream);
    void RecursiveTransfer(EgIndexNode* branch, QDataStream& d_stream);

    // EgIndexConditionsTree(): root(NULL) {}
    // ~EgIndexConditionsTree() { if (root) RecursiveClear(root);}

    // void Init(EgIndexFiles<qint32>* indexes_ptr);        // attach to indexes to get leaf sets FIXME check type

    // void clearSets(EgIndexNode *rootNode);      // clear sets in nodes
    void RecursiveClearSets(EgIndexNode* branch);

    // void AddNode(EgIndexNode* parent, bool is_leaf, bool is_left, int oper, QString FieldName, QVariant value);
    // void AddNode(EgIndexNode* parent, EgIndexNode* node, bool is_left);
};

#ifdef EG_LIB_BUILD     // library build switch, define it in project or egCore.h
#include "ecographdb_global.h"

class ECOGRAPHDBSHARED_EXPORT EgIndexCondition

#else                   // not a library build

class EgIndexCondition // logical node envelope

#endif

{
public:

    // EgIndexConditionsTree* iTree;
    EgIndexNode* iTreeNode = nullptr;

    // EgIndexCondition(EgIndexConditionsTree* myTree); // : iTree(myTree) {}
    // EgIndexCondition(EgDataNodesType& odb, QString a_FieldName, int an_oper, QVariant a_value);

    EgIndexCondition() {}

    EgIndexCondition(QString a_FieldName, int an_oper, QVariant a_value);
    EgIndexCondition(QString a_FieldName, QString str_oper, QVariant a_value);

    ~EgIndexCondition() {}

    EgIndexCondition& operator && (const EgIndexCondition rvalue);
    EgIndexCondition& operator || (const EgIndexCondition rvalue);

    void AddNode(EgIndexNode* parent, EgIndexNode* node, bool is_left);

    int BuildTreeFromMap(QMap<EgIndexNodeIDtype, EgIndexNode> &indexNodes, const EgIndexNodeIDtype rootID);
    void RecursiveBuildTreeFromMap(QMap<EgIndexNodeIDtype, EgIndexNode>& indexNodes, const EgIndexNodeIDtype nodeID);
};

typedef EgIndexCondition IC; // alias


#endif // EG_INDEX_CONDITIONS_H
