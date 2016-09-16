/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef FUNCBLOCKS_H
#define FUNCBLOCKS_H

#include "ui_funcblocks.h"
#include "funcblock_form.h"

#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"
// #include "../../indexes/egIndexConditions.h"

class FuncBlocksForm : public QWidget
{
    Q_OBJECT
public:

    int project_id;

    FuncBlocksForm(QWidget *parent = 0);

    void loadFuncblocks();
    void refreshView();

private slots:


    void addSubBlock();
    void addTopBlock();
    void editSubBlock();
    void deleteSubBlock();
    void openCompForm();
    void okExit();
    void cancelExit();

    void on_treeView_clicked(const QModelIndex &index);

private:
    Ui::FuncBlocksForm* ui;
/*
    DataObjects Funcblocks;
    DataObjects Statuses;
    DataObjects Owners;
    */    

    EgGraphDatabase graphDB;

    EgDataNodesType Funcblocks;
    EgDataNodesType Projects;

    EgDataNodesType Statuses;
    EgDataNodesType Owners;

    // LinkData* FuncblockStatusLink;
    // LinkData* FuncblockOwnerLink;

    FuncblockForm* funcBlockForm;

    QStandardItemModel* model;

    inline void InitFunkblockForm();

};


#endif // FUNCBLOCKS_H
