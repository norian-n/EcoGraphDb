#ifndef EG_INDEX_CONDITIONS_H
#define EG_INDEX_CONDITIONS_H

#include <QString>
#include <QSet>
#include <QVariant>

#include "egIndexesFiles.h"
#include "egLocalFiles.h"

namespace egIndexConditionsNamespace
{

const bool RIGHT = false;
const bool LEFT = true;

const bool COMB = false;
const bool LEAF = true;

} // namespace

struct EgIndexNode
{
    int is_leaf;    // simple condition (1) or combination (0)
    int oper;       // operation type

    QSet<quint64> my_set;

    QString FieldName;
    QVariant value;

    EgIndexNode* left;
    EgIndexNode* right;
    EgIndexNode* parent;
};



class EgIndexConditions
{
public:
    // QSet<quint64> empty;

    EgIndexNode* root;
    // EgIndexFiles3<qint32> indexes;

    QString IndexBaseName;

    int CalcTreeSet(QSet<quint64>& final_set, EgDataFiles *LocalFiles);      // calc final set of tree
    void RecursiveCalc(EgIndexNode* branch, EgDataFiles *LocalFiles);

    void clear();      // clear tree

    EgIndexConditions(const QString& an_IndexBaseName) : root(NULL), IndexBaseName(an_IndexBaseName)  { }
    ~EgIndexConditions() { clear();}

    void Init(EgIndexFiles<qint32>* indexes_ptr);        // attach to indexes to get leaf sets

    void RecursiveClear(EgIndexNode* branch);

    void clearSets();      // clear sets in nodes
    void RecursiveClearSets(EgIndexNode* branch);

    void AddNode(EgIndexNode* parent, bool is_leaf, bool is_left, int oper, QString FieldName, QVariant value);
    void AddNode(EgIndexNode* parent, EgIndexNode* node, bool is_left);
};

class EgDataNodesType;

class EgIndexCondition
{
public:

    EgIndexConditions* iTree;
    EgIndexNode* iTreeNode;

    EgIndexCondition(EgIndexConditions* myTree); // : iTree(myTree) {}
    EgIndexCondition(EgDataNodesType& odb, QString a_FieldName, int an_oper, QVariant a_value);
    ~EgIndexCondition() {}

    EgIndexCondition& operator && (const EgIndexCondition rvalue);
    EgIndexCondition& operator || (const EgIndexCondition rvalue);
};

typedef EgIndexCondition IC; // alias


#endif // EG_INDEX_CONDITIONS_H
