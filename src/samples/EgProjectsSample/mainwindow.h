/*
 * EcoGraphDB - Exo Cortex Graph Database Engine
 *
 * Copyright (c) 2016 Dmitry 'Norian' Solodkiy
 *
 * License: propietary open source, free for non-commercial applications
 *
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QStandardItemModel>
#include <QFile>
#include <QTcpSocket>

#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"

#include "projects.h"

#include "ui_mainwindow.h"

// static QString objdb_global_string;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

    EgGraphDatabase graphDB;

    EgDataNodesType Projects;
    EgDataNodesType Statuses;
    EgDataNodesType Owners;
    EgDataNodesType Funcblocks;

    EgLinkType funcblocksTree;
    EgLinkType funcblocksOfProject;

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    Ui::MainWindow* ui;

    ProjectsForm* projectsForm;
    // inline void InitProjectForm();

    void FillTestData();

private slots:

    void on_initButton_clicked();

    void on_projectsButton_clicked();
};

#endif // MAINWINDOW_H
