#include "TemplatesDragWidget.h"

#include <QtWidgets>
#include <QSizePolicy>

TemplatesDragWidget::TemplatesDragWidget(QWidget *parent)
    : QFrame(parent)
{
    setMinimumSize(100, 80);
    resize(200, 80);

    setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
    // setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);  // Fixed

    // setAcceptDrops(true);

    QImage image(84,64, QImage::Format_ARGB32);
    image.fill(qRgba(0, 0, 0, 0));

    QPainter painter(&image);

    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 10, QFont::Bold));
    painter.drawText(5, 20, "Node");
    painter.drawRoundedRect(2,2,80,60, 6,6);
    painter.end();

    QLabel* imageIcon = new QLabel(this);
    imageIcon->setPixmap(QPixmap::fromImage(image));
    imageIcon->move(10, 10);
    imageIcon->show();
    imageIcon->setAttribute(Qt::WA_DeleteOnClose);

    imageIcon-> setProperty("egDbID", 101);  // FIXME STUB

    /*

    QLabel *boatIcon = new QLabel(this);

    boatIcon->setPixmap(QPixmap(":/images/boat.png"));
    boatIcon->move(10, 10);
    boatIcon->show();
    boatIcon->setAttribute(Qt::WA_DeleteOnClose);

    QLabel *carIcon = new QLabel(this);
    carIcon->setPixmap(QPixmap(":/images/car.png"));
    carIcon->move(100, 10);
    carIcon->show();
    carIcon->setAttribute(Qt::WA_DeleteOnClose);

    */
}

void TemplatesDragWidget::mousePressEvent(QMouseEvent *event)
{
    QLabel *child = static_cast<QLabel*>(childAt(event->pos()));
    if (!child)
        return;

    QPixmap pixmap = *child->pixmap();

    QByteArray itemData;
    QDataStream dataStream(&itemData, QIODevice::WriteOnly);
    dataStream << pixmap << QPoint(event->pos() - child->pos());
//! [1]

//! [2]
    QMimeData *mimeData = new QMimeData;
    mimeData->setData("application/x-dnditemdata", itemData);
//! [2]

//! [3]
    QDrag *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(pixmap);
    drag->setHotSpot(event->pos() - child->pos());
//! [3]

    QPixmap tempPixmap = pixmap;
    QPainter painter;
    painter.begin(&tempPixmap);
    painter.fillRect(pixmap.rect(), QColor(63, 63, 63, 32)); //  QColor(127, 127, 127, 127));
    painter.end();

    child->setPixmap(tempPixmap);

    // qDebug() << "ID: " << child-> property("egDbID").toInt();

/*
    myForm-> dragDropAction = true;
    drag-> exec(Qt::CopyAction);
    myForm-> dragDropAction = false;

*/


    if (drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction) == Qt::MoveAction) {
        child->close();
    } else {
        child->show();
        child->setPixmap(pixmap);
    }
}

/*
void TemplatesDragWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void TemplatesDragWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}

void TemplatesDragWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasFormat("application/x-dnditemdata")) {
        QByteArray itemData = event->mimeData()->data("application/x-dnditemdata");
        QDataStream dataStream(&itemData, QIODevice::ReadOnly);

        QPixmap pixmap;
        QPoint offset;
        dataStream >> pixmap >> offset;

        QLabel *newIcon = new QLabel(this);
        newIcon->setPixmap(pixmap);
        newIcon->move(event->pos() - offset);
        newIcon->show();
        newIcon->setAttribute(Qt::WA_DeleteOnClose);

        if (event->source() == this) {
            event->setDropAction(Qt::MoveAction);
            event->accept();
        } else {
            event->acceptProposedAction();
        }
    } else {
        event->ignore();
    }
}
*/
