#include "kspread_factory.h"
#include "kspread_doc.h"

#include "KSpreadAppIface.h"

#include <kinstance.h>
#include <kstddirs.h>

extern "C"
{
    void* init_libkspread()
    {
	return new KSpreadFactory;
    }
};

KInstance* KSpreadFactory::s_global = 0;

KSpreadFactory::KSpreadFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "kspread" );
    s_global->dirs()->addResourceType( "toolbar",
				       KStandardDirs::kde_default("data") + "koffice/toolbar/");
    s_global->dirs()->addResourceType( "extensions", KStandardDirs::kde_default("data") + "koffice/extensions/");
    s_global->dirs()->addResourceType( "extensions", KStandardDirs::kde_default("data") + "kspread/extensions/");
    s_global->dirs()->addResourceType( "table-styles", KStandardDirs::kde_default("data") + "kspread/tablestyles/");
    s_global->dirs()->addResourceType( "scripts", KStandardDirs::kde_default("data") + "koffice/scripts/");
    s_global->dirs()->addResourceType( "scripts", KStandardDirs::kde_default("data") + "kspread/scripts/");

    (void)new KSpreadAppIface;
}

KSpreadFactory::~KSpreadFactory()
{
}

QObject* KSpreadFactory::create( QObject* parent, const char* name, const char* classname, const QStringList & )
{
/*    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KSpreadFactory: parent does not inherit KoDocument");
	return 0;
    }
*/
//    return new KSpreadDoc( (KoDocument*)parent, name );

  bool bWantKoDocument = ( strcmp( classname, "KofficeDocument" ) == 0 ); 
 
  KSpreadDoc *doc = new KSpreadDoc( parent, name, !bWantKoDocument );
  
  if ( !bWantKoDocument )
    doc->setReadWrite( false );

  emit objectCreated( doc );
  
  return doc;
}

KInstance* KSpreadFactory::global()
{
    return s_global;
}

#include "kspread_factory.moc"
