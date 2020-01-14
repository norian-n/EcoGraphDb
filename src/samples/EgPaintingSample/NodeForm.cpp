#include "NodeForm.h"
#include "GraphSceneForm.h"

#include <QtDebug>

NodeForm::NodeForm(QWidget *parent): QDialog(parent)
  , ui(new Ui::NodeForm)

{
    ui->setupUi(this);
        // connect buttons
    connect(ui->cancelButton, SIGNAL(clicked()),this, SLOT(cancelExit()));
    connect(ui->okButton, SIGNAL(clicked()),this, SLOT(okExit()));
}

void NodeForm::openNode()
{
        // check mode
    if (theFormMode != formModeAdd) // (projectID != 0)    // edit data object
    {
        // theProject = (*Projects)[projectID];

        ui->nameEdit->setText(mainCallee-> nodes[nodeID]["name"].toString());
        // ui->descEdit->setText(mainCallee-> nodes[nodeID]["description"].toString());

        ui->idLabel->setText(QVariant(mainCallee-> nodes[nodeID].dataNodeID).toString());
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


    if (theFormMode == formModeEdit) // edit project
    {
        mainCallee-> nodes[nodeID]["name"] = ui->nameEdit->text();
        // mainCallee-> nodes[nodeID]["description"] = ui->descEdit->text();

        mainCallee-> nodes.UpdateDataNode(nodeID); // mark as updated
        // mainCallee-> nodes.StoreData();

        mainCallee-> ShowGraphNodes();
        mainCallee-> ShowGraphLinks();
    }
    /*
    else if (theFormMode == formModeAdd) // insert project
    {
        theNode.dataFields.clear();
        for (int k = 0; k < Nodes->FieldsCount(); k++)
            theNode.dataFields << QVariant();   // FIXME theProject.Init(); or clearData();

            // update data fields
        theNode["name"]        = ui->nameEdit->text();
        theNode["description"] = ui->descEdit->text();

        Nodes-> AddDataNode(theNode.dataFields);
            // save data
        Nodes->StoreData();
            // update parent view
        if (mainCallee)
        {
            mainCallee->ShowGraphNodes();
            mainCallee->ShowGraphLinks();
        }
    }
    */
    else if (theFormMode == formModeDelete) // delete project
    {
        mainCallee-> nodes.DeleteLinksOfNode(nodeID);
        mainCallee-> nodes.DeleteDataNode(nodeID);
        // mainCallee-> nodes.StoreData();

            // update parent view
        mainCallee->ShowGraphNodes();
        mainCallee->ShowGraphLinks();
    }
        // clean up
    // theNode.dataFields.clear();
    // delete theProject;
        // exit
    close();
}

void NodeForm::cancelExit()
{
        // clean up
    theNode.dataFields.clear();

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

