// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// This file contains the definition of the interface to the Kugar KPart.


#ifndef _KUGAR_FACTORY_H
#define _KUGAR_FACTORY_H


#include <KoFactory.h>
#include <kaboutdata.h>

class KInstance;


class KugarFactory : public KoFactory
{
    Q_OBJECT
public:
    KugarFactory( QObject* parent = 0 );
    ~KugarFactory();

    virtual KParts::Part *createPartObject( QWidget *parentWidget = 0,
                                            QObject *parent = 0, const char *classname = "KoDocument",
                                            const QStringList &args = QStringList() );

    static KInstance* global();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();
private:
    static KInstance* s_instance;
    static KAboutData* s_aboutdata;
};

#endif
