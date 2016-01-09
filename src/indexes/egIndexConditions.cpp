#include "egIndexConditions.h"
#include "egDataNodesType.h"

using namespace egIndexConditionsNamespace;

EgIndexCondition::EgIndexCondition(EgIndexConditions* myTree):
        iTree(myTree),
        iTreeNode(new EgIndexNode())
{
    iTreeNode-> left   = NULL;
    iTreeNode-> right  = NULL;
    iTreeNode-> parent = NULL;

    iTreeNode-> is_leaf = false;
}

EgIndexCondition::EgIndexCondition(EgDataNodesType& odb, QString a_FieldName, int an_oper, QVariant a_value):
        iTree(odb.index_tree),
        iTreeNode(new EgIndexNode())
{
    iTreeNode-> oper = an_oper;
    iTreeNode-> FieldName = a_FieldName;
    iTreeNode-> value = a_value;

    iTreeNode-> is_leaf = true;
}

EgIndexCondition& EgIndexCondition::operator && (const EgIndexCondition rvalue)
{
    EgIndexCondition* new_cond = new EgIndexCondition(iTree);

    new_cond-> iTreeNode-> oper = AND;

    iTree-> AddNode(NULL, new_cond-> iTreeNode, 0);

    iTree-> AddNode(new_cond-> iTreeNode, (EgIndexNode*) (rvalue.iTreeNode), RIGHT);
    iTree-> AddNode(new_cond-> iTreeNode, iTreeNode, LEFT);

    iTree-> root = new_cond-> iTreeNode;

    return *new_cond;
}

EgIndexCondition& EgIndexCondition::operator || (const EgIndexCondition rvalue)
{
    EgIndexCondition* new_cond = new EgIndexCondition(iTree);

    new_cond-> iTreeNode-> oper = OR;

    iTree-> AddNode(NULL, new_cond-> iTreeNode, 0);

    iTree-> AddNode(new_cond-> iTreeNode, (EgIndexNode*) (rvalue.iTreeNode), RIGHT);
    iTree-> AddNode(new_cond-> iTreeNode, iTreeNode, LEFT);

    iTree-> root = new_cond-> iTreeNode;

    return *new_cond;
}

/*
void EgIndexConditions::Init(EgIndexFiles *indexes_ptr)
{
    indexes = indexes_ptr;
}
*/

int EgIndexConditions::CalcTreeSet(QSet<quint64>& final_set, EgDataFiles *LocalFiles)
{
    if (root)
    {
        RecursiveCalc(root, LocalFiles);
        final_set = root-> my_set;

        clearSets();
    }

    return 0;
}

void EgIndexConditions::clear()
{
    RecursiveClear(root);
    root = NULL;
}

void EgIndexConditions::RecursiveClear(EgIndexNode* branch)
{
    if (branch)
    {
        if (branch-> left)
            RecursiveClear(branch-> left);
        if (branch-> right)
            RecursiveClear(branch-> right);

        delete branch;
    }
}

void EgIndexConditions::clearSets()
{
    RecursiveClearSets(root);
}

void EgIndexConditions::RecursiveClearSets(EgIndexNode* branch)
{
    if (branch)
    {
        if (branch-> left)
            RecursiveClearSets(branch-> left);
        if (branch-> right)
            RecursiveClearSets(branch-> right);

        branch-> my_set.clear();
    }
}

void EgIndexConditions::RecursiveCalc(EgIndexNode* branch,   EgDataFiles*  LocalFiles)
{
    // qDebug() << "Select tree index " << branch->FieldName << (int) branch-> oper << FN;
    if (! branch)
        return;

    EgIndexFiles<qint32>* theIndexPtr;

    // FIXME find index

    if (branch->FieldName == "odb_pit")
        theIndexPtr = LocalFiles-> primIndexFiles;
    else if (LocalFiles-> indexFiles.contains(branch->FieldName))
    {
        theIndexPtr = LocalFiles-> indexFiles[branch->FieldName];
    }
    else
    {
        qDebug() << FN << "Index not found: " << branch->FieldName;
        return;
    }

    if (branch-> is_leaf)
    {
        switch (branch-> oper)
        {
        case EQ:
            theIndexPtr-> Load_EQ(branch->my_set, branch->value.toInt());
            break;
        case GE:
            theIndexPtr-> Load_GE(branch->my_set, branch->value.toInt());
            break;
        case LE:
            theIndexPtr-> Load_LE(branch->my_set, branch->value.toInt());
            break;
        case GT:
            theIndexPtr-> Load_GT(branch->my_set, branch->value.toInt());
            break;
        case LT:
            theIndexPtr-> Load_LT(branch->my_set, branch->value.toInt());
            break;
        }
    }
    else
    {
        if (branch-> left)
            RecursiveCalc(branch-> left, LocalFiles);
        if (branch-> right)
            RecursiveCalc(branch-> right, LocalFiles);

        switch (branch-> oper)
        {
        case AND:
            branch->my_set = (branch->left-> my_set) & (branch->right-> my_set);
            break;

        case OR:
            branch->my_set = branch->left-> my_set | branch->right-> my_set;
            break;
        }
    }

    // qDebug() << FN << branch->my_set;
}

void EgIndexConditions::AddNode(EgIndexNode* parent, bool is_leaf, bool is_left, int oper, QString FieldName, QVariant value)   // add combination node
{
    EgIndexNode* new_node;

    new_node = new EgIndexNode();
    // new_node-> my_set = new QSet<quint64>();

    new_node->left = NULL;
    new_node->right = NULL;
    new_node->parent = parent;

    new_node-> is_leaf = is_leaf;

    new_node-> oper = oper;
    new_node-> FieldName = FieldName;
    new_node-> value = value;

    if (! parent) // add root node
    {
        if (! root)
            root = new_node;
        // else error
    }
    else
    {
        if (is_left)
            parent->left = new_node;
        else
            parent->right = new_node;
    }
}

void EgIndexConditions::AddNode(EgIndexNode* parent, EgIndexNode* new_node, bool is_left)
{
    // new_node->left = NULL;
    // new_node->right = NULL;
    // new_node-> is_leaf = is_leaf;

    if (parent)
    {
        new_node->parent = parent;

        if (is_left)
            parent->left = new_node;
        else
            parent->right = new_node;
    }
}

