#ifndef REFERENCES_H
#define REFERENCES_H

#include <QWidget>

namespace Ui {
class References;
}

class References : public QWidget
{
    Q_OBJECT

public:
    explicit References(QWidget *parent = 0);
    ~References();

private:
    Ui::References *ui;
};

#endif // REFERENCES_H
