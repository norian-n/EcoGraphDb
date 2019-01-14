/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "EgIndexConditions.h"
#include "../egDataNodesType.h"

namespace egIndexConditionsNamespace
{
    static EgIndexNodeIDtype nextIndexNodeID = 1;
}

using namespace egIndexConditionsNamespace;


QDataStream& operator << (QDataStream& d_stream, EgIndexNode& indexNode)   // transfer and file operations
{
    d_stream << indexNode.nodeID;

    d_stream << indexNode.FieldName;
    d_stream << indexNode.value;
    d_stream << indexNode.oper;

    d_stream << indexNode.is_leaf;

    d_stream << indexNode.leftID;
    d_stream << indexNode.rightID;
    d_stream << indexNode.parentID;

    return d_stream;
}

QDataStream& operator >> (QDataStream& d_stream, EgIndexNode& indexNode)         // unpack
{
    d_stream >> indexNode.nodeID;

    d_stream >> indexNode.FieldName;
    d_stream >> indexNode.value;
    d_stream >> indexNode.oper;

    d_stream >> indexNode.is_leaf;

    d_stream >> indexNode.leftID;
    d_stream >> indexNode.rightID;
    d_stream >> indexNode.parentID;

    return d_stream;
}

EgIndexCondition::EgIndexCondition(QString a_FieldName, int an_oper, QVariant a_value):
        iTreeNode(new EgIndexNode())
{
    iTreeNode-> nodeID = nextIndexNodeID++; // global , not thread safe

    iTreeNode-> oper = an_oper;
    iTreeNode-> FieldName = a_FieldName;
    iTreeNode-> value = a_value;

    iTreeNode-> is_leaf = true;
}

EgIndexCondition::EgIndexCondition(QString a_FieldName, QString str_oper, QVariant a_value):
        iTreeNode(new EgIndexNode())
{
    iTreeNode-> nodeID = nextIndexNodeID++; // global , not thread safe

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

    newNode-> subnodesCount = rvalue.iTreeNode-> subnodesCount + iTreeNode-> subnodesCount; // FIXME check
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


int EgIndexConditionsTree::TransferTreeSet(EgIndexNode* rootNode, QDataStream& d_stream)
{
    if (rootNode)
    {
        // qDebug() << "rootNode-> subnodesCount+1: " << rootNode-> subnodesCount+1 << " nodeID: " << rootNode-> nodeID << FN;

        d_stream << rootNode-> subnodesCount+1;
        d_stream << rootNode-> nodeID;

        RecursiveTransfer(rootNode, d_stream);
    }

    return 0;
}

void EgIndexConditionsTree::RecursiveTransfer(EgIndexNode* branch, QDataStream& d_stream)
{
    // qDebug() << "Transfer  tree index " << branch->FieldName << (int) branch-> oper << " " << branch->value << FN;

    if (! branch)
        return;

    if (branch-> left)
        RecursiveTransfer(branch-> left, d_stream);
    if (branch-> right)
        RecursiveTransfer(branch-> right, d_stream);

    d_stream << *branch;

    // qDebug() << FN << branch->my_set;
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

    EgIndexFilesBase* theIndexPtr;

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
            qDebug() << "Index not found: " << branch->FieldName << FN;
            return;
        }

        switch (branch-> oper)
        {
        case EQ:
            theIndexPtr-> Load_EQ(branch->my_set, branch->value);
            break;
        case GE:
            theIndexPtr-> Load_GE(branch->my_set, branch->value);
            break;
        case LE:
            theIndexPtr-> Load_LE(branch->my_set, branch->value);
            break;
        case GT:
            theIndexPtr-> Load_GT(branch->my_set, branch->value);
            break;
        case LT:
            theIndexPtr-> Load_LT(branch->my_set, branch->value);
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

    // qDebug() << branch->my_set << FN;
}

void EgIndexCondition::AddNode(EgIndexNode* parent, EgIndexNode* new_node, bool is_left)
{
    // new_node->left = NULL;
    // new_node->right = NULL;
    // new_node-> is_leaf = is_leaf;

    if (parent) // FIXME check if to be removed
    {
        new_node-> parent = parent;
        new_node-> parentID = parent-> nodeID;

        parent-> subnodesCount++;

        if (is_left)
        {
            parent-> left = new_node;
            parent-> leftID = new_node-> nodeID;
        }
        else
        {
            parent-> right = new_node;
            parent-> rightID = new_node-> nodeID;
        }
    }
}

int EgIndexCondition::BuildTreeFromMap(QMap<EgIndexNodeIDtype, EgIndexNode>& indexNodes, const EgIndexNodeIDtype rootID)
{
    // find root
    if (rootID && indexNodes.contains(rootID))
    {
        iTreeNode = &(indexNodes[rootID]);

        RecursiveBuildTreeFromMap(indexNodes, rootID);
    }
    else
    {
        iTreeNode = nullptr;

        qDebug() << "Root node ID not found: " << rootID << FN;
        return -1;
    }

    // add left and right recursively

    return 0;
}

void EgIndexCondition::RecursiveBuildTreeFromMap(QMap<EgIndexNodeIDtype, EgIndexNode>& indexNodes, const EgIndexNodeIDtype nodeID)
{
    EgIndexNode* currentTreeNode;

    if (nodeID && indexNodes.contains(nodeID))
    {
        currentTreeNode = &(indexNodes[nodeID]);

        if ((currentTreeNode-> rightID) && indexNodes.contains(currentTreeNode->rightID))
        {
            currentTreeNode-> right = &(indexNodes[currentTreeNode->rightID]);
            indexNodes[currentTreeNode->rightID].parent = currentTreeNode;

            RecursiveBuildTreeFromMap(indexNodes, currentTreeNode->rightID);
        }

        if ((currentTreeNode-> leftID) && indexNodes.contains(currentTreeNode->leftID))
        {
            currentTreeNode-> left = &(indexNodes[currentTreeNode->leftID]);
            indexNodes[currentTreeNode->leftID].parent = currentTreeNode;

            RecursiveBuildTreeFromMap(indexNodes, currentTreeNode->leftID);
        }
    }

}
