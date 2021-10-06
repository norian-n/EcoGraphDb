#ifndef TEMPLATESDRAGWIDGET_H
#define TEMPLATESDRAGWIDGET_H

#include <QFrame>

class EgGraphForm;

class TemplatesDragWidget : public QFrame
{
public:
    explicit TemplatesDragWidget(QWidget *parent = nullptr);

    EgGraphForm* myForm = nullptr;

protected:

    /*
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    */

    void mousePressEvent(QMouseEvent *event) override;

    // void paintEvent(QPaintEvent *) override;
};


#endif // TEMPLATESDRAGWIDGET_H
