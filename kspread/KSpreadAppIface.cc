#include "KSpreadAppIface.h"

#include "kspread_doc.h"

#include <dcopclient.h>
#include <kapplication.h>

KSpreadAppIface::KSpreadAppIface()
    : DCOPObject( "Application" )
{
}

DCOPRef KSpreadAppIface::createDoc()
{
    KSpreadDoc* doc = new KSpreadDoc();
    doc->setInitDocFlags( KoDocument::InitDocFileNew );
    doc->initDoc();

    return DCOPRef( kapp->dcopClient()->appId(), doc->dcopObject()->objId() );
}

DCOPRef KSpreadAppIface::createDoc( const QString& name )
{
    // ######### Torben: Check for duplicate names here
    KSpreadDoc* doc = new KSpreadDoc( 0, name.latin1() );
    doc->initDoc();

    return DCOPRef( kapp->dcopClient()->appId(), doc->dcopObject()->objId() );
}

QMap<QString,DCOPRef> KSpreadAppIface::documents()
{
    QMap<QString,DCOPRef> map;

    QValueList<KSpreadDoc*> lst = KSpreadDoc::documents();
    QValueListIterator<KSpreadDoc*> it = lst.begin();
    for( ; it != lst.end(); ++it )
    {
	map[ QString( (*it)->name() ) ] =
	 DCOPRef( kapp->dcopClient()->appId(), (*it)->dcopObject()->objId() );
    }

    return map;
}

DCOPRef KSpreadAppIface::document( const QString& name )
{
    QValueList<KSpreadDoc*> lst = KSpreadDoc::documents();
    QValueListIterator<KSpreadDoc*> it = lst.begin();
    for( ; it != lst.end(); ++it )
	if ( name == (*it)->name() )
	    return DCOPRef( kapp->dcopClient()->appId(), (*it)->dcopObject()->objId() );

    return DCOPRef();
}

