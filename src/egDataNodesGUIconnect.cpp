#include "egDataNodesGUIconnect.h"
#include "egDataNodesType.h"

#include <QFile>

using namespace EgDataNodesGUInamespace;

EgBasicControlDesc::EgBasicControlDesc(EgDataNode &dataNode):
    AutoSubstClass(NULL)
{
    controlLabel    = dataNode["label"].toString();
    controlDefWidth = dataNode["width"].toInt();
}

EgDataNodesGUIconnect::EgDataNodesGUIconnect():
    controlDescs(NULL)
{ }

EgDataNodesGUIconnect::~EgDataNodesGUIconnect()
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

int EgDataNodesGUIconnect::CreateDataNodesForControlDescs()
{
    EgDataNodeTypeMetaInfo metaInfo(dataNodesType-> metaInfo.typeName + egGUIfileName);

    metaInfo.AddDataField("name");
    metaInfo.AddDataField("label");
    metaInfo.AddDataField("width");

        // store meta-info
    metaInfo.LocalStoreMetaInfo();

    return 0;
}

int EgDataNodesGUIconnect::LoadSimpleControlDesc()
{
    EgBasicControlDesc newDesc;

    basicControlDescs.clear();

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
    if (! controlDescs)
        controlDescs = new EgDataNodesType();

    if (controlDescs-> Connect(*(dataNodesType-> metaInfo.myECoGraphDB), dataNodesType-> metaInfo.typeName + egGUIfileName))
    {
            // qDebug()  << "No control descs for dataNodeType " << dataNodesType-> metaInfo.typeName << FN;
            return 1;
    }

    controlDescs-> LoadAllData();

    // qDebug()  << "Control descs count = " << controlDescs-> dataNodes.count() << FN;

    for (QMap<EgDataNodeIDtype, EgDataNode>::iterator nodesIter = controlDescs-> dataNodes.begin(); nodesIter != controlDescs-> dataNodes.end(); ++nodesIter)
    {
        if (dataNodesType-> metaInfo.nameToOrder.contains(nodesIter.value()["name"].toString()))
        {
            newDesc = EgBasicControlDesc(nodesIter.value());
            newDesc.fieldIndex = dataNodesType-> metaInfo.nameToOrder[nodesIter.value()["name"].toString()];
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

bool EgDataNodesGUIconnect::CheckLocalGUIFile()
{
    QFile ddt_file;

    ddt_file.setFileName(dataNodesType-> metaInfo.typeName + egGUIfileName + ".ddt");

    if (!ddt_file.exists())
    {
        // qDebug() << "file doesn't exist' " << dataNodesType-> metaInfo.typeName + egGUIfileName + ".ddt" << FN ;
        return false;
    }

    return true;
}

int EgDataNodesGUIconnect::AddSimpleControlDesc(QString fieldName, QString fieldLabel, int fieldWidth)
{
    QList<QVariant> addValues;

    if (! controlDescs)
        controlDescs = new EgDataNodesType();

    if (! CheckLocalGUIFile())      // FIXME server
        CreateDataNodesForControlDescs();

    controlDescs-> Connect(*(dataNodesType-> metaInfo.myECoGraphDB), dataNodesType-> metaInfo.typeName + egGUIfileName);

    addValues << fieldName << fieldLabel << fieldWidth;

    controlDescs-> AddNewData(addValues);
    controlDescs-> StoreData();

    return 0;
}

int EgDataNodesGUIconnect::AddRowOfModel(QStandardItemModel* model, QList<QStandardItem *>& items) // add items to model
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

    items[0]->setData(QVariant(is_added), data_status); // added row
    items[0]->setData(QVariant(0), data_id); // OBJ_ID

    // model->appendRow(items);
    parentItem->appendRow(items);

    return 0;
}

int EgDataNodesGUIconnect::DeleteRowOfModel(QStandardItemModel* model)
{
    dataNodesType-> MarkDeletedData(model->item(model_current_row,0)-> data(data_id).toInt());

    model->removeRow(model_current_row);

    return 0;
}


int EgDataNodesGUIconnect::ModifyRowOfModel(QStandardItemModel* model)
{
    if (model->item(model_current_row,0)-> data(data_status).toInt() == is_unchanged) // not added or already modified
        model->item(model_current_row,0)-> setData(QVariant(is_modified), data_status); // mark as modified

    // qDebug() << FN << "Model data changed at row " << current_row; //  << "," << bottomRight.row();

    return 0;
}


EgDataNodeIDtype EgDataNodesGUIconnect::GetIDByModel(QStandardItemModel* model)
{
    if (! model->item(model_current_row,0))
        return 0;

    return model->item(model_current_row,0)->data(data_id).toInt();
}

void EgDataNodesGUIconnect::SetModelHeaders(QStandardItemModel* model)
{
    int i = 0;

    for (QList<EgBasicControlDesc>::iterator curDesc = basicControlDescs.begin(); curDesc != basicControlDescs.end(); ++curDesc)
    {
        model->setHorizontalHeaderItem(i, new QStandardItem((*curDesc).controlLabel));
        i++;
    }
}

void EgDataNodesGUIconnect::SetViewWidths(QTableView* tableView)
{
    int i = 0;

    for (QList<EgBasicControlDesc>::iterator curDesc = basicControlDescs.begin(); curDesc != basicControlDescs.end(); ++curDesc)
    {
        tableView-> setColumnWidth(i, (*curDesc).controlDefWidth);
        i++;
    }
}

void EgDataNodesGUIconnect::SetViewWidths(QTreeView* treeView)
{
    int i = 0;

    for (QList<EgBasicControlDesc>::iterator curDesc = basicControlDescs.begin(); curDesc != basicControlDescs.end(); ++curDesc)
    {
        treeView-> setColumnWidth(i, (*curDesc).controlDefWidth);
        i++;
    }
}

void EgDataNodesGUIconnect::DataToModel(QStandardItemModel* model)
{
    QList<QStandardItem*> items;
    // QStandardItem *parentItem = model-> invisibleRootItem();

        // push data nodes to model
    model->clear();
    SetModelHeaders(model);  // fill headers

    for (QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodeIter = dataNodesType-> dataNodes.begin(); dataNodeIter != dataNodesType-> dataNodes.end(); ++dataNodeIter)
    {
        items.clear();
        for (QList<EgBasicControlDesc>::iterator curDesc = basicControlDescs.begin(); curDesc != basicControlDescs.end(); ++curDesc)
        {
            if (! (*curDesc).AutoSubstClass)
                items << new QStandardItem(dataNodeIter.value().dataFields[(*curDesc).fieldIndex].toString());
            else
            {
                EgDataNodeIDtype substNodeID = dataNodeIter.value().dataFields[(*curDesc).fieldIndex].toInt();
                items << new QStandardItem((*(*curDesc).AutoSubstClass)[substNodeID].dataFields[(*curDesc).AutoSubstFieldIndex].toString());
            }
        }

        // qDebug() << model->rowCount() << FN;

        items[0]->setData(QVariant(is_unchanged), data_status);                 // loaded data status
        items[0]->setData(QVariant(dataNodeIter.value().dataNodeID), data_id);  // ID

        // qDebug() << dataNodeIter.value().dataNodeID << FN;

        model-> appendRow(items);
        // parentItem-> appendRow(items);
    }
}

int EgDataNodesGUIconnect::DataToModelTree(QStandardItemModel* model) // set to model
{
    QList<QStandardItem*> items;
    // QStandardItem* parentItem = model-> invisibleRootItem();

        // push data nodes to model
    model->clear();

    /*
    QMultiMap<EgDataNodeIDtype, EgDataNode*>::iterator cur_obj;
    QList<EgSampleControlDesc>::iterator control_iter;
    QList<QStandardItem *> items;
    EgDataNode* data_obj;

    model->clear();     // clear model
    SetModelHeaders(model);  // fill headers
        // load data
    QStandardItem* parentItem = model->invisibleRootItem();
    cur_obj = parent_map.begin();
    while (cur_obj != parent_map.end())
    {
        data_obj = cur_obj.value();

        if (data_obj->data_obj_status != is_deleted) // check if data obj was deleted
        {
                // add fields
            items.clear();
            control_iter = FD.control_desc_list.begin();
            while (control_iter != FD.control_desc_list.end())
            {
                if (! (*control_iter).ALinkClass)
                {
                    // items << new QStandardItem(cur_obj.value().data_fields[(*control_iter).field_index].toString());
                    items << new QStandardItem(data_obj->data_fields[(*control_iter).field_index].toString());
                }
                else
                {
                    items << new QStandardItem
((*((*control_iter).ALinkClass))[data_obj->data_fields[(*control_iter).field_index].toInt()].data_fields[(*control_iter).ALinkIndex].toString());
                }
                control_iter++;
            }

            items[0]->setData(QVariant(data_obj->data_obj_status), data_status); // loaded data
            items[0]->setData(QVariant(data_obj->OBJ_ID), data_id); // OBJ_ID

            data_obj->modelItem = items[0];
            if (cur_obj.key())
                parentItem = dobj_map.find(cur_obj.key()).value().modelItem; // parent
            else
                parentItem = model->invisibleRootItem();    // top

            parentItem->appendRow(items);
        }
        cur_obj++;
    }
    */
    return 0;
}

int EgDataNodesGUIconnect::DataFromModel(QStandardItemModel* model) // get from model
{
    EgDataNode tmpDataNode;

    if (basicControlDescs.size() != model->columnCount())
    {
        qDebug() << FN << "ERROR: column counts of model and data node desc not match" ;
        return -1;
    }

        // scan model
    for (int row = 0; row < model-> rowCount(); ++row)
    {
        // qDebug() << FN << "Status of model row " << row << " is " << model->item(row,0)->data(DATA_STATUS).toInt();

        if (model->item(row,0)->data(data_status).toInt() == is_added) // added data row
        {
            tmpDataNode.clear();

            for (int column = 0; column < model-> columnCount(); ++column)
                tmpDataNode.dataFields[basicControlDescs[column].fieldIndex] = model->item(row,column)->text();

            dataNodesType-> AddNewData(tmpDataNode);
        }
        else if (model->item(row,0)->data(data_status).toInt() == is_modified) // updated data row
        {
                // update object
            for (int column = 0; column < model-> columnCount(); ++column)
                (*dataNodesType)[model->item(row,0)->data(data_id).toInt()].dataFields[basicControlDescs[column].fieldIndex] = model->item(row,column)->text();

            dataNodesType-> SetModifiedData(model->item(row,0)->data(data_id).toInt());
        }
    }

    return 0;
}

void EgDataNodesGUIconnect::FillComboBox(QComboBox* my_box)
{
    my_box->clear();
    my_box->addItem(QString("<Not found>"), 0); // <Not found>

    for (QMap<EgDataNodeIDtype, EgDataNode>::iterator dataNodeIter = dataNodesType-> dataNodes.begin(); dataNodeIter != dataNodesType-> dataNodes.end(); ++dataNodeIter)
           my_box->addItem(dataNodeIter.value().dataFields[0].toString(), QVariant(dataNodeIter.key())); // FIXME - field name
}

void EgDataNodesGUIconnect::SetComboBox(QComboBox* my_box, QVariant dobj_id)
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

QVariant EgDataNodesGUIconnect::GetComboBoxID(QComboBox* my_box)
{
    return my_box->itemData(my_box->currentIndex());
}


int EgDataNodesGUIconnect::AddAutoSubstitute(const char* my_field, EgDataNodesType& ref_class, const char* ref_field)
{
    if (basicControlDescsOrder.contains(my_field) && ref_class.metaInfo.nameToOrder.contains(ref_field))
    {
        basicControlDescsOrder[my_field]-> AutoSubstClass = &ref_class;
        basicControlDescsOrder[my_field]-> AutoSubstFieldIndex = ref_class.metaInfo.nameToOrder[ref_field];

        return 0;
    }

    qDebug() << "Bad field name, descs follow" << FN;
    qDebug() << basicControlDescsOrder << FN;
    qDebug() << ref_class.metaInfo.nameToOrder << FN;

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
