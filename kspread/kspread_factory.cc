#include "kspread_factory.h"
#include "kspread_doc.h"

#include <klibglobal.h>
#include <kstddirs.h>

extern "C"
{
    void* init_libkspread()
    {
	return new KSpreadFactory;
    }
};

KLibGlobal* KSpreadFactory::s_global = 0;

KSpreadFactory::KSpreadFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KLibGlobal( "kspread" );
    s_global->dirs()->addResourceType( "toolbar",
				       KStandardDirs::kde_default("data") + "koffice/toolbar/");
}

KSpreadFactory::~KSpreadFactory()
{
}

QObject* KSpreadFactory::create( QObject* parent, const char* name, const char* classname )
{
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KSpreadFactory: parent does not inherit KoDocument");
	return 0;
    }

    return new KSpreadDoc( (KoDocument*)parent, name );
}

KLibGlobal* KSpreadFactory::global()
{
    return s_global;
}

#include "kspread_factory.moc"
