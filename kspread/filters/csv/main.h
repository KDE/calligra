/******************************************************************/
/* CSVFilter - (c) 1999 by David Faure <faure@kde.org>            */
/* License: GNU GPL                                               */
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

#include <csvfilter.h>

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

    void filter(KOffice::Filter::Data& data,const QCString & _from,const QCString & _to);
};
#endif  // __main_h__
