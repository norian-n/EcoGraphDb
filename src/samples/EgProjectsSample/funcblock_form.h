#ifndef FUNCBLOCK_FORM_H
#define FUNCBLOCK_FORM_H

#include "ui_funcblock_form.h"
#include "../../egDataNodesType.h"

class FuncBlocksForm;

/*
class CustomFuncBlock
{
public:

    QString name;
    int status;
    QString owner;
    QDate launch_date;
    QDate end_date;
    QString description;
};
*/

class FuncblockForm :  public QWidget
{
    Q_OBJECT

public:
    FuncblockForm(QWidget *parent = 0);

    FuncBlocksForm* main_callee;
//    SubProjectsForm * sub_callee;
    int FuncBlock_id;

    void initFuncBlock();
    void openFuncBlock();
        // primary table
    /*

    DataObjects* FuncBlocks;
    DataObj TheFuncBlock; // local ptr, move to
        // references
    DataObjects* Statuses;
    DataObjects* Owners;
    */
    EgDataNode newFuncBlock;

    EgDataNodesType* FuncBlocks;
    EgDataNode theFuncBlock; // update or delete

        // references
    EgDataNodesType* Statuses;
    EgDataNodesType* Owners;

    // CustomFuncBlock cp;

    // QList<QVariant> new_fields;

private slots:
    // void on_inputSpinBox1_valueChanged(int value);
    // void on_inputSpinBox2_valueChanged(int value);
    // void addFuncBlock();
    // void deleteSubFuncBlock();
    void okExit();
    void cancelExit();

private:
    Ui::FuncblockForm *ui;
};

#endif // FUNCBLOCK_FORM_H
