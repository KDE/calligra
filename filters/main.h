/******************************************************************/
/* OLEFilter - (c) by Werner Trobin 1999                          */
/* Version: 0.0.1                                                 */
/* Author: Werner Trobin                                          */
/* E-Mail: wtrobin@carinthia.com                                  */
/* Homepage: http://members.carinthia.com/wtrobin/                */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Filter (header)                                        */
/******************************************************************/

#ifndef __main_h__
#define __main_h__

#include <kom.h>
#include <komComponent.h>
#include <komAutoLoader.h>
#include <koffice.h>
#include <koApplication.h>

#include <qstring.h>
#include <qcstring.h>

#include <olefilter.h>
#include <myfile.h>

/******************************************************************/
/* MyApplication                                                  */
/******************************************************************/

class MyApplication : public KoApplication {

    Q_OBJECT

public:
    MyApplication(int &argc,char **argv);

    void start();
};

/******************************************************************/
/* Factory                                                        */
/******************************************************************/

class Factory : virtual public KOffice::FilterFactory_skel {

public:
    Factory(const CORBA::ORB::ObjectTag &_tag);
    Factory(CORBA::Object_ptr _obj);

    KOffice::Filter_ptr create();
};

/******************************************************************/
/* Filter                                                         */
/******************************************************************/

class Filter : virtual public KOMComponent,
               virtual public KOffice::Filter_skel {

public:
    Filter();

    void filter(KOffice::Filter::Data& data,const char *_from,const char *_to);

private:
    OLEFilter *myOLEFilter;
    myFile docFile;
};
#endif  // __main_h__
