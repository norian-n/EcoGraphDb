#ifndef EG_FILTERPLUGIN_H
#define EG_FILTERPLUGIN_H

#include <QObject>
#include "egFilterPluginInterface.h"


class EgFilterPlugin : public QObject, EgFilterPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(EgFilterPluginInterface)

public:
    void PingFilter();
    FilterCallbackType FilterByID(qint32 filter_id);
};

#endif // EGFILTERPLUGIN_H
