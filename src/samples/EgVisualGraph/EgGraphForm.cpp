#include "EgGraphForm.h"
#include "ui_EgGraphForm.h"

EgGraphForm::EgGraphForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EgGraphForm)
{
    ui->setupUi(this);

    verticalLayout = new QVBoxLayout(this);

    DragWidget = new TemplatesDragWidget();

    DragWidget-> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    DragWidget-> setAcceptDrops(false);

    DropWidget = new EgGraphWidget();

    DropWidget-> setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    DropWidget-> setAcceptDrops(true);

    // DropWidget-> setMinimumSize(600, 400);
    // DropWidget-> resize(1000, 500); // process real size on loading // DropWidget-> resize(1000, 500);

    scrollArea1 = new QScrollArea();

    scrollArea1-> setWidget(DropWidget);
    scrollArea1-> setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea1-> setWidgetResizable(true);

    scrollArea1-> setMinimumSize(300, 300);

    spacer1 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    spacer2 = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    verticalLayout-> addWidget(DragWidget);

    verticalLayout->addItem(spacer1);

    verticalLayout-> addWidget(scrollArea1);

    verticalLayout->addItem(spacer2);

    verticalLayout-> addWidget(ui->groupBox);
}

EgGraphForm::~EgGraphForm()
{
    delete ui;

    delete DragWidget;
    delete scrollArea1;

    delete verticalLayout;
}

