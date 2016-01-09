#ifndef META_INFO_H
#define META_INFO_H

#include "egDataNode.h"

class EgGraphDatabase;

class EgDataNodeTypeMetaInfo
{
public:
    EgDataNodeIDtype typeID;            // data class ID
    QString typeName;

    EgDataNodeIDtype objCount;          // data objects count
    EgDataNodeIDtype nextObjID;         // next available data object ID

    EgGraphDatabase* myECoGraphDB;

    QList<QString> dataFields;          // fields of DataNodeType

    QHash<QString, int> nameToOrder;    // map field names to data list order (dont mesh up with data index)
    QHash<QString, int> indexedToOrder; // map indexed fields names to data list order (dont mesh up with data index)

    EgDataNodeTypeMetaInfo(): typeName("Error_no_type_name"), objCount(0), nextObjID(1), myECoGraphDB(NULL) {}
    EgDataNodeTypeMetaInfo(QString a_typeName) : typeName(a_typeName), objCount(0), nextObjID(1), myECoGraphDB(NULL)  {}

    ~EgDataNodeTypeMetaInfo() {}

    void Clear() {objCount = 0; nextObjID = 1; dataFields.clear(); /*indexedFields.clear();*/ nameToOrder.clear(); indexedToOrder.clear();}

    void AddDataField(QString fieldName, bool indexed = false);    // add field descriptor, no GUI control data

    int  LocalStoreMetaInfo();               // save to file or server
    int  LocalLoadMetaInfo();                // load from file or server

    void PrintMetaInfo();               // debug print of field descriptions

};

// QDataStream& operator << (QDataStream& dStream, EgDataNodeTypeMetaInfo& metaInfo);
// QDataStream& operator >> (QDataStream& dStream, EgDataNodeTypeMetaInfo& metaInfo);


#endif // META_INFO_H
