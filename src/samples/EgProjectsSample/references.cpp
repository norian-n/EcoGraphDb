#include "references.h"
#include "ui_references.h"

References::References(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::References)
{
    ui->setupUi(this);
}

References::~References()
{
    delete ui;
}
