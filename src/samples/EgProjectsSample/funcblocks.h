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
#include "projects.h"

#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"
// #include "../../indexes/egIndexConditions.h"

class FuncBlocksForm : public QWidget
{
    Q_OBJECT
public:

    int projectID;

    quint64 oldOffset;

    bool isTop;

    FuncBlocksForm(QWidget *parent = 0);

    ProjectsForm* calleeForm;

    void loadFuncblocks();

    void refreshView();
    void refreshView2();

private slots:

    void addSubBlock();
    void addTopBlock();
    // void editSubBlock();
    // void deleteSubBlock();
    // void openCompForm();
    void okExit();
    void cancelExit();

    void on_treeView_clicked(const QModelIndex &index);

    void on_editButton_clicked();

private:
    Ui::FuncBlocksForm* ui;
/*
    DataObjects Funcblocks;
    DataObjects Statuses;
    DataObjects Owners;
    */    

    // EgGraphDatabase graphDB;

    EgDataNodesType Funcblocks;
    // EgDataNodesType Projects;

    // EgDataNodesType Statuses;
    // EgDataNodesType Owners;

    // LinkData* FuncblockStatusLink;
    // LinkData* FuncblockOwnerLink;

    FuncblockForm* funcBlockForm;



    QStandardItemModel* model;

    inline void InitFuncblockForm();

};


#endif // FUNCBLOCKS_H
