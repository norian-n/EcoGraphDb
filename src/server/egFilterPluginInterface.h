#ifndef EG_FILTER_PLUGIN_INTERFACE_H
#define EG_FILTER_PLUGIN_INTERFACE_H

#include "egCore.h"

class EgFilterPluginInterface
{
public:
    virtual ~EgFilterPluginInterface() {}
    virtual void PingFilter() = 0;
    virtual FilterCallbackType FilterByID(qint32 filter_id) = 0;
};


QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(EgFilterPluginInterface, "Plugin.EgFilterPluginInterface/1.0")
QT_END_NAMESPACE


#endif // EG_FILTER_PLUGIN_INTERFACE_H
