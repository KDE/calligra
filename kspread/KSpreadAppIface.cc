#include "KSpreadAppIface.h"

#include "kspread_doc.h"

#include <dcopclient.h>

KSpreadAppIface::KSpreadAppIface()
    : DCOPObject( "Application" )
{
}

DCOPRef KSpreadAppIface::createDoc()
{
    KSpreadDoc* doc = new KSpreadDoc();
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
    
    QList<KSpreadDoc>& lst = KSpreadDoc::documents();
    QListIterator<KSpreadDoc> it( lst );
    for( ; it.current(); ++it )
    {
	map[ QString( it.current()->name() ) ] =
	 DCOPRef( kapp->dcopClient()->appId(), it.current()->dcopObject()->objId() );
    }
    
    return map;
}

DCOPRef KSpreadAppIface::document( const QString& name )
{
    QList<KSpreadDoc>& lst = KSpreadDoc::documents();
    QListIterator<KSpreadDoc> it( lst );
    for( ; it.current(); ++it )
	if ( name == it.current()->name() )
	    return DCOPRef( kapp->dcopClient()->appId(), it.current()->dcopObject()->objId() );
    
    return DCOPRef();
}

