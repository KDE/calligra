// Copyright (c) 1999 Mutiny Bay Software                         
// Copyright (c) 2000 Phil Thompson <phil@river-bank.demon.co.uk>
//
// This file contains the implementation of the KDE GUI wrapper around the
// report viewer widget.


#include <kapp.h>
#include <klocale.h>
#include <kaction.h>
#include <kstdaction.h>
#include <ktoolbar.h>
#include <kstddirs.h>
#include <kurl.h>
#include <kmessagebox.h>
#include <kio/netaccess.h>
#include <qfile.h>

#include "report.h"


// The ctor.

Report::Report() : KMainWindow(0L)
{
	// Create the viewer widget.

	rptviewer = new MReportViewer(this);

	connect(rptviewer,SIGNAL(preferedTemplate(const QString &)),
			  SLOT(slotPreferedTemplate(const QString &)));

	setCentralWidget(rptviewer);

	// Create the user interface.

	KStdAction::print(this,SLOT(slotPrint()),actionCollection());
	KStdAction::quit(this,SLOT(slotFileQuit()),actionCollection());

	KStdAction::prior(rptviewer,SLOT(slotPrevPage()),actionCollection());
	KStdAction::next(rptviewer,SLOT(slotNextPage()),actionCollection());
	KStdAction::firstPage(rptviewer,SLOT(slotFirstPage()),actionCollection());
	KStdAction::lastPage(rptviewer,SLOT(slotLastPage()),actionCollection());

	KStdAction::showToolbar(this,SLOT(slotViewToolBar()),actionCollection());
	KStdAction::showStatusbar(this,SLOT(slotViewStatusBar()),actionCollection());

	statusBar();

	createGUI();
}


// The dtor.

Report::~Report()
{
}


// Render the report.

void Report::renderReport()
{
	rptviewer -> renderReport();
}


// Set the name of the data file.

void Report::setReportData(const QString &data)
{
	QFile f(data);

	if (f.open(IO_ReadOnly))
	{
		if (!rptviewer -> setReportData(&f))
			KMessageBox::sorry(this,i18n("Invalid data file: %1").arg(data));

		f.close();
	}
	else
		KMessageBox::sorry(this,i18n("Unable to open data file: %1").arg(data));
}


// Set the name of the template file.

void Report::setReportTemplate(const QString &tpl)
{
	KURL url(tpl);
	QString localtpl;
	bool isTemp = false;

	if (url.isMalformed())
	{
		if (tpl.find('/') >= 0)
			localtpl = tpl;
		else
			localtpl = kapp -> dirs() -> findResource("appdata","templates/" + tpl);
	}
	else
	{
		if (KIO::NetAccess::download(url,localtpl))
			isTemp = true;
		else
			KMessageBox::sorry(this,i18n("Unable to download template file: %1").arg(url.prettyURL()));
	}

	if (!localtpl.isNull())
	{
		QFile f(localtpl);

		if (f.open(IO_ReadOnly))
		{
			if (!rptviewer -> setReportTemplate(&f))
				KMessageBox::sorry(this,i18n("Invalid template file: %1").arg(localtpl));

			f.close();
		}
		else
			KMessageBox::sorry(this,i18n("Unable to open template file: %1").arg(localtpl));

		if (isTemp)
			KIO::NetAccess::removeTempFile(localtpl);
	}
}


// Exit the application.

void Report::slotFileQuit()
{
	kapp -> quit();
}


// Print the report.

void Report::slotPrint()
{
	rptviewer -> printReport();
}


// Toggle the toolbar display.

void Report::slotViewToolBar()
{
	KToolBar *tb = toolBar("mainToolBar");

	if (tb -> isVisible())
		tb -> hide();
	else
		tb -> show();
}


// Toggle the statusbar display.

void Report::slotViewStatusBar()
{
	if (statusBar() -> isVisible())
		statusBar() -> hide();
	else
		statusBar() -> show();
}


// Return the preferred size.

QSize Report::sizeHint() const
{
	// This should also allow for the bits KMainWindow adds.

	return rptviewer -> sizeHint();
}


// Handle a request from the data to use a particular template.

void Report::slotPreferedTemplate(const QString &tplname)
{
	setReportTemplate(tplname);
}
#include "report.moc"
