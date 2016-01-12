#ifndef FUNCBLOCKS_H
#define FUNCBLOCKS_H

#include "ui_funcblocks.h"
#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"

class FuncBlocksForm : public QWidget
{
    Q_OBJECT
public:
    FuncBlocksForm(QWidget *parent = 0);

private slots:
    void addSubBlock();
    void addTopBlock();
    void editSubBlock();
    void deleteSubBlock();
    void openCompForm();
    void okExit();
    void cancelExit();

private:
    Ui::FuncBlocksForm* ui;
/*
    DataObjects Funcblocks;
    DataObjects Statuses;
    DataObjects Owners;
    */

    EgGraphDatabase graphDB;

    EgDataNodesType Funcblocks;
    EgDataNodesType Statuses;
    EgDataNodesType Owners;

    // LinkData* FuncblockStatusLink;
    // LinkData* FuncblockOwnerLink;

    QStandardItemModel* model;

};


#endif // FUNCBLOCKS_H
