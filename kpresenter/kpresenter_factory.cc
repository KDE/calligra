#include "kpresenter_factory.h"
#include "kpresenter_doc.h"
#include "preview.h"

#include <kimgio.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kinstance.h>

#include <qstringlist.h>

extern "C"
{
    void* init_kpresenter()
    {
	/**
	 * Initialize KPresenter stuff
	 */
		
	// Image IO handler
	KFilePreviewDialog::registerPreviewModule( "wmf", wmfPreviewHandler, PreviewPixmap );
	KFilePreviewDialog::registerPreviewModule( "WMF", wmfPreviewHandler, PreviewPixmap );
	
	QStringList list = KImageIO::types(KImageIO::Reading);
	QStringList::ConstIterator it;
	for (it = list.begin(); it != list.end(); it++)
	    KFilePreviewDialog::registerPreviewModule( *it, pixmapPreviewHandler, PreviewPixmap );

	return new KPresenterFactory;
    }
};


KInstance* KPresenterFactory::s_global = 0;

KPresenterFactory::KPresenterFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "kpresenter" );

    s_global->dirs()->addResourceType("kpresenter_template",
				      KStandardDirs::kde_default("data") + "kpresenter/templates/");
    s_global->dirs()->addResourceType("autoforms",
				      KStandardDirs::kde_default("data") + "kpresenter/autoforms/");
    s_global->dirs()->addResourceType("toolbar",
				      KStandardDirs::kde_default("data") + "koffice/toolbar/");
    s_global->dirs()->addResourceType("toolbar",
				      KStandardDirs::kde_default("data") + "kpresenter/toolbar/");
}

KPresenterFactory::~KPresenterFactory()
{
    delete s_global;
}

QObject* KPresenterFactory::create( QObject* parent, const char* name, const char* classname, const QStringList & )
{
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KPresenterFactory: parent does not inherit KoDocument");
	return 0;
    }
    KPresenterDoc *doc = new KPresenterDoc( (KoDocument*)parent, name );
    emit objectCreated(doc);
    return doc;
}

KInstance* KPresenterFactory::global()
{
    return s_global;
}

#include "kpresenter_factory.moc"
