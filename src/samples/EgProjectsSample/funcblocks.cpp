/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#include "funcblocks.h"
#include <QtDebug>

FuncBlocksForm::FuncBlocksForm(QWidget *parent)
    : QWidget(parent)
  ,  ui(new Ui::FuncBlocksForm)
  ,  funcBlockForm(NULL)
  ,  model (NULL)
{
    ui->setupUi(this);
        // connect buttons
    connect(ui->addTopBlockButton, SIGNAL(clicked()),this, SLOT(addTopBlock()));
    connect(ui->addButton, SIGNAL(clicked()),this, SLOT(addSubBlock()));
    // connect(ui->editButton, SIGNAL(clicked()),this, SLOT(editSubBlock()));
    // connect(ui->deleteButton, SIGNAL(clicked()),this, SLOT(deleteSubBlock()));
    connect(ui->cancelButton, SIGNAL(clicked()),this, SLOT(cancelExit()));
    connect(ui->okButton, SIGNAL(clicked()),this, SLOT(okExit()));
    // connect(ui->compButton, SIGNAL(clicked()),this, SLOT(openCompForm()));
/*
    ui->treeWidget->setColumnCount(2);
    ui->treeWidget->setHeaderLabels(QStringList(QString("Func block")) << QString("Description"));
    ui->treeWidget->setColumnWidth(0,300);

    ui->treeWidget->clear();
*/

    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    // ui->treeView->setExpandsOnDoubleClick(true);
/*
    Funcblocks.Connect(graphDB, "funcblocks");
    // Funcblocks.parent_field = QString("parent");
    Funcblocks.LoadAllData();

    Funcblocks.LoadLinkedData("projects_funcblocks", project_id);

    Funcblocks.LoadLinks();
    */

/*
        // ref obj classes
    Statuses.Connect(graphDB, "status_data");
    Statuses.LoadData();
    Owners.Connect(graphDB, "owner_data");
    Owners.LoadData();

    */

    /*
        // datalinks:  status - create and set autolink
    FuncblockStatusLink = new LinkData(&Funcblocks, &Statuses); // create link
    FuncblockStatusLink->SetAutoLinkFields(Funcblocks.FD["status"], Statuses.FD["status"]); // set autolink
        // datalinks: owner - create and set autolink
    FuncblockOwnerLink = new LinkData(&Funcblocks, &Owners); // create link
    FuncblockOwnerLink->SetAutoLinkFields(Funcblocks.FD["owner"],Owners.FD["login"]); // set autolink
    */


    /*
    QList<LinkData*> autolinks_list;
    autolinks_list << FuncblockStatusLink << FuncblockOwnerLink;
    */
    /*
    Funcblocks.GUI-> AddAutoSubstitute("status", Statuses, "status");
    Funcblocks.GUI-> AddAutoSubstitute("owner",  Owners,   "login");


    */

    /*
    model = new QStandardItemModel(0, Funcblocks.ModelFieldsCount());

    Funcblocks.GUI-> DataToModelTree(model, "funcblocksTree");
        // attach model
    ui->treeView->setModel(model);

    // Funcblocks.GUI-> SetViewWidths(ui->treeView);

    ui->treeView->expandAll();
    */

    // graphDB.Connect();


    // Funcblocks.getGUIinfo();

    // Projects.Connect(graphDB, "projects");

    if (! model)
        model = new QStandardItemModel(0, Funcblocks.ModelFieldsCount());
}

void FuncBlocksForm::loadFuncblocks()
{
    // IC RootCond = IC("odb_pit", EQ, projectID);

    // Projects.index_tree-> clear();
    // Projects.index_tree-> AddNode(NULL, true, false, EQ, "odb_pit", projectID);  // FIXME STUB

    // Funcblocks.Connect(graphDB, "funcblocks");
    // Projects.Connect(graphDB, "projects");

    // Projects.LoadData("odb_pit", EQ, projectID);


    // Funcblocks.LoadAllData(); // STUB

    // Projects.LoadLinks();

    Funcblocks.Connect(calleeForm-> graphDB, "funcblocks");

    funcblocksOfProject.Connect(calleeForm-> graphDB, "projects_funcblocks", calleeForm-> Projects, Funcblocks);
    funcblocksTree.Connect(calleeForm-> graphDB, "funcblocksTree", Funcblocks, Funcblocks);

    // Funcblocks.ConnectLinkType("projects_funcblocks");
    // Funcblocks.ConnectLinkType("funcblocksTree");

    // funcblocksOfProject.LoadLinkedNodes(projectID);
    // funcblocksTree.LoadLinksFromAllNodes();
    // funcblocksTree.ResolveLinksToPointers();

    Funcblocks.LoadLinkedData("projects_funcblocks", projectID);

    Funcblocks.LoadLinkType("funcblocksTree");
    Funcblocks.myLinkTypes["funcblocksTree"]-> ResolveLinksToPointers();

    // calleeForm-> Projects.LoadLinkType("projects_funcblocks");   // to add new linked fblocks (?) check if is required
    // calleeForm-> Projects.myLinkTypes["projects_funcblocks"]-> ResolveLinksToPointers(); // (?) check if is required


/*
    qDebug() << "project ID = " << project_id << FN;
    qDebug() << "projects count = " << Projects.dataNodes.count() << FN;
    qDebug() << "funcblocks count = " << Funcblocks.dataNodes.count() << FN;
    */



    Funcblocks.GUI-> DataToModelTree(model, "funcblocksTree");

        // attach model
    ui->treeView->setModel(model);

    // Funcblocks.GUI-> SetViewWidths(ui->treeView);

    ui->treeView->expandAll();
}

void FuncBlocksForm::on_treeView_clicked(const QModelIndex &index)
{
        Funcblocks.GUI-> model_current_row = index.row();

        Funcblocks.GUI-> model_current_item = model-> itemFromIndex(index);

        // qDebug() << "model_current_item->column() = " << Funcblocks.GUI-> model_current_item->column() << FN;
}

inline void FuncBlocksForm::InitFuncblockForm()   // project details form setup
{
    funcBlockForm = new FuncblockForm;
    funcBlockForm-> main_callee = this;
    funcBlockForm-> FuncBlocks = &Funcblocks;
    // funcBlockForm-> Projects = &Projects;
    // funcBlockForm-> Statuses = &Statuses;
    // funcBlockForm-> Owners = &Owners;
    funcBlockForm-> initFuncBlock();
}

void FuncBlocksForm::addSubBlock()
{
    if (! Funcblocks.GUI-> model_current_item)
        return;

    isTop = false;

        // open fucblock form

    if (! funcBlockForm)
        InitFuncblockForm();

    funcBlockForm-> FuncBlockID = 0;

    funcBlockForm-> openFuncBlock();
    funcBlockForm-> show();
}

void FuncBlocksForm::addTopBlock()
{
    isTop = true;

        // open fucblock form

    if (! funcBlockForm)
        InitFuncblockForm();

    funcBlockForm-> FuncBlockID = 0;

    funcBlockForm-> openFuncBlock();
    funcBlockForm-> show();
}

void FuncBlocksForm::refreshView()
{
    QStandardItem* parentItem;
    // EgDataNode theFuncBlock;

    // qDebug() << "funcBlockForm-> FuncBlockID = " << funcBlockForm-> FuncBlockID << FN;

        // process return code

    if (funcBlockForm-> FuncBlockID)
    {
        // theFuncBlock = Funcblocks[funcBlockForm-> FuncBlockID];

        // qDebug() << "name = " << theFuncBlock["name"].toString() << FN;

        // FIXME add links


        // calleeForm-> Projects.AddArrowLink("projects_funcblocks", projectID, Funcblocks, funcBlockForm-> FuncBlockID);

        funcblocksOfProject.AddArrowLink(projectID, funcBlockForm-> FuncBlockID);

        funcblocksOfProject.StoreLinks();

        // calleeForm-> Projects.StoreLinkType("projects_funcblocks");

        if (isTop)
        {
            Funcblocks.AddEntryNode(funcBlockForm-> FuncBlockID);
        }
        else
        {
            // Funcblocks.AddArrowLink("funcblocksTree", Funcblocks.GUI-> model_current_item-> data(data_id).toInt(),
            //                        Funcblocks, funcBlockForm-> FuncBlockID);

            funcblocksTree.AddArrowLink(Funcblocks.GUI-> model_current_item-> data(data_id).toInt(), funcBlockForm-> FuncBlockID);

        }

            // show new item
        if (isTop)
            parentItem = model-> invisibleRootItem();
        else
            parentItem = Funcblocks.GUI-> model_current_item;

        QList<QStandardItem*> items;

        items << new QStandardItem(Funcblocks[funcBlockForm-> FuncBlockID]["name"].toString());

        items[0]->setData(QVariant(isUnchanged), data_status);     // loaded data status
        items[0]->setData(QVariant(funcBlockForm-> FuncBlockID), data_id);                    // ID

        parentItem-> appendRow(items);

        if (! isTop)
            ui->treeView-> expand(parentItem->index());
    }
}

void FuncBlocksForm::refreshView2()
{
    /*
        // update link
    Projects.myLinkTypes["projects_funcblocks"]-> UpdateLinkIndex(projectID, oldOffset, Funcblocks[funcBlockForm-> FuncBlockID].dataFileOffset);

    // if (Funcblocks.GUI-> model_current_item-> parent())
    //    qDebug() << "Not top item, " << FN;

    // qDebug() << "oldOffset = " << oldOffset << ", link name = " << Projects.myLinkTypes["projects_funcblocks"]-> linkName << FN;

    if (Funcblocks.GUI-> model_current_item-> parent())
        Funcblocks.myLinkTypes["funcblocksTree"]-> UpdateLinkIndex(Funcblocks.GUI-> model_current_item-> parent()-> data(data_id).toInt(),
                                                                   oldOffset, Funcblocks[funcBlockForm-> FuncBlockID].dataFileOffset);

*/
    Funcblocks.GUI-> model_current_item->setText(Funcblocks[funcBlockForm-> FuncBlockID]["name"].toString());

    // Funcblocks.GUI-> DataToModelTree(model, "funcblocksTree");

        // attach model
    // ui->treeView->setModel(model);

    // Funcblocks.GUI-> SetViewWidths(ui->treeView);

    // ui->treeView->expandAll();
}


void FuncBlocksForm::okExit()
{
    // Funcblocks.LoadData();
    // Funcblocks.PrintObjData();

    // calleeForm-> Projects.StoreLinkType("projects_funcblocks");
    Funcblocks.StoreLinkType("funcblocksTree");
        // exit
    close();
}

void FuncBlocksForm::cancelExit()
{
        // exit
    close();
}


void FuncBlocksForm::on_editButton_clicked()
{
    if (! Funcblocks.GUI-> model_current_item)
        return;

    if (! funcBlockForm)
        InitFuncblockForm();

    funcBlockForm-> FuncBlockID = Funcblocks.GUI-> model_current_item-> data(data_id).toInt();

    oldOffset = Funcblocks[funcBlockForm-> FuncBlockID].dataFileOffset;

    funcBlockForm-> openFuncBlock();
    funcBlockForm-> show();
}


/*
void FuncBlocksForm::editSubBlock()
{

}

void FuncBlocksForm::deleteSubBlock()
{

}

void FuncBlocksForm::openCompForm()
{

}
*/
