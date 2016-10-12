/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "EgIndexConditions.h"
#include "../egDataNodesType.h"

using namespace egIndexConditionsNamespace;

EgIndexCondition::EgIndexCondition(QString a_FieldName, int an_oper, QVariant a_value):
        iTreeNode(new EgIndexNode())
{
    iTreeNode-> oper = an_oper;
    iTreeNode-> FieldName = a_FieldName;
    iTreeNode-> value = a_value;

    iTreeNode-> is_leaf = true;
}

EgIndexCondition::EgIndexCondition(QString a_FieldName, QString str_oper, QVariant a_value):
        iTreeNode(new EgIndexNode())
{

    iTreeNode-> FieldName = a_FieldName;
    iTreeNode-> value = a_value;

    iTreeNode-> is_leaf = true;

    if (str_oper == strEQ)
        iTreeNode-> oper = EQ;
    else if (str_oper == strGE)
        iTreeNode-> oper = GE;
    else if (str_oper == strLE)
        iTreeNode-> oper = LE;
    else if (str_oper == strGT)
        iTreeNode-> oper = GT;
    else if (str_oper == strLT)
        iTreeNode-> oper = LT;
    else if (str_oper == strLT)
        iTreeNode-> oper = GE;
    else if (str_oper == strNE)
        iTreeNode-> oper = NE;
    else
        qDebug() << "Bad operation code: " << str_oper << FN;

}

EgIndexCondition& EgIndexCondition::operator && (const EgIndexCondition rvalue)
{
    EgIndexCondition* newCond = new EgIndexCondition();
    EgIndexNode* newNode = new EgIndexNode();

    newNode-> oper = AND;
    // newNode-> is_leaf = false; // constructor

    AddNode(newNode, (EgIndexNode*) (rvalue.iTreeNode), RIGHT);
    AddNode(newNode, iTreeNode, LEFT);

    newCond-> iTreeNode = newNode;

    return *newCond;
}

EgIndexCondition& EgIndexCondition::operator || (const EgIndexCondition rvalue)
{
    EgIndexCondition* newCond = new EgIndexCondition();
    EgIndexNode* newNode = new EgIndexNode();

    newNode-> oper = OR;
    // newNode-> is_leaf = false; // constructor

    AddNode(newNode, (EgIndexNode*) (rvalue.iTreeNode), RIGHT);
    AddNode(newNode, iTreeNode, LEFT);

    newCond-> iTreeNode = newNode;

    return *newCond;
}

int EgIndexConditionsTree::CalcTreeSet(EgIndexNode* rootNode, QSet<quint64>& final_set, EgDataFiles *LocalFiles)
{
    if (rootNode)
    {
        RecursiveCalc(rootNode, LocalFiles);
        final_set = rootNode-> my_set; // FIXME remove copy

        RecursiveClearSets(rootNode);
    }

    return 0;
}

void EgIndexConditionsTree::RecursiveClear(EgIndexNode* branch)
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

void EgIndexConditionsTree::RecursiveClearSets(EgIndexNode* branch)
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

void EgIndexConditionsTree::RecursiveCalc(EgIndexNode* branch,   EgDataFiles*  LocalFiles)
{
    // qDebug() << "Select tree index " << branch->FieldName << (int) branch-> oper << FN;
    if (! branch)
        return;

    EgIndexFiles<qint32>* theIndexPtr;

    // FIXME find index

    if (branch-> is_leaf)
    {
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

void EgIndexCondition::AddNode(EgIndexNode* parent, EgIndexNode* new_node, bool is_left)
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

