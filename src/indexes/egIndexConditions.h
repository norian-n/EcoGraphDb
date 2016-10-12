/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
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
    bool is_leaf;    // simple condition (1) or combination (0)

    QSet<quint64> my_set;

    QString FieldName;
    QVariant value;
    int oper;       // logical operation type - enum LOType in egCore.h or enum FilterType if it is a leaf

    EgIndexNode* left;
    EgIndexNode* right;
    EgIndexNode* parent;

    EgIndexNode(): is_leaf(false), left(NULL), right(NULL), parent(NULL) {}
};



class EgIndexConditionsTree // conditional nodes tree operations
{
public:

    EgIndexNode* root;

    int CalcTreeSet(EgIndexNode *root, QSet<quint64>& final_set, EgDataFiles *LocalFiles);      // calc final set of tree
    void RecursiveCalc(EgIndexNode* branch, EgDataFiles *LocalFiles);

    // void clear(EgIndexNode *rootNode);      // clear tree
    void RecursiveClear(EgIndexNode* branch);

    EgIndexConditionsTree(): root(NULL) {}
    ~EgIndexConditionsTree() { if (root) RecursiveClear(root);}

    void Init(EgIndexFiles<qint32>* indexes_ptr);        // attach to indexes to get leaf sets

    // void clearSets(EgIndexNode *rootNode);      // clear sets in nodes
    void RecursiveClearSets(EgIndexNode* branch);

    // void AddNode(EgIndexNode* parent, bool is_leaf, bool is_left, int oper, QString FieldName, QVariant value);
    // void AddNode(EgIndexNode* parent, EgIndexNode* node, bool is_left);
};

// class EgDataNodesType;

class EgIndexCondition // logical node envelope
{
public:

    // EgIndexConditionsTree* iTree;
    EgIndexNode* iTreeNode;

    // EgIndexCondition(EgIndexConditionsTree* myTree); // : iTree(myTree) {}
    // EgIndexCondition(EgDataNodesType& odb, QString a_FieldName, int an_oper, QVariant a_value);

    EgIndexCondition(): iTreeNode(NULL) {}
    EgIndexCondition(QString a_FieldName, int an_oper, QVariant a_value);
    EgIndexCondition(QString a_FieldName, QString str_oper, QVariant a_value);
    ~EgIndexCondition() {}

    EgIndexCondition& operator && (const EgIndexCondition rvalue);
    EgIndexCondition& operator || (const EgIndexCondition rvalue);

    void AddNode(EgIndexNode* parent, EgIndexNode* node, bool is_left);
};

typedef EgIndexCondition IC; // alias


#endif // EG_INDEX_CONDITIONS_H
