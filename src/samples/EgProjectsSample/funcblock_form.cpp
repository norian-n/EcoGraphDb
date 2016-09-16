#include "funcblock_form.h"
#include "funcblocks.h"

#include <QtDebug>
// #include <QList>

FuncblockForm::FuncblockForm(QWidget *parent): QWidget(parent)
  , ui(new Ui::FuncblockForm)

{
    ui->setupUi(this);
        // connect buttons
    connect(ui->cancelButton, SIGNAL(clicked()),this, SLOT(cancelExit()));
    connect(ui->okButton, SIGNAL(clicked()),this, SLOT(okExit()));
}

void FuncblockForm::initFuncBlock()
{
        // set field descriptors
    // theFuncBlock.field_descs = &(FuncBlocks->FD);
        // init combos
    // Owners->  GUI.FillComboBox(ui->ownerBox);
    // Statuses-> GUI.FillComboBox(ui->statusBox);
}

void FuncblockForm::openFuncBlock()
{
        // check mode
    if (FuncBlock_id != 0)    // edit data object
    {
        theFuncBlock = (*FuncBlocks)[FuncBlock_id];

        ui->nameEdit->setText(theFuncBlock["name"].toString());
        ui->descEdit->setText(theFuncBlock["description"].toString());

        Owners-> GUI.SetComboBox(ui->ownerBox, theFuncBlock["owner"]);
        Statuses-> GUI.SetComboBox(ui->statusBox, theFuncBlock["status"]);

        ui->startDateEdit->setDate(theFuncBlock["launch_date"].toDate());
        ui->completeDateEdit->setDate(theFuncBlock["end_date"].toDate());

        ui->idLabel->setText(QVariant(theFuncBlock.dataNodeID).toString());
    }
    else    // add new
    {
        ui->nameEdit->clear();
        ui->descEdit->clear();
        ui->idLabel->clear();

        ui->startDateEdit->setDate(QDate::currentDate());
        ui->completeDateEdit->clear();
    }
    ui->nameEdit->setFocus();

// #undef FIELD

}

void FuncblockForm::okExit()
{
        // check mandatory fields
    if (ui->nameEdit->text().isEmpty())
    {
        ui->nameEdit->setFocus();
        return;
    }
        // init
    // theFuncBlock = new DataObj();


    if (FuncBlock_id) // edit FuncBlock
    {
        theFuncBlock.dataFields.clear();
        for (int k = 0; k < FuncBlocks->FieldsCount(); k++)
            theFuncBlock.dataFields << QVariant();   // FIXME theFuncBlock.Init(); or clearData();

            // update data fields
        theFuncBlock["name"]        = ui->nameEdit->text();
        theFuncBlock["description"] = ui->descEdit->text();

        theFuncBlock["status"]      = Statuses-> GUI.GetComboBoxID(ui->statusBox);
        theFuncBlock["owner"]       = Owners-> GUI.GetComboBoxID(ui->ownerBox);

        if (ui->startDateEdit->date() != QDate(2000,01,01))
            theFuncBlock["launch_date"] = ui->startDateEdit->date();
        else
            theFuncBlock["launch_date"] = QDate();

        if (ui->completeDateEdit->date() != QDate(2000,01,01))
            theFuncBlock["end_date"] = ui->completeDateEdit->date();
        else
            theFuncBlock["end_date"] = QDate();

            // check for modified fields
        if (theFuncBlock.dataFields != (*FuncBlocks)[FuncBlock_id].dataFields)
        {
            FuncBlocks->SetModifiedData(theFuncBlock.dataFields, FuncBlock_id);
                // save data
            FuncBlocks->StoreData();
                // update parent view
            if (main_callee)
                main_callee->refreshView();
        }
    }
    else // insert FuncBlock
    {
        theFuncBlock.dataFields.clear();
        for (int k = 0; k < FuncBlocks->FieldsCount(); k++)
            theFuncBlock.dataFields << QVariant();   // FIXME theFuncBlock.Init(); or clearData();

            // update data fields
        theFuncBlock["name"]        = ui->nameEdit->text();
        theFuncBlock["description"] = ui->descEdit->text();

        theFuncBlock["status"]      = Statuses-> GUI.GetComboBoxID(ui->statusBox);
        theFuncBlock["owner"]       = Owners-> GUI.GetComboBoxID(ui->ownerBox);

        if (ui->startDateEdit->date() != QDate(2000,01,01))
            theFuncBlock["launch_date"] = ui->startDateEdit->date();
        else
            theFuncBlock["launch_date"] = QDate();

        if (ui->completeDateEdit->date() != QDate(2000,01,01))
            theFuncBlock["end_date"] = ui->completeDateEdit->date();
        else
            theFuncBlock["end_date"] = QDate();

        FuncBlocks->AddNewData(theFuncBlock.dataFields);
            // save data
        FuncBlocks->StoreData();
            // update parent view
        if (main_callee)
            main_callee->refreshView();
    }
        // clean up
    theFuncBlock.dataFields.clear();
    // delete theFuncBlock;
        // exit
    close();
}

void FuncblockForm::cancelExit()
{
        // clean up
    theFuncBlock.dataFields.clear();

    close();
}

// custom FuncBlock operatons
/*
cp.name = (*theFuncBlock)["name"].toString();
cp.owner = (*Owners)[(*theFuncBlock)["owner"].toInt()]["login"].toString();
cp.status = (*theFuncBlock)["status"].toInt();
cp.description = (*theFuncBlock)["description"].toString();

qDebug() << FN << cp.name << cp.owner << cp.status << cp.description;
*/


// #define FIELD(f_name) new_fields[FuncBlocks->FD[f_name]]
// #define FIELD(f_name) (*theFuncBlock)[f_name]
// #undef FIELD

// qDebug() << FN << theFuncBlock->data_fields;
// qDebug() << FN << (*FuncBlocks)[FuncBlock_id].data_fields;
// qDebug() << FN << "not equal";

/*
    new_fields.clear();
    for (int k = 0; k < FuncBlocks->FieldsCount(); k++)
        new_fields << QVariant();
        */

// FuncBlocks->AddNewData(new_fields);
// #define FIELD(f_name) (*FuncBlocks)[FuncBlock_id][f_name]

