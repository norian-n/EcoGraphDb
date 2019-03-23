/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2018 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

#include "egGUIconnect.h"
#include "egDataNodesType.h"
#include "egDataClient.h"

#include <QFile>

using namespace EgDataNodesNamespace;

EgBasicControlDesc::EgBasicControlDesc(EgDataNode &dataNode):
    controlLabel(dataNode["label"].toString()),
    controlDefWidth(dataNode["width"].toInt())
{

}

EgDataNodesGUIsupport::EgDataNodesGUIsupport(EgDataNodesType* thePrimaryNodesType):
    primaryNodesType(thePrimaryNodesType),
    controlDescs (new EgDataNodesType())
{

}

EgDataNodesGUIsupport::~EgDataNodesGUIsupport()
{
    if (controlDescs)
        delete controlDescs;
}

/*
void EgDataNodesGUIconnect::Init()
{
    LoadSimpleControlDesc();
}
*/
/*
int EgDataNodesGUIsupport::CreateDataNodesForControlDescs()
{
    EgDataNodeTypeMetaInfo metaInfo(dataNodesType-> metaInfo.typeName + egGUIfileName);

    metaInfo.AddDataField("name");
    metaInfo.AddDataField("label");
    metaInfo.AddDataField("width");

        // store meta-info
    metaInfo.LocalStoreMetaInfo();

    return 0;
}
*/

int EgDataNodesGUIsupport::LoadSimpleControlDesc()
{
    EgBasicControlDesc newDesc;

/*
        // check if GUI descriptors exists - FIXME for server
    QFile dat_file;
    dat_file.setFileName(dataNodesType-> metaInfo.typeName + egGUIfileName + ".dat");

    if (!dat_file.exists())
    {
        // qDebug() << FN << "file  doesn't exist" << dataNodesType-> metaInfo.typeName + egGUIfileName + ".dat";
        return -1;
    }
*/
    // if (! controlDescs)
    //    controlDescs = new EgDataNodesType();

    if (controlDescs-> ConnectServiceNodeType(*(primaryNodesType-> extraInfo.myECoGraphDB), primaryNodesType-> extraInfo.typeName + egGUIfileName))
    {
            // qDebug()  << "No control descs for dataNodeType " << primaryNodesType-> metaInfo.typeName + egGUIfileName << FN;
            return 1;
    }

    basicControlDescs.clear();

    controlDescs-> LoadAllDataNodes();

    // qDebug()  << "Control descs count = " << controlDescs-> dataNodes.count() << FN;

    for (QMap<EgDataNodeIdType, EgDataNode>::iterator nodesIter = controlDescs-> dataNodes.begin(); nodesIter != controlDescs-> dataNodes.end(); ++nodesIter)
    {
        if (primaryNodesType-> extraInfo.nameToOrder.contains(nodesIter.value()["name"].toString()))
        {
            newDesc = EgBasicControlDesc(nodesIter.value());
            newDesc.fieldIndex = primaryNodesType-> extraInfo.nameToOrder[nodesIter.value()["name"].toString()];
            // newDesc.controlLabel = nodesIter.value()["label"].toString();
            // newDesc.controlDefWidth = nodesIter.value()["width"].toInt();

            // qDebug()  << "Control desc added " << newDesc.fieldIndex << FN;

            basicControlDescs.append(newDesc);
            basicControlDescsOrder.insert(nodesIter.value()["name"].toString(), &(basicControlDescs.last()));
        }
        else
            qDebug()  << "Bad control desc field name " << nodesIter.value()["name"].toString() << FN;
    }


    return 0;
}
/*
bool EgDataNodesGUIsupport::CheckLocalGUIFile()
{
    QFile ddt_file;

    ddt_file.setFileName("egdb/" + dataNodesType-> metaInfo.typeName + egGUIfileName + ".ddt");

    if (!ddt_file.exists())
    {
        // qDebug() << "file doesn't exist' " << dataNodesType-> metaInfo.typeName + egGUIfileName + ".ddt" << FN ;
        return false;
    }

    return true;
}
*/

int EgDataNodesGUIsupport::AddSimpleControlDesc(QString fieldName, QString fieldLabel, int fieldWidth)
{
    QList<QVariant> addValues;

    // if (! controlDescs)
    //    controlDescs = new EgDataNodesType(); // FIXME constructor

    // if (! CheckLocalGUIFile())      // FIXME server
    //    CreateDataNodesForControlDescs();

    controlDescs-> Connect(*(primaryNodesType-> extraInfo.myECoGraphDB), primaryNodesType-> extraInfo.typeName + egGUIfileName);

    addValues << fieldName << fieldLabel << fieldWidth;

    controlDescs-> AddDataNode(addValues);
    controlDescs-> StoreData();

    return 0;
}

int EgDataNodesGUIsupport::AddRowOfModel(QStandardItemModel* model, QList<QStandardItem *>& items) // add items to model
{
     QStandardItem *item;
     QStandardItem *parentItem = model->invisibleRootItem();
        // fill with empty strings by default
     if (items.isEmpty())
     {
         for (int column = 0; column < basicControlDescs.count(); ++column)
         {
             item = new QStandardItem("");
             items << item;
         }
     }

    items[0]->setData(QVariant(isAdded), data_status); // added row
    items[0]->setData(QVariant(0), data_id); // OBJ_ID

    // model->appendRow(items);
    parentItem->appendRow(items);

    return 0;
}

int EgDataNodesGUIsupport::DeleteRowOfModel(QStandardItemModel* model)
{
    primaryNodesType-> DeleteDataNode(model->item(model_current_row,0)-> data(data_id).toInt());

    model->removeRow(model_current_row);

    return 0;
}


int EgDataNodesGUIsupport::ModifyRowOfModel(QStandardItemModel* model)
{
    if (model->item(model_current_row,0)-> data(data_status).toInt() == isUnchanged) // not added or already modified
        model->item(model_current_row,0)-> setData(QVariant(isModified), data_status); // mark as modified

    // qDebug() << FN << "Model data changed at row " << current_row; //  << "," << bottomRight.row();

    return 0;
}


EgDataNodeIdType EgDataNodesGUIsupport::GetIDByModel(QStandardItemModel* model)
{
    if (! model->item(model_current_row,0))
        return 0;

    return model->item(model_current_row,0)->data(data_id).toInt();
}

void EgDataNodesGUIsupport::SetModelHeaders(QStandardItemModel* model)
{
    int i = 0;

    for (QList<EgBasicControlDesc>::iterator curDesc = basicControlDescs.begin(); curDesc != basicControlDescs.end(); ++curDesc)
    {
        model->setHorizontalHeaderItem(i, new QStandardItem((*curDesc).controlLabel));
        i++;
    }
}

void EgDataNodesGUIsupport::SetViewWidths(QTableView* tableView)
{
    int i = 0;

    for (QList<EgBasicControlDesc>::iterator curDesc = basicControlDescs.begin(); curDesc != basicControlDescs.end(); ++curDesc)
    {
        tableView-> setColumnWidth(i, (*curDesc).controlDefWidth);
        i++;
    }
}

void EgDataNodesGUIsupport::SetViewWidths(QTreeView* treeView)
{
    int i = 0;

    for (QList<EgBasicControlDesc>::iterator curDesc = basicControlDescs.begin(); curDesc != basicControlDescs.end(); ++curDesc)
    {
        treeView-> setColumnWidth(i, (*curDesc).controlDefWidth);
        i++;
    }
}

void EgDataNodesGUIsupport::DataToModel(QStandardItemModel* model)
{
    QList<QStandardItem*> items;
    // QStandardItem *parentItem = model-> invisibleRootItem();

        // push data nodes to model
    model->clear();
    SetModelHeaders(model);  // fill headers

    for (QMap<EgDataNodeIdType, EgDataNode>::iterator dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
    {
        items.clear();
        for (QList<EgBasicControlDesc>::iterator curDesc = basicControlDescs.begin(); curDesc != basicControlDescs.end(); ++curDesc)
        {
            if (! (*curDesc).AutoSubstClass)
                items << new QStandardItem(dataNodeIter.value().dataFields[(*curDesc).fieldIndex].toString());
            else
            {
                EgDataNodeIdType substNodeID = dataNodeIter.value().dataFields[(*curDesc).fieldIndex].toInt();
                items << new QStandardItem((*(*curDesc).AutoSubstClass)[substNodeID].dataFields[(*curDesc).AutoSubstFieldIndex].toString());
            }
        }

        // qDebug() << model->rowCount() << FN;

        items[0]->setData(QVariant(isUnchanged), data_status);                 // loaded data status
        items[0]->setData(QVariant(dataNodeIter.value().dataNodeID), data_id);  // ID

        // qDebug() << dataNodeIter.value().dataNodeID << FN;

        model-> appendRow(items);
        // parentItem-> appendRow(items);
    }
}

inline QStandardItem* EgDataNodesGUIsupport::AddNodeToModelTree(QStandardItem* parentItem, EgDataNode* dataNode)
{
    QList<QStandardItem*> items;

        // check if GUI info loaded
    if (basicControlDescs.isEmpty())
    {
        qDebug() << "GUI info not loaded for node type " << dataNode->extraInfo-> typeName << FN;
        return NULL;
    }

    // items.clear();
    for (QList<EgBasicControlDesc>::iterator curDesc = basicControlDescs.begin(); curDesc != basicControlDescs.end(); ++curDesc)
    {
        if (! (*curDesc).AutoSubstClass)
            items << new QStandardItem(dataNode-> dataFields[(*curDesc).fieldIndex].toString());
        else
        {
            EgDataNodeIdType substNodeID = dataNode-> dataFields[(*curDesc).fieldIndex].toInt();
            items << new QStandardItem((*(*curDesc).AutoSubstClass)[substNodeID].dataFields[(*curDesc).AutoSubstFieldIndex].toString());
        }
    }

    items[0]->setData(QVariant(isUnchanged), data_status);       // loaded data status
    items[0]->setData(QVariant(dataNode-> dataNodeID), data_id);  // ID

    parentItem-> appendRow(items);

    return items[0];
}

int EgDataNodesGUIsupport::DataToModelTree(QStandardItemModel* model, QString linkName)
{
    // QList<QStandardItem*> items;
    TreeBuildNodeType buildNode, buildNode2;
    QList <TreeBuildNodeType> buildNodes;
    // QList<EgExtendedLinkType> linkedNodes;

    EgDataNode* entryNodePtr = nullptr;

    model->clear();    
    SetModelHeaders(model);  // fill headers

    QStandardItem* topItem = model->invisibleRootItem();
    QStandardItem* parentItem = NULL;
    QStandardItem* newItem = NULL;

    if (! primaryNodesType-> entryNodes)   // no entry nodes option
    {
        qDebug()  << "Entry nodes not enabled for type: " << primaryNodesType->extraInfo.typeName << FN;
        return -1;
    }

        // iterate entry nodes
    for (auto Iter = primaryNodesType->entryNodes-> entryNodesList.begin(); Iter != primaryNodesType->entryNodes-> entryNodesList.end(); ++Iter)
    {
        // qDebug()  << "Entry node ID = " << Iter.key() << FN;

            // check if node loaded
        if (! primaryNodesType-> dataNodes.contains(*Iter))
        {
            // qDebug()  << "Entry node ID not found in dataNodesType-> dataNodes " << Iter.key() << FN;
            // qDebug()  << dataNodesType-> dataNodes.keys() << FN;

            continue;
        }

            // add entry node
        parentItem = topItem;
        entryNodePtr = &((*primaryNodesType)[*Iter]);
        newItem = AddNodeToModelTree(parentItem, entryNodePtr); // Iter.value()); // dataNodesType[Iter.key()]

        if (! newItem)
            return -1; // ERR

        parentItem = newItem;

        // return 1; // FIXME STUB

            // add out links to queue (list)
        if (entryNodePtr-> nodeLinks
            && (! entryNodePtr-> nodeLinks-> outLinks.empty()) && entryNodePtr-> nodeLinks-> outLinks.contains(linkName))
        {
            for (QList<EgExtendedLinkType>::iterator Iter2 = entryNodePtr-> nodeLinks-> outLinks[linkName].begin(); Iter2 != entryNodePtr-> nodeLinks-> outLinks[linkName].end(); ++Iter2)
            {
                // qDebug()  << "branch node ID = " << (*Iter2).dataNodePtr-> dataNodeID << FN;

                newItem = AddNodeToModelTree(parentItem, (*Iter2).dataNodePtr);

                if ((*Iter2).dataNodePtr && (*Iter2).dataNodePtr-> nodeLinks
                    && (! (*Iter2).dataNodePtr-> nodeLinks-> outLinks.empty()) && (*Iter2).dataNodePtr-> nodeLinks-> outLinks.contains(linkName))
                {
                    buildNode.dataNode = (*Iter2).dataNodePtr;
                    buildNode.modelItem = newItem;

                    // qDebug()  << "new buildNode ID = " << buildNode.dataNode-> dataNodeID
                    //           << "ptr = " << hex << (int) buildNode.dataNode << FN;

                    buildNodes.append(buildNode);
                }
            }
        }


            // process queue (list)
        while (! buildNodes.isEmpty())
        {
            buildNode = buildNodes.first();
            buildNodes.pop_front();

            // qDebug()  << "build node ID = " << buildNode.dataNode-> dataNodeID << FN;

            parentItem = buildNode.modelItem;

            // if (buildNode.dataNode && buildNode.dataNode-> nodeLinks && (! buildNode.dataNode-> nodeLinks-> outLinks.empty()) && buildNode.dataNode-> nodeLinks-> outLinks.contains(linkName))

            for (QList<EgExtendedLinkType>::iterator Iter3 = buildNode.dataNode-> nodeLinks-> outLinks[linkName].begin(); Iter3 != buildNode.dataNode-> nodeLinks-> outLinks[linkName].end(); ++Iter3)
            {

                // qDebug()  << "subbranch node ID = " << (*Iter3).dataNodePtr-> dataNodeID << FN;

                newItem = AddNodeToModelTree(parentItem, (*Iter3).dataNodePtr);

                    // add next level
                if ((*Iter3).dataNodePtr && (*Iter3).dataNodePtr-> nodeLinks
                        && (! (*Iter3).dataNodePtr-> nodeLinks-> outLinks.empty()) && (*Iter3).dataNodePtr-> nodeLinks-> outLinks.contains(linkName)) // FIXME
                {
                    buildNode2.dataNode = (*Iter3).dataNodePtr;
                    buildNode2.modelItem = newItem;

                    // qDebug()  << "new buildNode ID = " << buildNode2.dataNode-> dataNodeID
                    //          << "ptr = " << hex << (int) buildNode2.dataNode << FN;

                    buildNodes.append(buildNode2);
                }
            }
        }
    }

    return 0;
}

int EgDataNodesGUIsupport::DataFromModel(QStandardItemModel* model) // get from model
{
    EgDataNode tmpDataNode;

    if (basicControlDescs.size() != model->columnCount())
    {
        qDebug()  << "ERROR: column counts of model and data node desc not match" << FN;
        return -1;
    }

        // scan model
    for (int row = 0; row < model-> rowCount(); ++row)
    {
        // qDebug() << FN << "Status of model row " << row << " is " << model->item(row,0)->data(DATA_STATUS).toInt();

        if (model->item(row,0)->data(data_status).toInt() == isAdded) // added data row
        {
            tmpDataNode.clear();

            for (int column = 0; column < model-> columnCount(); ++column)
                tmpDataNode.dataFields[basicControlDescs[column].fieldIndex] = model->item(row,column)->text();

            primaryNodesType-> AddDataNode(tmpDataNode);
        }
        else if (model->item(row,0)->data(data_status).toInt() == isModified) // updated data row
        {
                // update object
            for (int column = 0; column < model-> columnCount(); ++column)
                (*primaryNodesType)[model->item(row,0)->data(data_id).toInt()].dataFields[basicControlDescs[column].fieldIndex] = model->item(row,column)->text();

            primaryNodesType-> UpdateDataNode(model->item(row,0)->data(data_id).toInt());
        }
    }

    return 0;
}

void EgDataNodesGUIsupport::FillComboBox(QComboBox* my_box)
{
    my_box->clear();
    my_box->addItem(QString("<Not found>"), 0); // <Not found>

    for (QMap<EgDataNodeIdType, EgDataNode>::iterator dataNodeIter = primaryNodesType-> dataNodes.begin(); dataNodeIter != primaryNodesType-> dataNodes.end(); ++dataNodeIter)
           my_box->addItem(dataNodeIter.value().dataFields[0].toString(), QVariant(dataNodeIter.key())); // FIXME - field name
}

void EgDataNodesGUIsupport::SetComboBox(QComboBox* my_box, QVariant dobj_id)
{
        // search object id in box data
    for (int index =0; index < my_box->count(); index++)
    {
        if (my_box->itemData(index) == dobj_id)
        {
            my_box->setCurrentIndex(index);
            return;
        }
    }
        // not found - go top
    my_box->setCurrentIndex(0);
}

QVariant EgDataNodesGUIsupport::GetComboBoxID(QComboBox* my_box)
{
    return my_box->itemData(my_box->currentIndex());
}


int EgDataNodesGUIsupport::AddAutoSubstitute(const char* my_field, EgDataNodesType& ref_class, const char* ref_field)
{
    if (basicControlDescsOrder.contains(my_field) && ref_class.extraInfo.nameToOrder.contains(ref_field))
    {
        basicControlDescsOrder[my_field]-> AutoSubstClass = &ref_class;
        basicControlDescsOrder[my_field]-> AutoSubstFieldIndex = ref_class.extraInfo.nameToOrder[ref_field];

        return 0;
    }

    qDebug() << "Bad field name, descriptors follow" << FN;
    qDebug() << basicControlDescsOrder << FN;
    qDebug() << ref_class.extraInfo.nameToOrder << FN;

    return -1;
}

    /*
inline void EgDataNodesGUIconnect::BuildTreeMap()
{

    QMap<EgDataNodeIDtype, EgDataNode>::iterator current_obj = dobj_map.begin();
    if (FD[parent_field]>=0)
    {
        while (current_obj != dobj_map.end())
        {
            parent_map.insert(current_obj.value().data_fields[FD[parent_field]].toInt(), &(current_obj.value()));
            current_obj++;
        }
    }

}
    */
