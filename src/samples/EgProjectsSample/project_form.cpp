#include "project_form.h"
#include "projects.h"

#include <QtDebug>

ProjectForm::ProjectForm(QWidget *parent): QWidget(parent)
  , ui(new Ui::ProjectForm)

{
    ui->setupUi(this);
        // connect buttons
    connect(ui->cancelButton, SIGNAL(clicked()),this, SLOT(cancelExit()));
    connect(ui->okButton, SIGNAL(clicked()),this, SLOT(okExit()));
}

void ProjectForm::initProject()
{
    theProject.metaInfo = &(Projects-> metaInfo);
        // init combos
    Owners->  GUI-> FillComboBox(ui->ownerBox);
    Statuses-> GUI-> FillComboBox(ui->statusBox);
}

void ProjectForm::openProject()
{
        // check mode
    if (formMode != formModeAdd) // (projectID != 0)    // edit data object
    {
        // theProject = (*Projects)[projectID];

        ui->nameEdit->setText((*Projects)[projectID]["name"].toString());
        ui->descEdit->setText((*Projects)[projectID]["description"].toString());

        Owners-> GUI-> SetComboBox(ui->ownerBox, (*Projects)[projectID]["owner"]);
        Statuses-> GUI-> SetComboBox(ui->statusBox, (*Projects)[projectID]["status"]);

        ui->startDateEdit->setDate((*Projects)[projectID]["launch_date"].toDate());
        ui->completeDateEdit->setDate((*Projects)[projectID]["end_date"].toDate());

        ui->idLabel->setText(QVariant((*Projects)[projectID].dataNodeID).toString());
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

void ProjectForm::okExit()
{
        // check mandatory fields
    if (ui->nameEdit->text().isEmpty())
    {
        ui->nameEdit->setFocus();
        return;
    }
        // init
    // theProject = new DataObj();


    if (formMode == formModeEdit) // edit project
    {
        theProject.dataFields.clear();
        for (int k = 0; k < Projects->FieldsCount(); k++)
            theProject.dataFields << QVariant();   // FIXME theProject.Init(); or clearData();

            // update data fields
        theProject["name"]        = ui->nameEdit->text();
        theProject["description"] = ui->descEdit->text();

        theProject["status"]      = Statuses-> GUI-> GetComboBoxID(ui->statusBox);
        theProject["owner"]       = Owners-> GUI-> GetComboBoxID(ui->ownerBox);

        if (ui->startDateEdit->date() != QDate(2000,01,01))
            theProject["launch_date"] = ui->startDateEdit->date();
        else
            theProject["launch_date"] = QDate();

        if (ui->completeDateEdit->date() != QDate(2000,01,01))
            theProject["end_date"] = ui->completeDateEdit->date();
        else
            theProject["end_date"] = QDate();

            // check for modified fields
        if (theProject.dataFields != (*Projects)[projectID].dataFields)
        {
            Projects-> UpdateDataNode(theProject.dataFields, projectID);
                // save data
            Projects-> StoreData();
                // update parent view
            if (main_callee)
                main_callee->refreshView();
        }
    }
    else if (formMode == formModeAdd) // insert project
    {
        theProject.dataFields.clear();
        for (int k = 0; k < Projects->FieldsCount(); k++)
            theProject.dataFields << QVariant();   // FIXME theProject.Init(); or clearData();

            // update data fields
        theProject["name"]        = ui->nameEdit->text();
        theProject["description"] = ui->descEdit->text();

        theProject["status"]      = Statuses-> GUI-> GetComboBoxID(ui->statusBox);
        theProject["owner"]       = Owners-> GUI-> GetComboBoxID(ui->ownerBox);

        if (ui->startDateEdit->date() != QDate(2000,01,01))
            theProject["launch_date"] = ui->startDateEdit->date();
        else
            theProject["launch_date"] = QDate();

        if (ui->completeDateEdit->date() != QDate(2000,01,01))
            theProject["end_date"] = ui->completeDateEdit->date();
        else
            theProject["end_date"] = QDate();

        Projects-> AddDataNode(theProject.dataFields);
            // save data
        Projects->StoreData();
            // update parent view
        if (main_callee)
            main_callee->refreshView();
    }
    else if (formMode == formModeDelete) // delete project
    {
        Projects-> DeleteDataNode(projectID);

        Projects->StoreData();
            // update parent view
        if (main_callee)
            main_callee->refreshView();
    }
        // clean up
    theProject.dataFields.clear();
    // delete theProject;
        // exit
    close();
}

void ProjectForm::cancelExit()
{
        // clean up
    theProject.dataFields.clear();

    close();
}

// custom project operatons
/*
cp.name = (*theProject)["name"].toString();
cp.owner = (*Owners)[(*theProject)["owner"].toInt()]["login"].toString();
cp.status = (*theProject)["status"].toInt();
cp.description = (*theProject)["description"].toString();

qDebug() << FN << cp.name << cp.owner << cp.status << cp.description;
*/


// #define FIELD(f_name) new_fields[Projects->FD[f_name]]
// #define FIELD(f_name) (*theProject)[f_name]
// #undef FIELD

// qDebug() << FN << theProject->data_fields;
// qDebug() << FN << (*Projects)[projectID].data_fields;
// qDebug() << FN << "not equal";

/*
    new_fields.clear();
    for (int k = 0; k < Projects->FieldsCount(); k++)
        new_fields << QVariant();
        */

// Projects->AddDataNode(new_fields);
// #define FIELD(f_name) (*Projects)[projectID][f_name]

