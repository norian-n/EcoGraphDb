#include "NodeForm.h"
#include "GraphSceneForm.h"

#include <QtDebug>

NodeForm::NodeForm(QWidget *parent): QWidget(parent)
  , ui(new Ui::NodeForm)

{
    ui->setupUi(this);
        // connect buttons
    connect(ui->cancelButton, SIGNAL(clicked()),this, SLOT(cancelExit()));
    connect(ui->okButton, SIGNAL(clicked()),this, SLOT(okExit()));
}

void NodeForm::initProject()
{
    theProject.extraInfo = &(Projects-> extraInfo);
        // init combos
}

void NodeForm::openProject()
{
        // check mode
    if (formMode != formModeAdd) // (projectID != 0)    // edit data object
    {
        // theProject = (*Projects)[projectID];

        ui->nameEdit->setText((*Projects)[projectID]["name"].toString());
        ui->descEdit->setText((*Projects)[projectID]["description"].toString());

        ui->idLabel->setText(QVariant((*Projects)[projectID].dataNodeID).toString());
    }
    else    // add new
    {
        ui->nameEdit->clear();
        ui->descEdit->clear();
        ui->idLabel->clear();
    }
    ui->nameEdit->setFocus();

// #undef FIELD

}

void NodeForm::okExit()
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

            // check for modified fields
        if (theProject.dataFields != (*Projects)[projectID].dataFields)
        {
            Projects-> UpdateDataNode(theProject.dataFields, projectID);
                // save data
            Projects-> StoreData();
                // update parent view
            if (main_callee)
            {
                main_callee->ShowGraphNodes();
                main_callee->ShowGraphLinks();
            }
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

        Projects-> AddDataNode(theProject.dataFields);
            // save data
        Projects->StoreData();
            // update parent view
        if (main_callee)
        {
            main_callee->ShowGraphNodes();
            main_callee->ShowGraphLinks();
        }
    }
    else if (formMode == formModeDelete) // delete project
    {
        Projects-> DeleteDataNode(projectID);

        Projects->StoreData();
            // update parent view
        if (main_callee)
        {
            main_callee->ShowGraphNodes();
            main_callee->ShowGraphLinks();
        }
    }
        // clean up
    theProject.dataFields.clear();
    // delete theProject;
        // exit
    close();
}

void NodeForm::cancelExit()
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

