// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// This file contains the implementation of the Kugar KPart.


#include <klocale.h>
#include <kstandarddirs.h>
#include <kinstance.h>
#include <kaboutdata.h>
#include <kglobal.h>
#include <kiconloader.h>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "kugar_factory.h"
#include "kugar_part.h"
#include "kugar_about.h"

KInstance *KugarFactory::s_instance = 0L;
KAboutData *KugarFactory::s_aboutdata = 0L;


// The part's entry point.

K_EXPORT_COMPONENT_FACTORY( libkugarpart, KugarFactory )


// The factory ctor.

KugarFactory::KugarFactory( QObject *parent ) : KoFactory( parent )
{
    global();
}

KInstance* KugarFactory::global()
{
    if ( !s_instance )
    {
        s_instance = new KInstance( aboutData() );
        s_instance->iconLoader() ->addAppDir( "koffice" );
        s_instance->iconLoader() ->addAppDir( "kugar" );

    }
    return s_instance;
}

// The factory dtor.

KugarFactory::~KugarFactory()
{
    delete s_instance;
    s_instance = 0;
    delete s_aboutdata;
    s_aboutdata = 0;
}

KAboutData *KugarFactory::aboutData()
{
    if ( !s_aboutdata )
        s_aboutdata = newKugarAboutData();
    return s_aboutdata;
}

// Create a new part.

KParts::Part* KugarFactory::createPartObject( QWidget *parentWidget,
        QObject* parent, const char* classname, const QStringList & data )
{
    QString forcedUserTemplate;
    for ( QStringList::const_iterator it = data.begin();it != data.end();++it )
    {
        QString tmp = ( *it );
        if ( tmp.startsWith( "template=" ) )
            forcedUserTemplate = tmp.right( tmp.length() - 9 );
    }

    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );
    // parentWidget and widgetName are used by KoDocument for the "readonly+singleView" case.
    KugarPart *part = new KugarPart( parentWidget, parent, !bWantKoDocument );

    //  if ( !bWantKoDocument )
    part->setReadWrite( false );

    part->setForcedUserTemplate( forcedUserTemplate );
    return part;
    //return ( new KugarPart(parentWidget,name,forcedUserTemplate));
}


#include "kugar_factory.moc"
