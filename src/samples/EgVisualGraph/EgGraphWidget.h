#ifndef EGGRAPHWIDGET_H
#define EGGRAPHWIDGET_H

#include <QFrame>

class EgGraphForm;

class EgGraphWidget : public QFrame
{
public:
    explicit EgGraphWidget(QWidget *parent = nullptr);

    EgGraphForm* myForm = nullptr;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    // void paintEvent(QPaintEvent *) override;
};

#endif // EGGRAPHWIDGET_H
