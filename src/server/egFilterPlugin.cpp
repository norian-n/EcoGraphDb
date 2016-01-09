#include <QtGui>
#include <QtDebug>

#include "egFilterPlugin.h"


int FilterOwner (QList<QVariant>& obj_fields_values, QList<QVariant>& filter_values, QHash<QString, int>& obj_field_indexes)
{
      // qDebug() << "FilterTest1(): callback called";

      if ((obj_fields_values.count() >  obj_field_indexes["owner"]) && filter_values.count()) // filter value set assert
      {
          if (obj_fields_values[ obj_field_indexes["owner"] ] == filter_values[0])  // where owner.id == 1
              return 1; // good data
      }

      return 0; // bad data
}

int FilterStatus (QList<QVariant>& obj_fields_values, QList<QVariant>& filter_values, QHash<QString, int>& obj_field_indexes)
{
      // qDebug() << "FilterTest2(): callback called";

      if ((obj_fields_values.count() >  obj_field_indexes["status"]) && filter_values.count()) // filter value set assert
      {
          if (obj_fields_values[ obj_field_indexes["status"] ] == filter_values[0])  // where status.id == 1
              return 1; // good data
      }

      return 0; // bad data
}

int FilterName (QList<QVariant>& obj_fields_values, QList<QVariant>& filter_values, QHash<QString, int>& obj_field_indexes)
{
      // qDebug() << "FilterTest2(): callback called";

      if ((obj_fields_values.count() >  obj_field_indexes["name"]) && filter_values.count()) // filter value set assert
      {
          if (obj_fields_values[ obj_field_indexes["name"] ] == filter_values[0])  // where status.id == 1
              return 1; // good data
      }

      return 0; // bad data
}


FilterCallbackType EgFilterPlugin::FilterByID(qint32 filter_id)
{
    switch ( filter_id )
    {
    case 1:
        // qDebug() << "FilterByID(): FilterTest1 callback to return" << (int)&FilterTest1;
        return &FilterOwner;
        break;
    case 2:
         // qDebug() << "FilterByID(): FilterTest2 callback to return " << (int)&FilterTest2;
        return &FilterStatus;
        break;
    case 3:
         // qDebug() << "FilterByID(): FilterTest3 callback to return " << (int)&FilterTest3;
        return &FilterName;
        break;
    default:
        return 0;   // no callback
    }
}

void EgFilterPlugin::PingFilter()
{
    qDebug() << "PingFilter(): plugin is alive";
}

Q_EXPORT_PLUGIN2(EgFilterPlugin, EgFilterPlugin);

