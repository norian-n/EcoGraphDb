#ifndef EGGRAPHFORM_H
#define EGGRAPHFORM_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>

#include "../../egDataNodesType.h"
#include "../../egGraphDatabase.h"

#include "NodeForm.h"
#include "TemplatesDragWidget.h"
#include "EgGraphWidget.h"

namespace Ui {
class EgGraphForm;
}

class EgGraphForm : public QWidget
{
    Q_OBJECT

public:

    EgDataNodesType nodes;
    EgDataNodesType images;

    EgLinkType linktype;


    explicit EgGraphForm(QWidget *parent = nullptr);
    ~EgGraphForm();

    // void LoadImages();

    void ShowGraphNodes() {} //;
    void ShowGraphLinks() {} //;

private:
    Ui::EgGraphForm * ui;

    QVBoxLayout* verticalLayout;

    TemplatesDragWidget* DragWidget;
    EgGraphWidget* DropWidget;

    QScrollArea* scrollArea1;

    QSpacerItem* spacer1;
    QSpacerItem* spacer2;

};

#endif // EGGRAPHFORM_H
