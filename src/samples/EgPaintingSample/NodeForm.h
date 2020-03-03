/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2020 Dmitry 'Norian' Solodkiy
 *
 * License: defined in license.txt file located in the root sources dir
 *
 */

// #ifndef PROJECT_FORM_H
// #define PROJECT_FORM_H

#pragma once

#include <QDialog>

#include "ui_NodeForm.h"
#include "../../egDataNodesType.h"

// Qt form data node operations
enum formMode
{
    formModeAdd,
    formModeEdit,
    formModeDelete
};

class GraphSceneForm;

class NodeForm :  public QDialog
{
    Q_OBJECT

public:
    NodeForm(QWidget *parent = 0);

    GraphSceneForm* mainCallee;

    EgDataNodeIdType nodeID;
    formMode theFormMode;

    void openNode();

    EgDataNode newProject;

    EgDataNodesType* Nodes;
    EgDataNode theNode;

private slots:

    void okExit();
    void cancelExit();

private:
    Ui::NodeForm *ui;
};

// #endif // PROJECT_FORM_H
