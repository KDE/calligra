#include "kword_factory.h"
#include "kword_doc.h"
#include "preview.h"

#include <kimgio.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <klibglobal.h>

#include <qstringlist.h>

extern "C"
{
    void* init_kword()
    {
	/**
	 * Initialize KWord stuff
	 */
		
	// Image IO handler
	KFilePreviewDialog::registerPreviewModule( "wmf", wmfPreviewHandler, PreviewPixmap );
	KFilePreviewDialog::registerPreviewModule( "WMF", wmfPreviewHandler, PreviewPixmap );
	
	QStringList list = KImageIO::types(KImageIO::Reading);
	QStringList::ConstIterator it;
	for (it = list.begin(); it != list.end(); it++)
	    KFilePreviewDialog::registerPreviewModule( *it, pixmapPreviewHandler, PreviewPixmap );

	return new KWordFactory;
    }
};


KLibGlobal* KWordFactory::s_global = 0;

KWordFactory::KWordFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KLibGlobal( "kword" );

    s_global->dirs()->addResourceType( "kword_template",
				       KStandardDirs::kde_default("data") + "kword/templates/");
    s_global->dirs()->addResourceType( "toolbar",
				       KStandardDirs::kde_default("data") + "koffice/toolbar/");
    s_global->dirs()->addResourceType( "toolbar",
				       KStandardDirs::kde_default("data") + "kformula/pics/");
}

KWordFactory::~KWordFactory()
{
}

QObject* KWordFactory::create( QObject* parent, const char* name, const char* classname )
{
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KWordFactory: parent does not inherit KoDocument");
	return 0;
    }

    return new KWordDocument( (KoDocument*)parent, name );
}

KLibGlobal* KWordFactory::global()
{
    return s_global;
}

#include "kword_factory.moc"
