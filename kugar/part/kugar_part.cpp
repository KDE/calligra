// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// This file contains the implementation of the Kugar KPart.


#include <kapplication.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kinstance.h>
#include <kaboutdata.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <qfile.h>
#include <kglobal.h>
#include <kiconloader.h>

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "kugar.h"

#include "kugar_part.h"


KInstance *KugarFactory::s_instance = 0L;
KAboutData *KugarFactory::s_aboutdata = 0L;


// The part's entry point.

extern "C" void *init_libkugarpart()
{
    KGlobal::locale()->insertCatalogue("kugar");
    return new KugarFactory;
}


// The factory ctor.

KugarFactory::KugarFactory(QObject *parent, const char* name):KoFactory(parent,name)
{
	global();	
}

KInstance* KugarFactory::global()
{
    if ( !s_instance )
    {
        s_instance = new KInstance(aboutData());
        s_instance->iconLoader()->addAppDir("koffice");
        s_instance->iconLoader()->addAppDir("kugar");

    }
    return s_instance;
}

// The factory dtor.

KugarFactory::~KugarFactory()
{
	delete s_instance;
	s_instance=0;
	delete s_aboutdata;
	s_aboutdata=0;
}

KAboutData *KugarFactory::aboutData()
{
	if (!s_aboutdata)
	s_aboutdata=new KAboutData(PACKAGE,
                                                   I18N_NOOP("Kugar"),
                   	                               VERSION);
	return s_aboutdata;
}

// Create a new part.

KParts::Part* KugarFactory::createPartObject( QWidget *parentWidget, const char *,
        QObject* parent, const char* name, const char*, const QStringList & data)
{
	QString forcedUserTemplate;
	for (QStringList::const_iterator it=data.begin();it!=data.end();++it)
	{
		QString tmp=(*it);
		if (tmp.startsWith("template="))
			forcedUserTemplate=tmp.right(tmp.length()-9);
	}
	return ( new KugarPart(parentWidget,name,forcedUserTemplate));
}


// The view ctor.

KugarPart::KugarPart(QWidget *parent,const char *name,const QString &forcedUserTemplate)
	: KParts::ReadOnlyPart(parent,name)
{
	m_forcedUserTemplate=forcedUserTemplate;
	setInstance(KugarFactory::global());

	view = new MReportViewer(parent);

	connect(view,SIGNAL(preferedTemplate(const QString &)),
		     SLOT(slotPreferedTemplate(const QString &)));

	view -> setFocusPolicy(QWidget::ClickFocus);
	view -> show();

	setWidget(view);

	m_extension = new KugarBrowserExtension(this);


	// Define the actions.

	KStdAction::prior(view,SLOT(slotPrevPage()),actionCollection(),"kuPrevPage");
	KStdAction::next(view,SLOT(slotNextPage()),actionCollection(),"kuNextPage");
	KStdAction::firstPage(view,SLOT(slotFirstPage()),actionCollection(),"kuFirstPage");
	KStdAction::lastPage(view,SLOT(slotLastPage()),actionCollection(),"kuLastPage");

	setXMLFile("kugarpart.rc");

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
	KURL url(m_forcedUserTemplate.isEmpty()?tpl:m_forcedUserTemplate);
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
