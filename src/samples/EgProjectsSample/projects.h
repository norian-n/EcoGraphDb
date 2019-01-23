/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef PROJECTS_H
#define PROJECTS_H


#include <QStandardItemModel>
#include <QFile>
#include <QTcpSocket>

#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"

#include "ui_projects.h"
#include "project_form.h"
#include "funcblocks.h"

#include "ui_mainwindow.h"

// static QString objdb_global_string;

class ProjectsForm : public QWidget
{
    Q_OBJECT
public:
    ProjectsForm(QWidget* parent = 0);
    ~ProjectsForm();

    EgGraphDatabase graphDB;

    EgDataNodesType Projects;
    EgDataNodesType Statuses;
    EgDataNodesType Owners;
    EgDataNodesType Funcblocks;


    QStandardItemModel* model;

    // int FilterTest (QList<QVariant>& my_data); // filter callback
    // static void TestFunc(); //  { Owners.DClassName;};
    void refreshView();

    // QTcpSocket *tcpSocket;
    // QString currentFortune;
    // quint16 blockSize;

    // EgRemoteConnect test_server;

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::ProjectsForm* ui;
    int added_row; // new record auto-gen number

    QStandardItemModel* linked_model;

    ProjectForm* project_form;
    FuncBlocksForm* funcblocks_form;

    inline void InitProjectForm();

private slots:
    void model_data_changed(const QModelIndex&, const QModelIndex&);
    void on_tableView_clicked(QModelIndex index);
    void on_deleteButton_clicked();
    void on_addButton_clicked();
    void on_addProjectButton_clicked();
    void on_editProjectButton_clicked();
};

#endif // PROJECTS_H
