#include "egDataNodesLink.h"
#include "egDataNodesType.h"

/*
EgDataNodesLinkType::EgDataNodesLinkType():
    LinkFileName(left_dc->DClassName+"_"+right_dc->DClassName+".dln")
{
    // constructor
    // qDebug() << "egDataNodesLink(): LinkFileName = " << LinkFileName;
}

*/

int EgDataNodesLinkType::AddLink (EgDataNodeIDtype leftNodeID, EgDataNodeIDtype rightNodeID)
{
    addedLinks.insert(leftNodeID, rightNodeID);

    // qDebug() << linkName << ": link added " << leftNodeID << "to" <<  rightNodeID << FN;

    return 0;
}


int EgDataNodesLinkType::StoreLinks()
{
    QFile dat_file;             // data file
    QDataStream dat;

    if (addedLinks.isEmpty())
        return 1;

    dat_file.setFileName(linkName + ".dln");

    if (!dat_file.open(QIODevice::ReadWrite)) // WriteOnly Append | QIODevice::Truncate
    {
        qDebug() << "can't open file " << linkName + ".dln" << FN;
        return -1;
    }

    dat.setDevice(&dat_file);

    dat.device()-> seek(dat.device()-> size());

        // walk add list
    for (QMultiMap<EgDataNodeIDtype, EgDataNodeIDtype>::iterator addIter = addedLinks.begin(); addIter != addedLinks.end(); ++addIter)
    {
        // qDebug() << "Adding link" << (int) addIter.key() << (int) addIter.value() << " on offset" << hex << (int) dat.device()-> pos() << FN;

        // primIndexFiles.data_offset = dat.device()-> pos();

        dat << addIter.key();
        dat << addIter.value();

            // add primary index
        // primIndexFiles.the_index = addIter.key();
        // primIndexFiles.AddObjToIndex();
    }

    addedLinks.clear();

    dat_file.close();

    // primIndexFiles.CloseIndexFiles();

    return 0;
}


int EgDataNodesLinkType::LoadLinks()
{
     QFile dat_file;             // data file
     QDataStream dat;

     EgDataNodeIDtype leftNodeID;
     EgDataNodeIDtype rightNodeID;

     loadedLinks.clear();

     dat_file.setFileName(linkName + ".dln");

     if (!dat_file.open(QIODevice::ReadOnly)) // WriteOnly Append | QIODevice::Truncate
     {
         // qDebug() << "can't open file " << linkName + ".dln" << FN;
         return 1;
     }

     dat.setDevice(&dat_file);

     while(! dat.atEnd())
     {
         dat >> leftNodeID;
         dat >> rightNodeID;

         loadedLinks.insert(leftNodeID, rightNodeID);
     }

     dat_file.close();

     return 0;
}


