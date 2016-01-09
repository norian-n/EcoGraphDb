#ifndef OBJDB_FILTERS_H
#define OBJDB_FILTERS_H

#include <QObject>
#include "filter_interface.h"
#include "datadesc.h"

class FilterPlugin : public QObject, FilterInterface
{
    Q_OBJECT
    Q_INTERFACES(FilterInterface)

public:
    void PingFilter();
    FilterCallbackType FilterByID(qint32 filter_id);
};

#endif // OBJDB_FILTERS_H
