// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// This file contains the implementation of the Kugar KPart.


#include <kapp.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kinstance.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <qfile.h>
#include <kglobal.h>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "kugar.h"

#include "kugar_part.h"


KInstance *KugarFactory::s_instance = 0L;


// The part's entry point.

extern "C" void *init_libkugarpart()
{
    KGlobal::locale()->insertCatalogue("kugar");
    return new KugarFactory;
}


// The factory ctor.

KugarFactory::KugarFactory()
{
}


// The factory dtor.

KugarFactory::~KugarFactory()
{
	if (s_instance)
	{
		delete s_instance -> aboutData();
		delete s_instance;

		s_instance = 0L;
	}
}


// Create a new part.

QObject *KugarFactory::create(QObject *parent,const char *name,const char *,const QStringList &)
{
	QObject *obj = new KugarPart((QWidget*)parent,name);

	emit objectCreated(obj);

	return obj;
}


// Create a new instance.

KInstance *KugarFactory::instance()
{
	// Create it if it hasn't already been done.

	if (!s_instance)
	{
		KAboutData *about = new KAboutData(PACKAGE,
						   I18N_NOOP("Kugar"),
						   VERSION);

		s_instance = new KInstance(about);
	}

	return s_instance;
}


// The view ctor.

KugarPart::KugarPart(QWidget *parent,const char *name)
	: KParts::ReadOnlyPart(parent,name)
{
	setInstance(KugarFactory::instance());

	view = new MReportViewer(parent);

	connect(view,SIGNAL(preferedTemplate(const QString &)),
		     SLOT(slotPreferedTemplate(const QString &)));

	view -> setFocusPolicy(QWidget::ClickFocus);
	view -> show();

	setWidget(view);

	m_extension = new KugarBrowserExtension(this);

	setXMLFile("kugarpart.rc");

	// Define the actions.

	KStdAction::prior(view,SLOT(slotPrevPage()),actionCollection(),"kuPrevPage");
	KStdAction::next(view,SLOT(slotNextPage()),actionCollection(),"kuNextPage");
	KStdAction::firstPage(view,SLOT(slotFirstPage()),actionCollection(),"kuFirstPage");
	KStdAction::lastPage(view,SLOT(slotLastPage()),actionCollection(),"kuLastPage");
}


// The view dtor.

KugarPart::~KugarPart()
{
	closeURL();
}


// Open a data file.

bool KugarPart::openFile()
{
	bool ok = false;
	QFile f(m_file);

	if (f.open(IO_ReadOnly))
	{
		if (view -> setReportData(&f))
		{
			if (view -> renderReport())
				ok = true;
		}
		else
			KMessageBox::sorry(widget(),i18n("Invalid data file: %1").arg(m_file));

		f.close();
	}
	else
		KMessageBox::sorry(widget(),i18n("Unable to open data file: %1").arg(m_file));

	return ok;
}


// Close the data file.

bool KugarPart::closeURL()
{
	// Nothing to do.

	return true;
}


// Print the report.

void KugarPart::print()
{
	view -> printReport();
}


// Handle a prefered template in the data.  This is actually the only way to
// set the template when used as a browser extension.

void KugarPart::slotPreferedTemplate(const QString &tpl)
{
	KURL url(tpl);
	QString localtpl;
	bool isTemp = false;

	if (url.isMalformed())
	{
		if (tpl.find('/') >= 0)
			localtpl = tpl;
		else
			localtpl = kapp -> dirs() -> findResource("data","kugar/templates/" + tpl);
	}
	else
	{
		if (KIO::NetAccess::download(url,localtpl))
			isTemp = true;
		else
			KMessageBox::sorry(widget(),i18n("Unable to download template file: %1").arg(url.prettyURL()));
	}

	if (!localtpl.isNull())
	{
		QFile f(localtpl);

		if (f.open(IO_ReadOnly))
		{
			if (!view -> setReportTemplate(&f))
				KMessageBox::sorry(widget(),i18n("Invalid template file: %1").arg(localtpl));

			f.close();
		}
		else
			KMessageBox::sorry(widget(),i18n("Unable to open template file: %1").arg(localtpl));

		if (isTemp)
			KIO::NetAccess::removeTempFile(localtpl);
	}
}


// The browser extension ctor.

KugarBrowserExtension::KugarBrowserExtension(KugarPart *parent)
	: KParts::BrowserExtension(parent,"KugarBrowserExtension")
{
    emit enableAction( "print", true );
}


// The browser extension dtor.

KugarBrowserExtension::~KugarBrowserExtension()
{
}


// This is the browser hook to print the report.

void KugarBrowserExtension::print()
{
	((KugarPart *)parent()) -> print();
}
#include "kugar_part.moc"
